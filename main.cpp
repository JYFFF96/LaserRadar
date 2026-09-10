#include "laserradar.h"
#include "englishtranslator.h"

#include <QApplication>
#include <QFile>
#include <QStyleFactory>

void loadStyleSheet(QApplication &app) {
    QFile file(":./qss/style.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = file.readAll();
        app.setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "Failed to load QSS file!";
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::ButtonText, Qt::white);
    qApp->setPalette(palette);

    loadStyleSheet(a);

    // English branch: translate all Qt Designer / tr() strings to English.
    EnglishTranslator englishTranslator;
    a.installTranslator(&englishTranslator);

    LaserRadar w;
    // Override the legacy hard-coded Chinese title in LaserRadar's constructor.
    w.setWindowTitle("LiDAR Debugging Software");
    w.show();
    return a.exec();
}
