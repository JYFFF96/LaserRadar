#include "udpreceiver.h"
#include <QDebug>
#include <QDateTime>
#include <QtEndian>
#include <QDir>
#include <cmath>

/******************  UdpReceiver.cpp (修正版)  *******************/

UdpReceiver::UdpReceiver(QObject *parent) : QObject(parent) {
    exporter = new Exporter(this);
}

UdpReceiver::~UdpReceiver()
{
    delete exporter;
    if (running) stop();
}

/* -------------------------- 启停 -------------------------- */
void UdpReceiver::start(quint16 dataPort, quint16 devInfoPort)
{
    if (running) return;

    socket1 = new QUdpSocket(this);
    socket2 = new QUdpSocket(this);

    bool ok1 = socket1->bind(QHostAddress::AnyIPv4, dataPort, QUdpSocket::ShareAddress);
    bool ok2 = socket2->bind(QHostAddress::AnyIPv4, devInfoPort, QUdpSocket::ShareAddress);

    if (!ok1 || !ok2) {
        qWarning() << "UDP bind failed:"
                   << "dataPort=" << dataPort << socket1->errorString()
                   << "devInfoPort=" << devInfoPort << socket2->errorString();

        stop();
        return;
    }

    connect(socket1, &QUdpSocket::readyRead, this, &UdpReceiver::onReadyRead1);
    connect(socket2, &QUdpSocket::readyRead, this, &UdpReceiver::onReadyRead2);

    running = true;
}

void UdpReceiver::stop()
{
    if (!running) return;
    if (socket1) { socket1->close(); socket1->deleteLater(); socket1=nullptr; }
    if (socket2) { socket2->close(); socket2->deleteLater(); socket2=nullptr; }
    running=false; collectingFrame=false;
}

void UdpReceiver::closeConnection() { stop(); }

/* --------------------- 导出控制 ---------------------- */
void UdpReceiver::onExportRequested(const ExportParams &p)
{
    currentExportParams=p; exportFrameCounter=0; exportActive=true;
    dirPath = p.filePath;
    if (p.fileType==singleFile && p.exportType==CSV) {
        csvFile.setFileName(p.filePath);
        if (csvFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
            csvStream=new QTextStream(&csvFile);
            *csvStream<<"time,x,y,z,azimuth,distance,intensity,channel\n";
        }
    } else if (p.fileType==singleFile && p.exportType==LAS) {
        lasOfs.open(p.filePath.toStdString(),std::ios::binary);
        if (lasOfs.is_open()) {
            lasHeader.SetDataFormatId(liblas::ePointFormat1);
            lasHeader.SetScale(0.001,0.001,0.001);
            lasWriter=std::make_unique<liblas::Writer>(lasOfs,lasHeader);
        }
    }
}

void UdpReceiver::stopExport()
{
    exportActive=false;
    if (csvStream){ delete csvStream; csvStream=nullptr; }
    if (csvFile.isOpen()) csvFile.close();
    if (lasWriter) lasWriter.reset();
    if (lasOfs.is_open()) lasOfs.close();
    emit exportFinished();
}

/* ------------------- 数据接收 --------------------- */
void UdpReceiver::onReadyRead1()
{
    while(socket1 && socket1->hasPendingDatagrams()){
        QByteArray d; d.resize(socket1->pendingDatagramSize());
        socket1->readDatagram(d.data(),d.size());
        QMutexLocker lock(&mutex);
        if(exportActive)
            pcapPacketsBuffer.append(d);
        parseMSOP(d);
    }
}

void UdpReceiver::onReadyRead2()
{
    while(socket2 && socket2->hasPendingDatagrams()){
        QByteArray d; d.resize(socket2->pendingDatagramSize());
        socket2->readDatagram(d.data(),d.size());
        parseVerticalAndHorizontalAngles(d);
        parseDIFOPPacket(d);
    }
}

/* ------------------ 解析与帧组装 ------------------ */
void UdpReceiver::parseMSOP(const QByteArray &data)
{
    // Helios16 MSOP: 42-byte Header + 1200-byte Data Blocks + 6-byte Tail = 1248 bytes.
    if (data.size() != 1248) return;
    if (quint8(data[0]) != 0x55 || quint8(data[1]) != 0xAA ||
        quint8(data[2]) != 0x05 || quint8(data[3]) != 0x5A) return;
    if (quint8(data[31]) != 0x06 || quint8(data[32]) != 0x03) return; // Helios / Helios16
    if (quint8(data[1246]) != 0x00 || quint8(data[1247]) != 0xFF) return;
    parseHeader(data.left(42));
    parseDataBlock(data.mid(42, 1200));
}

void UdpReceiver::parseHeader(const QByteArray &h)
{
    uint64_t sec=0; uint32_t us=0;
    for(int i=0;i<6;++i) sec=(sec<<8)|quint8(h[20+i]);
    for(int i=6;i<10;++i)us=(us<<8)|quint8(h[20+i]);
    currentTimestamp=sec+us/1e6;
}

void UdpReceiver::parseDataBlock(const QByteArray &blk)
{
    if (blk.size() < 1200 || COR_VERT_ANG.size() < 16 || COR_HOR_ANG.size() < 16) return;

    float azimuths[12] = {};
    for (int i = 0; i < 12; ++i) {
        const QByteArray b = blk.mid(i * 100, 100);
        if (b.size() < 100 || quint8(b[0]) != 0xFF || quint8(b[1]) != 0xEE) return;
        azimuths[i] = ((quint8(b[2]) << 8) | quint8(b[3])) / 100.0f;
    }

    for (int i = 0; i < 12; ++i) {
        const QByteArray b = blk.mid(i * 100, 100);
        const float az1 = azimuths[i];

        // 单回波模式下 data17~32 是第二组 16 线采样，需要按手册在相邻 Block 间插值。
        // 双回波模式下 data17~32 是同一次采样的第二回波，与 data1~16 共用方位角。
        float az2 = az1;
        if (returnMode != 0x00) {
            float nextAz;
            if (i < 11) {
                nextAz = azimuths[i + 1];
            } else {
                float prev = azimuths[10];
                float delta = az1 - prev;
                if (delta < 0.0f) delta += 360.0f;
                nextAz = az1 + delta;
                if (nextAz >= 360.0f) nextAz -= 360.0f;
            }
            float adjustedNext = nextAz;
            if (adjustedNext < az1) adjustedNext += 360.0f;
            az2 = az1 + (adjustedNext - az1) * 0.5f;
            if (az2 >= 360.0f) az2 -= 360.0f;
        }

        if (!collectingFrame) {
            collectingFrame = true;
            accumulatedAngle = 0.0f;
        } else {
            float delta = az1 - lastAzimuth;
            if (delta < 0.0f) delta += 360.0f;
            accumulatedAngle += delta;
        }

        for (int j = 0; j < 16; ++j)
            parseChannelData(b.mid(4 + j * 3, 3), az1, j, 0.0, currentTimestamp);
        for (int j = 0; j < 16; ++j)
            parseChannelData(b.mid(4 + (16 + j) * 3, 3), az2, j, 0.0, currentTimestamp);

        lastAzimuth = az1;
        if (accumulatedAngle >= 360.0f) {
            emit dataUpdated(pointList);
            emit pointDataUpdated(pointDataList);
            if (exportActive) handleExportFrame();
            pointList.clear(); pointDataList.clear(); pcapPacketsBuffer.clear();
            collectingFrame = false; accumulatedAngle = 0.0f;
        }
    }
}

void UdpReceiver::parseChannelData(const QByteArray &d,float az,int idx,double R,double t)
{
    Q_UNUSED(R);
    if (d.size() < 3 || idx < 0 || idx >= 16) return;
    const quint16 dist = (quint8(d[0]) << 8) | quint8(d[1]);
    const quint8 inten = quint8(d[2]);
    const double dd = dist * 0.0025; // Helios16: 0.25 cm = 0.0025 m

    // 应用每通道水平偏移角校准，再按与 Fairy 48 线版本一致的工程坐标系输出。
    double correctedAzimuth = az + COR_HOR_ANG.value(idx, 0.0f);
    while (correctedAzimuth >= 360.0) correctedAzimuth -= 360.0;
    while (correctedAzimuth < 0.0) correctedAzimuth += 360.0;
    const CartesianCoordinates pos = helios16ToCartesian(dd, correctedAzimuth, COR_VERT_ANG[idx]);

    pointList.append({static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z), static_cast<float>(inten)});
    pointDataList.append({t, pos.x, pos.y, pos.z, static_cast<float>(correctedAzimuth), dd, static_cast<float>(inten), idx});
}

bool UdpReceiver::parseVerticalAndHorizontalAngles(const QByteArray &d)
{
    if(d.size()<612) return false;
    QVector<float> v(16),h(16); const uint8_t* r=(uint8_t*)d.constData();
    for (int i=0; i<16; ++i) {
        int vo=468+i*3, ho=564+i*3;
        v[i]=parseSignedAngle3Bytes(r[vo],r[vo+1],r[vo+2]);
        h[i]=parseSignedAngle3Bytes(r[ho],r[ho+1],r[ho+2]); }
    if (COR_VERT_ANG.count() >=16) {
        for (int i = 0; i < 16; ++i) {
            if (v[i] != COR_VERT_ANG[i])
                qDebug() << "--------------------";
        }
    }
    COR_VERT_ANG=v;
    COR_HOR_ANG=h;
    return true;
}

float UdpReceiver::parseSignedAngle3Bytes(uint8_t s,uint8_t h,uint8_t l)
{ int v=(h<<8)|l; return s==0x01?-v*0.01f:v*0.01f; }

/* ------------------- 导出核心 ------------------- */
void UdpReceiver::handleExportFrame()
{
    if (currentExportParams.fileType == singleFile) {
        appendToSingleFile();
    } else {
        currentExportParams.filePath = dirPath + "/" +QString::number(exportFrameCounter);
    }

    ++exportFrameCounter;

    bool finish = false;
    if (currentExportParams.frameType == currentFrame)
        finish = true;
    if (currentExportParams.frameType == rangeFrame &&
        exportFrameCounter > currentExportParams.endFrame - currentExportParams.startFrame)
        finish = true;
    // 发出异步写入信号（Exporter 线程处理）
    // emit exportFrameReady(currentExportParams, pointDataList, pointList, pcapPacketsBuffer);
    qDebug() << "exportFrameReady : " << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    exporter->enqueueFrame(currentExportParams, pointDataList, pointList, pcapPacketsBuffer);

    if (finish) stopExport();
    pointList.clear();
    pointDataList.clear();
    pcapPacketsBuffer.clear();
    collectingFrame = false;
}

void UdpReceiver::appendToSingleFile()
{
    if(currentExportParams.exportType==CSV && csvStream){
        for(auto &pt:pointDataList)
            *csvStream<<pt.time<<","<<pt.x<<","<<pt.y<<","<<pt.z<<","<<pt.azimuthValue<<","<<pt.distance<<","<<pt.intensity<<","<<pt.channel<<"\n";
    } else if(currentExportParams.exportType==LAS && lasWriter){
        for(auto &pd:pointDataList){
            liblas::Point p(&lasHeader); p.SetCoordinates(pd.x,pd.y,pd.z); p.SetIntensity(pd.intensity); lasWriter->WritePoint(p);
        }
    }
}

void UdpReceiver::parseDIFOPPacket(const QByteArray &data)
{
    if (data.size() != 1248) { qWarning() << "DIFOP size invalid:" << data.size(); return; }
    static const quint8 header[8] = {0xA5,0xFF,0x00,0x5A,0x11,0x11,0x55,0x55};
    for (int i = 0; i < 8; ++i) if (quint8(data[i]) != header[i]) { qWarning() << "DIFOP header invalid"; return; }
    if (quint8(data[1246]) != 0x0F || quint8(data[1247]) != 0xF0) { qWarning() << "DIFOP tail invalid"; return; }

    DifopInfo info{};
    info.speed = (quint16(quint8(data[8])) << 8) | quint8(data[9]);

    QString lidarIp = QString("%1.%2.%3.%4").arg(quint8(data[10])).arg(quint8(data[11])).arg(quint8(data[12])).arg(quint8(data[13]));
    QString destIp  = QString("%1.%2.%3.%4").arg(quint8(data[14])).arg(quint8(data[15])).arg(quint8(data[16])).arg(quint8(data[17]));
    QString macAddr;
    for (int i = 0; i < 6; ++i) { macAddr += QString("%1").arg(quint8(data[18+i]), 2, 16, QLatin1Char('0')); if (i != 5) macAddr += ":"; }
    const quint16 msopPort  = (quint16(quint8(data[24])) << 8) | quint8(data[25]);
    const quint16 difopPort = (quint16(quint8(data[28])) << 8) | quint8(data[29]);
    info.netInfo = InternetInfo{lidarIp, destIp, macAddr, msopPort, difopPort};

    info.fov_start = ((quint16(quint8(data[32])) << 8) | quint8(data[33])) / 100;
    info.fov_end   = ((quint16(quint8(data[34])) << 8) | quint8(data[35])) / 100;
    info.mot_phase = (quint16(quint8(data[38])) << 8) | quint8(data[39]);
    info.returnMode = quint8(data[300]);
    returnMode = info.returnMode;
    info.timeSyncInfo.type = quint8(data[301]);
    info.timeSyncInfo.status = quint8(data[302]);
    info.rainMode = 0; // Helios16 协议无独立雨雾模式字段，302 属于时间同步信息。
}
