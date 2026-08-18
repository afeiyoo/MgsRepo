#pragma once

#include <QMap>

#include "t_basepage.h"

class ElaLineEdit;
class ElaPushButton;
class ElaRadioButton;
class ElaToggleButton;
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

private:
    void setIoButtonsReadOnly(bool readOnly);
    void resetIoButtons();
    void updateIoButtons(quint16 status);
    void syncOutputState();
    QByteArray packSendData(const QMap<int, bool> &relayMap, int triggerLevel);

private:
    // UI
    ElaLineEdit *m_connectInfoEdit = nullptr;
    ElaPushButton *m_connectButton = nullptr;

    ElaRadioButton *m_outputButton = nullptr;
    ElaRadioButton *m_inputButton = nullptr;

    ElaToggleButton *m_offsetButton = nullptr;
    QList<ElaToggleButton *> m_controlButton;
    QList<ElaToggleButton *> m_triggerButton;

    ElaText *m_heartStatusText = nullptr;
    ElaPlainTextEdit *m_logEdit = nullptr;
    ElaPushButton *m_logClearButton = nullptr;

    // 后台
    SmartLaneController *m_smartController = nullptr;
    bool m_isTcpConnected = false;  // tcp连接是否正常
    QMap<int, bool> m_lastRelayMap; // 上一次发送的继电器状态
    int m_lastTriggerLevel = 0;     // 电平位取消时，沿用上一次电平关闭输出

    quint16 m_lastCtrlStatus = 0;   // 最近一次D2上报的IO状态
    bool m_hasLastCtrlStatus = false;
};
