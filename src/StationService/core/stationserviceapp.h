#pragma once

// 支持如下命令:
// StationService.exe -install  服务安装
// StationService.exe -uninstall   服务卸载
// StationService.exe -terminate    服务停止
// StationService.exe -exec     前台调试应用
// StationService.exe -help

#include "qtservice.h"

class Cron;
class QSettings;
class RequestMapper;

namespace stefanfrings {
class HttpListener;
}

class StationServiceApp : public QtService<QCoreApplication>
{
public:
    StationServiceApp(int argc, char **argv);
    ~StationServiceApp() override;

protected:
    void start() override;
    void stop() override;

private:
    void cleanup();

private:
    Cron *m_checker = nullptr;
    QSettings *m_listenerSettings = nullptr;
    RequestMapper *m_requestMapper = nullptr;
    stefanfrings::HttpListener *m_listener = nullptr;
};
