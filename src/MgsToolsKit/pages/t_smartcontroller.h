#pragma once

#include "t_basepage.h"

class ElaLineEdit;
class ElaPushButton;
class ElaRadioButton;
class ElaToggleButton;
class ElaToggleSwitch;
class ElaPlainTextEdit;
class ElaText;
class SmartLaneController;
class T_SmartController : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_SmartController(QWidget *parent = nullptr);
    ~T_SmartController() override;

    void initContent();

public slots:
    void onConnectServer();
    void onRecvFromSmartLaneController(uchar type, QByteArray data);
    void onSendToSmartLaneController();

private:
    QByteArray packSendData(const QMap<int, bool> &relayMap, int triggerLevel);

private:
    // UI
    ElaLineEdit *m_connectInfoEdit = nullptr;
    ElaPushButton *m_connectButton = nullptr;

    ElaToggleSwitch *m_devSwitch = nullptr;
    ElaPushButton *m_sendButton = nullptr;
    ElaRadioButton *m_outputButton = nullptr;
    ElaRadioButton *m_inputButton = nullptr;

    ElaToggleButton *m_offsetButton = nullptr;
    QList<ElaToggleButton *> m_controlButton;
    QList<ElaToggleButton *> m_triggerButton;

    ElaText *m_heartStatusText = nullptr;
    ElaPlainTextEdit *m_logEdit = nullptr;

    // 后台
    SmartLaneController *m_smartController = nullptr;
    bool m_isTcpConnected = false;  // tcp连接是否正常
    QTimer *m_heartTimer = nullptr; // 心跳定时器
};
