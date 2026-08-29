#pragma once

#include "ivehrecognizer.h"
#include "t_basepage.h"

class ElaLineEdit;
class ElaPlainTextEdit;
class ElaPushButton;
class ElaSpinBox;
class ElaText;
class QLabel;

class T_VehRecognizer : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_VehRecognizer(QWidget *parent = nullptr);
    ~T_VehRecognizer() override;

private slots:
    void onConnectServer();
    void onVehicleTypeInfoReady(const ST_VehicleTypeInfo &vehicleInfo);
    void onVehicleImageInfoReady(const ST_VehicleImageInfo &imageInfo);
    void onSetUploadUrl();
    void onShowLed();
    void onPlayVoice();

private:
    enum class StatusTone { Error, Pending, Success };

    void initContent();
    void createRecognizer();
    bool parseEndpoint(QString &ip, quint16 &port);
    void resetConnectionUi();
    void updateConnectionStatus(const QString &text, StatusTone tone);
    void showImage(QLabel *label, const QString &path);
    void resetVehicleWidgets();
    void setCommandButtonsEnabled(bool enabled);

private:
    ElaLineEdit *m_connectInfoEdit = nullptr;
    ElaPushButton *m_connectButton = nullptr;
    ElaPushButton *m_clearButton = nullptr;
    ElaPushButton *m_logClearButton = nullptr;
    ElaText *m_connectionStatusText = nullptr;

    QLabel *m_headImage = nullptr;
    ElaText *m_headImageInfo = nullptr;
    QLabel *m_tailImage = nullptr;
    ElaText *m_tailImageInfo = nullptr;
    QLabel *m_bodyImage = nullptr;
    ElaText *m_bodyImageInfo = nullptr;

    ElaText *m_plateText = nullptr;
    ElaText *m_plateColorText = nullptr;
    ElaText *m_timeText = nullptr;
    ElaText *m_vehicleClassText = nullptr;
    ElaText *m_axleTypeText = nullptr;
    ElaText *m_axleCountText = nullptr;
    ElaText *m_dimensionText = nullptr;
    ElaText *m_extFlagText = nullptr;
    ElaText *m_directionText = nullptr;

    ElaLineEdit *m_uploadUrlEdit = nullptr;
    ElaSpinBox *m_uploadMinutesSpinBox = nullptr;
    ElaPushButton *m_uploadButton = nullptr;
    ElaLineEdit *m_ledTextEdit = nullptr;
    ElaSpinBox *m_ledColorSpinBox = nullptr;
    ElaPushButton *m_ledButton = nullptr;
    ElaLineEdit *m_voiceTextEdit = nullptr;
    ElaSpinBox *m_voiceCountSpinBox = nullptr;
    ElaSpinBox *m_voiceIntervalSpinBox = nullptr;
    ElaPushButton *m_voiceButton = nullptr;

    ElaPlainTextEdit *m_logEdit = nullptr;
    IVehRecognizer *m_recognizer = nullptr;
    bool m_connecting = false;
    bool m_connected = false;
    bool m_reconnecting = false;
    bool m_userDisconnectRequested = false;
};
