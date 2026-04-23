#pragma once

#include <QHash>
#include <QLibrary>
#include <QObject>

#include "qmutex.h"

#ifdef WIN32
    #ifndef WINAPI
        #define WINAPI __stdcall
    #endif
#else
    #define WINAPI
#endif

typedef int(WINAPI *fn_dtp_connect)(int *handle, const char *ip, const int port);
typedef int(WINAPI *fn_dtp_disconn)(int handle);
typedef int(WINAPI *fn_dtp_login)(int handle, const char *user, const char *passwd, int *role);
typedef int(WINAPI *fn_dtp_logout)(int handle);
typedef int(WINAPI *fn_dtp_putmsg)(int handle, const char *queue, char *msg_content, const int msg_len, const int nPriority);
typedef int(WINAPI *fn_dtp_getmsg)(int handle, const char *queue, char *msg_content, int *msg_len);
typedef int(WINAPI *fn_dtp_storemsg)(int handle, const char *qname, const char *msgid, char *msg, const int nMsgLen);

extern fn_dtp_connect dtp_connect;
extern fn_dtp_disconn dtp_disconn;
extern fn_dtp_login dtp_login;
extern fn_dtp_logout dtp_logout;
extern fn_dtp_putmsg dtp_putmsg;
extern fn_dtp_getmsg dtp_getmsg;
extern fn_dtp_storemsg dtp_storemsg;

extern QLibrary *dtp_lib;

class DtpSender : public QObject
{
    Q_OBJECT
public:
    explicit DtpSender(QObject *parent = nullptr);

    ~DtpSender();

    // 发送数据到指定站点 DTP
    int sendMsgToDtp(QString serverIP, int port, QString queue, QString laneId, const QString &msg);

    // 连接到 DTP 服务端
    int connectDtpServer(QString serverIP, int port);

    // 加载 DTP-Client 动态库
    bool initDtp(QString s);

private:
    QHash<QString, int> m_dtpHandles;
    QHash<QString, int> m_loginDtps;
    QMutex m_mutex;
};
