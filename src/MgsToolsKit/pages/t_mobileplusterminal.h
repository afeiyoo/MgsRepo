#pragma once

#include "t_basepage.h"

class ElaComboBox;
class ElaLineEdit;
class ElaPlainTextEdit;
class ElaPushButton;
class ElaSpinBox;
class ElaText;
class IMobilePlusTerminal;

class T_MobilePlusTerminal : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_MobilePlusTerminal(QWidget *parent = nullptr);
    ~T_MobilePlusTerminal() override;

private slots:
    void onConnectServer();
    void onShowQrCode();
    void onShowLed();
    void onSelectPicture();
    void onShowPicture();
    void onSetUploadUrl();

private:
    enum class StatusTone { Error, Pending, Success };

    void initContent();
    void createTerminal();
    void setConnectionFieldsEnabled(bool enabled);
    void setCommandButtonsEnabled(bool enabled);
    void updateConnectionStatus(const QString &text, StatusTone tone);
    void resetConnectionUi();
    // 解析输入的IP与端口
    bool parseEndpoint(QString &ip, quint16 &port);
    void showInputError(const QString &message);

private:
    ElaLineEdit *m_connectInfoEdit = nullptr;
    ElaLineEdit *m_stationIdEdit = nullptr;
    ElaSpinBox *m_laneIdSpinBox = nullptr;
    ElaSpinBox *m_deviceSeqSpinBox = nullptr;
    ElaComboBox *m_versionComboBox = nullptr;
    ElaPushButton *m_connectButton = nullptr;
    ElaText *m_connectionStatusText = nullptr;

    ElaLineEdit *m_stationNameEdit = nullptr;
    ElaLineEdit *m_vehicleClassEdit = nullptr;
    ElaLineEdit *m_vehiclePlateEdit = nullptr;
    ElaLineEdit *m_barCodeEdit = nullptr;
    ElaPushButton *m_qrCodeButton = nullptr;

    ElaLineEdit *m_ledTextEdit = nullptr;
    ElaPushButton *m_ledButton = nullptr;
    ElaLineEdit *m_picturePathEdit = nullptr;
    ElaPushButton *m_pictureSelectButton = nullptr;
    ElaPushButton *m_pictureButton = nullptr;
    ElaLineEdit *m_uploadUrlEdit = nullptr;
    ElaSpinBox *m_uploadIntervalSpinBox = nullptr;
    ElaPushButton *m_uploadUrlButton = nullptr;

    ElaPlainTextEdit *m_logEdit = nullptr;
    ElaPushButton *m_logClearButton = nullptr;

    IMobilePlusTerminal *m_terminal = nullptr;
    bool m_connecting = false;
    bool m_connected = false;
    bool m_reconnecting = false;
    bool m_userDisconnectRequested = false;
};
