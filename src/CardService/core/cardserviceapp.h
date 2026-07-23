#pragma once

#include "qtservice.h"

// 支持如下命令:
// CardService.exe -install  服务安装
// CardService.exe -uninstall   服务卸载
// CardService.exe -terminate    服务停止
// CardService.exe -exec     前台调试应用
// CardService.exe -help

class QSettings;
class RequestMapper;

namespace stefanfrings {
class HttpListener;
}

class CardServiceApp : public QtService<QCoreApplication>
{
public:
    explicit CardServiceApp(int argc, char **argv);
    ~CardServiceApp() override;

protected:
    void start() override;
    void stop() override;

private:
    void cleanup();

private:
    bool m_initialized = false;
    QSettings *m_listenerSettings = nullptr;
    RequestMapper *m_requestMapper = nullptr;
    stefanfrings::HttpListener *m_listener = nullptr;
};
