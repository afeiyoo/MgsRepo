#pragma once

#include <QList>
#include <QMap>

#include "t_basepage.h"

class ElaLineEdit;
class ElaPlainTextEdit;
class ElaPushButton;
class ElaRadioButton;
class ElaSpinBox;
class ElaText;
class ElaToggleButton;
class ISmartLaneController;

class T_SmartController : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_SmartController(QWidget *parent = nullptr);
    ~T_SmartController() override;

private slots:
    void onConnectServer();
    void onSendA2();
    void onRecvD2Cmd(QByteArray cmd);
    // 模式切换
    void onModeToggled(bool checked);
    // 偏移位触发
    void onOffsetToggled(bool checked);
    // 控制位触发
    void onControlToggled(int index, bool checked);
    // 电平位触发
    void onLevelToggled(int level, bool checked);
    // 连接状态改变
    void onConnectionStateChanged(bool connected);
    // 心跳状态改变
    void onHeartbeatStateChanged(bool normal);
    // 重连失败
    void onReconnectFailed();

private:
    enum class StatusTone { Error, Pending, Success };
    enum class ConnectionState { Disconnected, Connecting, Connected, Reconnecting };

    // 页面初始化
    void initContent();
    // 网关对象初始化
    void initSmartController();
    void setConnectionState(ConnectionState state);
    void setCommandControlsEnabled(bool enabled);
    // 设置IO按钮不可点击
    void setToggleReadOnly(ElaToggleButton *button, bool readOnly);
    // 界面IO按钮重置
    void resetIoControls();
    void updateConnectionStatus(const QString &text, StatusTone tone);
    void updateHeartbeatStatus(const QString &text, StatusTone tone);
    // 解析服务地址
    bool parseEndpoint(QString &ip, quint16 &port);
    // 输入错误提示
    void showInputError(const QString &message);
    void refreshControlButtons();
    bool isConnected() const;
    bool isOutputMode() const;
    void sendA1Command();
    QMap<int, int> relayMap() const;
    QMap<int, int> levelMap() const;

private:
    ElaLineEdit *m_connectInfoEdit = nullptr;
    ElaPushButton *m_connectButton = nullptr;
    ElaText *m_connectionStatusText = nullptr;
    ElaText *m_heartBeatStatusText = nullptr;

    ElaRadioButton *m_inputModeButton = nullptr;
    ElaRadioButton *m_outputModeButton = nullptr;
    ElaToggleButton *m_offsetButton = nullptr; // 偏移位按钮
    QList<ElaToggleButton *> m_controlButtons; // 控制位按钮
    QList<ElaToggleButton *> m_levelButtons;   // 电平位按钮

    // URL上传配置
    ElaLineEdit *m_uploadUrlEdit = nullptr;
    ElaSpinBox *m_uploadIntervalSpinBox = nullptr;
    ElaPushButton *m_sendA2Button = nullptr;

    ElaPlainTextEdit *m_logEdit = nullptr;
    ElaPushButton *m_logClearButton = nullptr;

    ISmartLaneController *m_smartController = nullptr;
    ConnectionState m_connectionState = ConnectionState::Disconnected;
    bool m_userDisconnectRequested = false; // 区分用户主动断开与连接异常
    bool m_hasInputStatus = false;
    quint16 m_inputStatus = 0;  // 输入状态
    quint16 m_outputStatus = 0; // 输出状态
    int m_outputOffset = 0;
    int m_outputLevel = -1;
};
