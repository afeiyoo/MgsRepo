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
    void onSendA1();
    void onSendA2();
    void onRecvD2(QByteArray cmd);
    void onRecvD6(QByteArray cmd);

private:
    enum class StatusTone { Error, Pending, Success };

    void initContent();
    void setConnectionFieldsEnabled(bool enabled);
    void setCommandControlsEnabled(bool enabled);
    void updateConnectionStatus(const QString &text, StatusTone tone);
    void resetConnectionUi();
    bool parseEndpoint(QString &ip, quint16 &port);
    void showInputError(const QString &message);
    void updateD2Status(quint16 status);
    QMap<int, int> relayMap() const;
    QMap<int, int> levelMap() const;

private:
    ElaLineEdit *m_connectInfoEdit = nullptr;
    ElaPushButton *m_connectButton = nullptr;
    ElaText *m_connectionStatusText = nullptr;

    QList<ElaToggleButton *> m_relayButtons;
    ElaRadioButton *m_lowLevelButton = nullptr;
    ElaRadioButton *m_highLevelButton = nullptr;
    ElaPushButton *m_allOffButton = nullptr;
    ElaPushButton *m_sendA1Button = nullptr;

    ElaLineEdit *m_uploadUrlEdit = nullptr;
    ElaSpinBox *m_uploadIntervalSpinBox = nullptr;
    ElaPushButton *m_sendA2Button = nullptr;

    QList<ElaToggleButton *> m_d2StatusButtons;
    ElaText *m_d2StatusText = nullptr;
    ElaText *m_deviceStatusText = nullptr;
    ElaText *m_d6IoStatusText = nullptr;
    ElaText *m_heartbeatTimeText = nullptr;

    ElaPlainTextEdit *m_logEdit = nullptr;
    ElaPushButton *m_logClearButton = nullptr;

    ISmartLaneController *m_smartController = nullptr;
    bool m_connecting = false;
    bool m_connected = false;
    bool m_reconnecting = false;
    bool m_userDisconnectRequested = false;
};
