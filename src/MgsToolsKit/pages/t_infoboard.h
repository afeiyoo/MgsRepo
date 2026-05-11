#pragma once

#include "t_basepage.h"
#include <QMap>

class ElaLineEdit;
class ElaPushButton;
class ElaComboBox;
class ElaRadioButton;
class ElaCheckBox;
class ElaPlainTextEdit;
class InfoBoardHandler;
class T_InfoBoard : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_InfoBoard(QWidget *parent = nullptr);
    ~T_InfoBoard() override;

    void initContent();

public slots:
    void onConnectServer(); // 连接情报板
    void onSendMsg();       // 向情报板发送数据
    void onSetupUrl();      // 设置状态上报Url

private:
    void updateDevStatus(const QString &status, bool isWarning = false);
    void updateNetworkStatus(const QString &status, bool isOnline);

private:
    // UI
    ElaLineEdit *m_connectInfoEdit = nullptr;
    ElaPushButton *m_connectButton = nullptr;
    ElaRadioButton *m_httpButton = nullptr;
    ElaRadioButton *m_tcpButton = nullptr;

    ElaLineEdit *m_monitorUrlEdit = nullptr;
    ElaPushButton *m_confirmButton = nullptr;
    ElaText *m_networkStatusText = nullptr;
    ElaText *m_devStatusText = nullptr;

    ElaComboBox *m_filesComboBox = nullptr; // 播放内容选择框
    ElaPushButton *m_sendButton = nullptr;
    ElaRadioButton *m_hxButton = nullptr;
    ElaRadioButton *m_xlButton = nullptr;

    ElaPlainTextEdit *m_logEdit = nullptr;
    ElaPushButton *m_logClearButton = nullptr;

    // 后台
    InfoBoardHandler *m_infoBoardHandler = nullptr;

    bool m_isTcpConnected = false; // tcp是否已连接
    QMap<QString, QString> m_boardContentMap;
};
