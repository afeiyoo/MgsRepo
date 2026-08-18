#pragma once

#include <QImage>
#include <QList>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QStringList>

class MainWindow;
class LaneUI;
class IServiceHub;
class LaneMaster : public QObject
{
    Q_OBJECT
public:
    explicit LaneMaster(QObject *parent = nullptr);
    ~LaneMaster() override;

    // 车道初始化
    void initLane(QApplication &app);

private:
    // 混合入口界面接口获取
    LaneUI *createMtcInUI(bool isMaxShow, QObject *parent = nullptr);

    // 混合出口界面接口获取
    LaneUI *createMtcOutUI(bool isMaxShow, bool isSptShow, QObject *parent = nullptr);

    // ETC界面接口获取
    LaneUI *createEtcUI(bool isMaxShow, QObject *parent = nullptr);

private:
    MainWindow *m_mainWindow = nullptr;
    LaneUI *m_ui = nullptr;
    IServiceHub *m_hub = nullptr;
};
