#ifndef LASERRADAR_H
#define LASERRADAR_H

#include <QMainWindow>
#include <QToolButton>
#include "colorbarwidget.h"
#include <QPushButton>
#include "udpreceiver.h"
#include "pointdatapagemodel.h"
#include "pointcloudfiltermanager.h"  // 需要加这个！
#include "exportframedialog.h"
#include "browserdialog.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class LaserRadar;
}
QT_END_NAMESPACE
class LaserRadar : public QMainWindow
{
    Q_OBJECT

public:
    LaserRadar(QWidget *parent = nullptr);
    ~LaserRadar();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void signalExportToUDP(const ExportParams& params);

private slots:
    void on_checkBox_toggled(bool checked);

    void on_comboBox_currentIndexChanged(int index);
    void wgtCloseBtnClicked();
    void on_btn_basicSetting_clicked();

    void on_btn_start_clicked();

    void on_btn_colorSetting_clicked();

    void on_btn_radarBD_clicked();

    void on_btn_obs_clicked();

    void on_btn_dataShow_clicked();

    void on_btn_dylb_clicked();

    void on_btn_import_clicked();

    void on_btn_export_clicked();

    void updatePointData(QList<PointData> pointDataList);  // 你原来的

    // 🚀 新增：自动滤波槽函数
    void onPointDataUpdated(const QList<PointXYZI> &points);

    void on_pushButton_clicked();

    void on_spinPassXMax_valueChanged(double arg1);

    void on_spinPassXMin_valueChanged(double arg1);

    void on_spinPassYMax_valueChanged(double arg1);

    void on_spinPassYMin_valueChanged(double arg1);

    void on_spinPassZMax_valueChanged(double arg1);

    void on_spinPassZMin_valueChanged(double arg1);
    void openExprotFrameDlg(ExportType type);
    void onExportParamsReceived(const ExportParams& params);
    void onExportFinished();
    void onStopExport();

    // void on_checkBox_enable_toggled(bool checked);

    // void on_checkBox_disable_toggled(bool checked);

    void on_checkBox_enable_clicked();

    void on_checkBox_disable_clicked();

    void on_label_link_linkActivated(const QString &link);

    void on_btn_connectDev_clicked();

    void on_btn_connectDev_2_clicked();

    void on_label_2_linkActivated(const QString &link);
    void updateLBQpage();
    void updateObsParams();

private:
    void initUI();
    void initTittleBar();
    void resizeWindow(const QPoint &delta);
    void updateCursorShape(const QPoint &cursorPos);
    void checkResizeDirection(const QPoint &cursorPos);
    void enableMouseTrackingForAll(QWidget *parent);
    void initWidgets(QWidget *wgt, int w, int h);
    void setupButtons();
    void toggleButton(QPushButton *clickedBtn);
    void setButtonStyle(QPushButton *btn, bool isSelected);
    void showWgtAnimation(QWidget *wgt);

    // 🚀 新增：初始化滤波器界面
    void initFilterUI();
    void updateFilterSettings();  // 新增，更新 filterManager 的 filterList

    void updateExportSatatus(bool exporting);

    void loadObsSettings();
    void saveObsSettings();
    void resetObsSettingsToDefaults();
    void updateObsRanges();

private:
    Ui::LaserRadar *ui;
    QPoint lastMousePos;
    bool mousePressed = false;
    bool isDragging = false;
    QRect normalGeometry;
    QToolButton *minButton;
    QToolButton *maxButton;
    QToolButton *closeButton;
    bool isResizing = false;
    int resizingDirection = 0;
    Qt::Edges resizeDirection;

    ColorBarWidget *colorBar;
    bool initColorBar = false;

    QMap<QWidget*, bool> wgtShowMap;
    QMap<QPushButton*, bool> buttonStates;
    QList<QPushButton *> buttons;

    UdpReceiver *udp;
    PointDataPageModel *model;

    bool enableObstacleDetection = false;
    bool obstacleDetecting = false;
    // 🚀 新增：滤波器管理器
    PointCloudFilterManager *filterManager = nullptr;

    bool enableFilter = false;  // 是否启用滤波器

    // 🚀 缓存原始点云
    QList<PointXYZI> originalPoints;

    QAction* actionRecord;
    QMenu* exportSubMenu;
    QAction* actionStopExport;
};

#include <QDialog>
#include <QVBoxLayout>
#include <QSvgWidget>

class SvgDialog : public QDialog {
public:
    SvgDialog(const QString &svgFile, QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("激光雷达坐标及旋转方向示意图");

        QVBoxLayout *layout = new QVBoxLayout(this);
        QSvgWidget *svgWidget = new QSvgWidget(svgFile, this);
        svgWidget->setMinimumSize(400, 400);

        layout->addWidget(svgWidget);
        setLayout(layout);
    }
};


#include <QLabel>
#include <QPixmap>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileInfo>
class ImageDialog : public QDialog
{
public:
    ImageDialog(const QString &imageFile, QWidget *parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle(tr("激光雷达坐标及旋转方向示意图"));

        resize(900,700);

        m_imageLabel = new QLabel(this);
        m_imageLabel->setAlignment(Qt::AlignCenter);
        m_imageLabel->setStyleSheet(
            "background-color: rgb(255,255,255);");
        QPixmap pix(imageFile);

        if(!pix.isNull())
        {
            m_imageLabel->setPixmap(
                pix.scaled(
                    850,
                    600,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation));
        }
        else
        {
            m_imageLabel->setText(tr("示意图加载失败！"));
        }

        m_scrollArea = new QScrollArea(this);
        m_scrollArea->setWidget(m_imageLabel);
        m_scrollArea->setWidgetResizable(true);
        m_scrollArea->setAlignment(Qt::AlignCenter);

        m_textLabel = new QLabel(
            tr("激光雷达的坐标原点定义在激光雷达底座中心处"),
            this);

        m_textLabel->setAlignment(Qt::AlignCenter);
        m_textLabel->setWordWrap(true);

        QFont font = m_textLabel->font();
        font.setPointSize(11);
        font.setBold(true);
        m_textLabel->setFont(font);


        QVBoxLayout *layout = new QVBoxLayout(this);

        layout->addWidget(m_scrollArea,1);
        layout->addWidget(m_textLabel);

        setLayout(layout);
    }
private:
    QLabel *m_imageLabel;
    QLabel *m_textLabel;
    QScrollArea *m_scrollArea;
};

#endif // LASERRADAR_H
