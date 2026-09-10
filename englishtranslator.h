#ifndef ENGLISHTRANSLATOR_H
#define ENGLISHTRANSLATOR_H

#include <QTranslator>
#include <QHash>
#include <QString>

class EnglishTranslator final : public QTranslator
{
public:
    explicit EnglishTranslator(QObject *parent = nullptr) : QTranslator(parent) {}

    QString translate(const char *context, const char *sourceText,
                      const char *disambiguation = nullptr, int n = -1) const override
    {
        Q_UNUSED(context)
        Q_UNUSED(disambiguation)
        Q_UNUSED(n)
        static const QHash<QString, QString> map = {
            {QStringLiteral("基础设置"), QStringLiteral("Basic Settings")},
            {QStringLiteral("MSOP端口号（1025-65535）:"), QStringLiteral("MSOP Port (1025-65535):")},
            {QStringLiteral("DIFOP端口号（1025-65535）:"), QStringLiteral("DIFOP Port (1025-65535):")},
            {QStringLiteral("FOV设置（0-360）度:"), QStringLiteral("FOV (0-360 deg):")},
            {QStringLiteral("至"), QStringLiteral("to")},
            {QStringLiteral("锁相设置（0-360）度:"), QStringLiteral("Phase Lock (0-360 deg):")},
            {QStringLiteral("返回模式:"), QStringLiteral("Return Mode:")},
            {QStringLiteral("转速:"), QStringLiteral("Motor Speed:")},
            {QStringLiteral("时间同步源:"), QStringLiteral("Time Sync Source:")},
            {QStringLiteral("FTP域号（0-127）:"), QStringLiteral("FTP Domain (0-127):")},
            {QStringLiteral("操作模式:"), QStringLiteral("Operation Mode:")},
            {QStringLiteral("反射率增强:"), QStringLiteral("Reflectivity Enhancement:")},
            {QStringLiteral("开启"), QStringLiteral("Enable")},
            {QStringLiteral("关闭"), QStringLiteral("Disable")},
            {QStringLiteral("雨雾模式:"), QStringLiteral("Rain/Fog Mode:")},
            {QStringLiteral("电机反转:"), QStringLiteral("Motor Reverse:")},
            {QStringLiteral("上"), QStringLiteral("Up")},
            {QStringLiteral("设备IP地址:"), QStringLiteral("Device IP Address:")},
            {QStringLiteral("设备IP掩码:"), QStringLiteral("Device IP Mask:")},
            {QStringLiteral("设备IP网关:"), QStringLiteral("Device IP Gateway:")},
            {QStringLiteral("目标IP地址:"), QStringLiteral("Destination IP Address:")},
            {QStringLiteral("确认"), QStringLiteral("Confirm")},
            {QStringLiteral("确定"), QStringLiteral("OK")},
            {QStringLiteral("高级"), QStringLiteral("Advanced")},
            {QStringLiteral("<a href=\"#\">高级</a>"), QStringLiteral("<a href=\"#\">Advanced</a>")},
            {QStringLiteral("颜色设置"), QStringLiteral("Color Settings")},
            {QStringLiteral("显示颜色条"), QStringLiteral("Show Color Bar")},
            {QStringLiteral("雷达标定"), QStringLiteral("LiDAR Calibration")},
            {QStringLiteral("X轴(cm)："), QStringLiteral("X Axis (cm):")},
            {QStringLiteral("Y轴(cm)："), QStringLiteral("Y Axis (cm):")},
            {QStringLiteral("Z轴(cm)："), QStringLiteral("Z Axis (cm):")},
            {QStringLiteral("偏航角(°)："), QStringLiteral("Yaw (deg):")},
            {QStringLiteral("横滚角(°)："), QStringLiteral("Roll (deg):")},
            {QStringLiteral("俯仰角(°)："), QStringLiteral("Pitch (deg):")},
            {QStringLiteral("<a href=\"#\">激光雷达坐标及旋转方向示意图</a>"), QStringLiteral("<a href=\"#\">LiDAR Coordinate and Rotation Diagram</a>")},
            {QStringLiteral("障碍物识别"), QStringLiteral("Obstacle Detection")},
            {QStringLiteral("欧氏距离"), QStringLiteral("Euclidean Distance")},
            {QStringLiteral("滤波设置"), QStringLiteral("Filter Settings")},
            {QStringLiteral("Z轴上下方向最搞感知距离"), QStringLiteral("Maximum Z-Axis Vertical Detection Distance")},
            {QStringLiteral("最少点云个数"), QStringLiteral("Minimum Point Count")},
            {QStringLiteral("最大点云个数"), QStringLiteral("Maximum Point Count")},
            {QStringLiteral("X轴前后方向最远感知距离"), QStringLiteral("Maximum X-Axis Longitudinal Detection Distance")},
            {QStringLiteral("Y轴左右方向最远感知距离"), QStringLiteral("Maximum Y-Axis Lateral Detection Distance")},
            {QStringLiteral("X轴前后方向障碍物停止距离"), QStringLiteral("X-Axis Longitudinal Obstacle Stop Distance")},
            {QStringLiteral("Y轴左右方向障碍物停止距离"), QStringLiteral("Y-Axis Lateral Obstacle Stop Distance")},
            {QStringLiteral("Z轴上下方向障碍物停止距离"), QStringLiteral("Z-Axis Vertical Obstacle Stop Distance")},
            {QStringLiteral("保存当前设置"), QStringLiteral("Save Current Settings")},
            {QStringLiteral("恢复默认设置"), QStringLiteral("Restore Defaults")},
            {QStringLiteral("首页"), QStringLiteral("First")},
            {QStringLiteral("上一页"), QStringLiteral("Previous")},
            {QStringLiteral("下一页"), QStringLiteral("Next")},
            {QStringLiteral("尾页"), QStringLiteral("Last")},
            {QStringLiteral("跳转"), QStringLiteral("Go")},
            {QStringLiteral("点云滤波"), QStringLiteral("Point Cloud Filtering")},
            {QStringLiteral("滤波器配置"), QStringLiteral("Filter Configuration")},
            {QStringLiteral("直通滤波器"), QStringLiteral("Pass-Through Filter")},
            {QStringLiteral("高斯滤波器"), QStringLiteral("Gaussian Filter")},
            {QStringLiteral("体素滤波器"), QStringLiteral("Voxel Grid Filter")},
            {QStringLiteral("统计滤波器"), QStringLiteral("Statistical Filter")},
            {QStringLiteral("半径滤波器"), QStringLiteral("Radius Filter")},
            {QStringLiteral("频率滤波器"), QStringLiteral("Frequency Filter")},
            {QStringLiteral("双边滤波器"), QStringLiteral("Bilateral Filter")},
            {QStringLiteral("X轴"), QStringLiteral("X Axis")},
            {QStringLiteral("Y轴"), QStringLiteral("Y Axis")},
            {QStringLiteral("Z轴"), QStringLiteral("Z Axis")},
            {QStringLiteral("最大值："), QStringLiteral("Maximum:")},
            {QStringLiteral("最小值："), QStringLiteral("Minimum:")},
            {QStringLiteral("米"), QStringLiteral(" m")},
            {QStringLiteral("体素大小："), QStringLiteral("Voxel Size:")},
            {QStringLiteral("邻居数："), QStringLiteral("Neighbors:")},
            {QStringLiteral("半径："), QStringLiteral("Radius:")},
            {QStringLiteral("最小邻居数："), QStringLiteral("Minimum Neighbors:")},
            {QStringLiteral("帧窗口大小："), QStringLiteral("Frame Window Size:")},
            {QStringLiteral("帧"), QStringLiteral(" frames")},
            {QStringLiteral("频率阈值："), QStringLiteral("Frequency Threshold:")},
            {QStringLiteral("应用滤波器"), QStringLiteral("Apply Filters")},
            {QStringLiteral("清除滤波器"), QStringLiteral("Clear Filters")},
            {QStringLiteral("区域设置"), QStringLiteral("Region Settings")},
            {QStringLiteral("X轴(m)："), QStringLiteral("X Axis (m):")},
            {QStringLiteral("Y轴(m)："), QStringLiteral("Y Axis (m):")},
            {QStringLiteral("Z轴(m)："), QStringLiteral("Z Axis (m):")},
            {QStringLiteral("通道设置"), QStringLiteral("Channel Settings")},
            {QStringLiteral("限定范围"), QStringLiteral("Range Limit")},
            {QStringLiteral("开启滤波"), QStringLiteral("Enable Filter")},
            {QStringLiteral("中心点坐标"), QStringLiteral("Center Coordinates")},
            {QStringLiteral("X轴："), QStringLiteral("X Axis:")},
            {QStringLiteral("Y轴："), QStringLiteral("Y Axis:")},
            {QStringLiteral("Z轴："), QStringLiteral("Z Axis:")},
            {QStringLiteral("邻域范围（立方体边长）"), QStringLiteral("Neighborhood Range (Cube Side Length)")},
            {QStringLiteral("计算权重"), QStringLiteral("Calculate Weights")},
            {QStringLiteral("加权平均"), QStringLiteral("Weighted Average")},
            {QStringLiteral("遍历点云"), QStringLiteral("Iterate Point Cloud")},
            {QStringLiteral("3D体素网格边长"), QStringLiteral("3D Voxel Grid Size")},
            {QStringLiteral("组合设置1"), QStringLiteral("Combination Setting 1")},
            {QStringLiteral("组合设置2"), QStringLiteral("Combination Setting 2")},
            {QStringLiteral("选项1"), QStringLiteral("Option 1")},
            {QStringLiteral("生成滤波点云"), QStringLiteral("Generate Filtered Point Cloud")},
            {QStringLiteral("临点数"), QStringLiteral("Neighbor Count")},
            {QStringLiteral("PCAP 回放"), QStringLiteral("PCAP Playback")},
            {QStringLiteral("循环"), QStringLiteral("Loop")},
            {QStringLiteral("选择帧"), QStringLiteral("Select Frames")},
            {QStringLiteral("当前帧"), QStringLiteral("Current Frame")},
            {QStringLiteral("所有帧"), QStringLiteral("All Frames")},
            {QStringLiteral("起始帧"), QStringLiteral("Start Frame")},
            {QStringLiteral("存档模式"), QStringLiteral("Save Mode")},
            {QStringLiteral("保存到一个文件中"), QStringLiteral("Save to One File")},
            {QStringLiteral("一帧一个文件"), QStringLiteral("One File per Frame")},
            {QStringLiteral("文件"), QStringLiteral("File")},
            {QStringLiteral("打开"), QStringLiteral("Open")},
            {QStringLiteral("导出"), QStringLiteral("Export")},
            {QStringLiteral("退出"), QStringLiteral("Exit")}
        };

        const auto it = map.constFind(QString::fromUtf8(sourceText));
        return it == map.constEnd() ? QString() : it.value();
    }
};

#endif // ENGLISHTRANSLATOR_H
