#include "browserdialog.h"

#include <QVBoxLayout>
#include <QWebEngineView>

BrowserDialog::BrowserDialog(const QUrl &url, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("激光雷达设置窗口");

    QVBoxLayout *layout = new QVBoxLayout(this);
    webView = new QWebEngineView(this);
    webView->load(url);
    webView->setMinimumSize(800, 600);

    layout->addWidget(webView);
    setLayout(layout);
}
