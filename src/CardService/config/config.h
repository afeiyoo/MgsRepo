#pragma once

#include <QObject>
#include <QReadWriteLock>

#include "def/defines.h"

namespace Utils {
class ConfigUtils;
}

class Config
{
public:
    explicit Config();
    ~Config();

    void loadConfig(const QString &path);

    ST_ConfigSnap getSnap() const;

    void updateConfig(int apiType, const QString &apiName, const QString &comPort, int slotNo);

private:
    mutable QReadWriteLock m_lock;

    QString m_comPort;
    int m_slotNo;
    // 读卡器动态库类型 0:福建省标准 1:交通部标准
    int m_apiType;
    // 读卡器动态库文件名 ApiType为0时,默认是API_JKM115; ApiType为1时,默认为ICC_HTXX
    QString m_apiName;
    int m_laneType = 1;

    Utils::ConfigUtils *m_confUtil = nullptr; // 配置读取工具
};
