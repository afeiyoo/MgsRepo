#pragma once

#include "t_basepage.h"

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QWidget>

class ElaLineEdit;
class ElaPushButton;
class ElaComboBox;
class ElaImageCard;
class ElaText;
class ElaCheckBox;
class ElaPlainTextEdit;
class CardRobotHandler;
class T_CardRobotStatus;
class T_CardRobot : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_CardRobot(QWidget *parent = nullptr);
    ~T_CardRobot() override;

    void initContent();

    void initRobotHandler();

public slots:
    void onListen();
    void onPowerOnMsgSend();                                // 上电信息发送
    void onOutCardMsgSend();                                // 出卡信息发送
    void onRequireCardMsgSend();                            // 按键取卡信息发送
    void onRetriveCardMsgSend();                            // 卡被取走信息发送
    void onRecycleCardMsgSend();                            // 卡回收完成信息发送
    void onIgnoreMsgSend();                                 // 按键忽略信息发送
    void onPaperMsgSend();                                  // 纸券取走及状态信息发送
    void onRobotSendCommandFinish(uchar cmdType, bool res); // 信息发送结果
    void onRobotReceiveCommand(uchar cmdType, QByteArray json);

private:
    bool playSound(uchar voiceNum, uchar playCount);
    QImage genQRCode(const QString &text, int size);
    void displaySecondScreen(const QString &text);
    void displayMainScreen(const QString &text);
    void displayQRCode(const QImage &img);

private:
    CardRobotHandler *m_robotHandler = nullptr;
    bool m_isListening = false; // 是否正在监听

    ElaLineEdit *m_portEdit = nullptr;
    ElaPushButton *m_listenButton = nullptr;
    ElaCheckBox *m_detailLogCheckBox = nullptr;
    ElaCheckBox *m_autoRespCheckBox = nullptr;
    ElaCheckBox *m_allowPlayCheckBox = nullptr;

    ElaPushButton *m_powerOnButton = nullptr;
    ElaPushButton *m_settingHeartButton = nullptr;
    ElaComboBox *m_workStationComboBox = nullptr;
    ElaComboBox *m_upBoxNoComboBox = nullptr;
    ElaComboBox *m_downBoxNoComboBox = nullptr;
    ElaImageCard *m_qrCodeImage = nullptr; // 二维码显示
    ElaText *m_mainScreenText = nullptr;   // 主屏显示内容
    ElaText *m_secondScreenText = nullptr; // 副屏显示内容

    // 心跳定时器
    QTimer *m_heartTimer = nullptr;

    // 出卡信息
    ElaComboBox *m_outCardWorkStationComboBox = nullptr;
    ElaComboBox *m_outCardBoxNoComboBox = nullptr;
    ElaPushButton *m_outCardButton = nullptr;

    // 按键取卡
    ElaComboBox *m_requireCardWorkStationComboBox = nullptr;
    ElaComboBox *m_requireCardBoxNoComboBox = nullptr;
    ElaPushButton *m_requireCardButton = nullptr;

    // 卡被取走
    ElaComboBox *m_retriveCardWorkStationComboBox = nullptr;
    ElaComboBox *m_retriveCardBoxNoComboBox = nullptr;
    ElaPushButton *m_retriveCardButton = nullptr;

    // 卡回收完成
    ElaComboBox *m_recycleCardWorkStationComboBox = nullptr;
    ElaComboBox *m_recycleCardBoxNoComboBox = nullptr;
    ElaPushButton *m_recycleCardButton = nullptr;

    // 按键忽略
    ElaComboBox *m_ignoreMessageWorkStationComboBox = nullptr;
    ElaComboBox *m_ignoreMessageCauseComboBox = nullptr;
    ElaPushButton *m_ignoreMessageButton = nullptr;

    // 纸券取走及状态信息
    ElaComboBox *m_paperWorkStationComboBox = nullptr;
    ElaComboBox *m_paperCauseComboBox = nullptr;
    ElaComboBox *m_paperStatusUpBoxNoComboBox = nullptr;
    ElaComboBox *m_paperStatusDownBoxNoComboBox = nullptr;
    ElaLineEdit *m_paperNumEdit = nullptr;
    ElaPushButton *m_paperButton = nullptr;

    // 交互日志显示区
    ElaPushButton *m_logClearButton = nullptr;
    ElaPlainTextEdit *m_logEdit = nullptr;

    // 卡状态设置窗口
    T_CardRobotStatus *m_robotStatusDlg = nullptr;

    QMediaPlayer *m_player = nullptr;
    QMediaPlaylist *m_playlist = nullptr;
};
