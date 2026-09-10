
#include "PlaybackController.h"
#include <QDebug>
#include <QtEndian>
PlaybackController::PlaybackController(QObject *parent)
    : QObject(parent) {}

bool PlaybackController::loadPCAP(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) { qWarning() << "Failed to open PCAP file:" << filePath; return false; }
    frames.clear(); currentFrame = 0; accumulatedAngle = 0; lastAzimuth = 0; collectingFrame = false;
    QDataStream stream(&file); stream.setByteOrder(QDataStream::LittleEndian); file.seek(24);
    QList<PointXYZI> framePoints;

    while (!stream.atEnd()) {
        if (file.bytesAvailable() < 16) break;
        char headerBuf[16]; if (file.read(headerBuf,16) != 16) break;
        const quint32 incl_len = *reinterpret_cast<quint32*>(&headerBuf[8]);
        if (incl_len < 42 + 1248) { file.seek(file.pos() + incl_len); continue; }
        const QByteArray fullPacket = file.read(incl_len);
        if (fullPacket.size() < 42 + 1248) continue;
        const QByteArray payload = fullPacket.mid(42,1248);
        if (quint8(payload[0]) != 0x55 || quint8(payload[1]) != 0xAA || quint8(payload[2]) != 0x05 || quint8(payload[3]) != 0x5A) continue;
        if (quint8(payload[31]) != 0x06 || quint8(payload[32]) != 0x03) continue;
        if (quint8(payload[1246]) != 0x00 || quint8(payload[1247]) != 0xFF) continue;
        const QByteArray blk = payload.mid(42,1200);

        float azimuths[12] = {};
        bool blocksValid = true;
        for (int i=0;i<12;++i) {
            const QByteArray b=blk.mid(i*100,100);
            if (b.size()<100 || quint8(b[0])!=0xFF || quint8(b[1])!=0xEE) { blocksValid=false; break; }
            azimuths[i]=((quint8(b[2])<<8)|quint8(b[3]))/100.0f;
        }
        if (!blocksValid) continue;

        // 导出的 PCAP 通常只包含 MSOP，无法从文件内获知 DIFOP 回波模式。
        // Helios16 出厂默认最强单回波，因此按单回波的第二组角度插值解析。
        for (int i=0;i<12;++i) {
            const QByteArray b=blk.mid(i*100,100); const float az1=azimuths[i];
            float nextAz;
            if (i<11) nextAz=azimuths[i+1];
            else { float delta=az1-azimuths[10]; if(delta<0) delta+=360.0f; nextAz=az1+delta; if(nextAz>=360.0f) nextAz-=360.0f; }
            float adjustedNext=nextAz; if(adjustedNext<az1) adjustedNext+=360.0f;
            float az2=az1+(adjustedNext-az1)*0.5f; if(az2>=360.0f) az2-=360.0f;

            if(!collectingFrame){ collectingFrame=true; accumulatedAngle=0.0f; }
            else { float delta=az1-lastAzimuth; if(delta<0) delta+=360.0f; accumulatedAngle+=delta; }
            for(int j=0;j<16;++j) framePoints.append(parseChannel(b.mid(4+j*3,3),az1,j));
            for(int j=0;j<16;++j) framePoints.append(parseChannel(b.mid(4+(16+j)*3,3),az2,j));
            lastAzimuth=az1;
            if(accumulatedAngle>=360.0f){ frames.append(framePoints); framePoints.clear(); collectingFrame=false; accumulatedAngle=0.0f; }
        }
    }
    if(!framePoints.isEmpty()) frames.append(framePoints);
    file.close(); return !frames.isEmpty();
}

double PlaybackController::parseHeader(const QByteArray &h)
{
    uint64_t sec = 0; uint32_t us = 0;
    for (int i = 0; i < 6; ++i) sec = (sec << 8) | quint8(h[20 + i]);
    for (int i = 6; i < 10; ++i) us = (us << 8) | quint8(h[20 + i]);
    return sec + us / 1e6;
}

PointXYZI PlaybackController::parseChannel(const QByteArray &d, float azimuth, int channel)
{
    if (d.size() < 3 || channel < 0 || channel >= 16) return {};
    const quint16 dist = (quint8(d[0]) << 8) | quint8(d[1]);
    const quint8 inten = quint8(d[2]);
    const double dd = dist * 0.0025;
    double correctedAzimuth = azimuth + COR_HOR_ANG.value(channel, 0.0f);
    while (correctedAzimuth >= 360.0) correctedAzimuth -= 360.0;
    while (correctedAzimuth < 0.0) correctedAzimuth += 360.0;
    const CartesianCoordinates pos = helios16ToCartesian(dd, correctedAzimuth, COR_VERT_ANG.value(channel, 0.0f));
    return {static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z), static_cast<float>(inten)};
}

void PlaybackController::start()
{
    if (isPlaying || frames.isEmpty()) return;

    if (!playbackTimer) {
        playbackTimer = new QTimer(this);
        connect(playbackTimer, &QTimer::timeout, this, &PlaybackController::onPlaybackTick);
    }

    int intervalMs = static_cast<int>(1000.0 / (playbackSpeed * 10));
    playbackTimer->start(intervalMs);
    isPlaying = true;
}

void PlaybackController::pause()
{
    if (playbackTimer) playbackTimer->stop();
    isPlaying = false;
}

void PlaybackController::seekToFrame(int index)
{
    if (index >= 0 && index < frames.size()) {
        currentFrame = index;
        emit frameReady(frames[index], index);
    }
}

void PlaybackController::setLoop(bool enabled)
{
    loop = enabled;
}

void PlaybackController::setSpeed(double speed)
{
    playbackSpeed = speed;
    if (isPlaying && playbackTimer) {
        playbackTimer->setInterval(static_cast<int>(1000.0 / (playbackSpeed * 10)));
    }
}

int PlaybackController::frameCount() const
{
    return frames.size();
}

void PlaybackController::onPlaybackTick()
{
    if (currentFrame >= frames.size()) {
        if (loop) currentFrame = 0;
        else { pause(); return; }
    }

    emit frameReady( frames[currentFrame], currentFrame);
    ++currentFrame;
}
