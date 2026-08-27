#pragma once

#include <QString>
#include <QStringList>

// 日志类别
static const char L_CATE[] = "DeployTool";

// 部署信息
struct ST_DeployInfo
{
    QString stationID;   // 收费站代码
    QString stationName; // 收费站名
    QString stationIP;   // 收费站服务器IP
    QString roadNetNo;   // 路网编号

    int laneID = 0;   // 车道号
    int laneType = 0; // 车道类型
    QString laneIP;   // 车道IP

    // 数据库配置信息
    QString dbIP = "127.0.0.1";
    QString dbName = "tolllanedb";
    QString dbUser = "tlman";
    QString dbPassword = "ds18fjeit";
    int dbPort = 3306;
    int dbType = 1;

    // Redis配置信息
    QString redisIP; // Redis IP
    QString redisName = "0";
    QString redisUser = "test";
    QString redisPassword = "Mgskj@202101";
    int redisPort = 6379;

    // 天线配置信息
    QString rsuIP;
    int rsuPower = 0;

    // 桌面读卡器配置
    QString readerApiName = "";
    int readerApiType = 1;
    QString readerPort;
    int readerSlot = 2;

    // 抓拍配置
    QString capDevice = "MGSKJ";
    QString capIP; // 抓拍IP
    int capPort = 9588;
    int capTriggerMode = 0;
    int capType = 8; // 默认TCP

    // 发卡机配置
    int robotBaudRate = 0;

    // 车道网络信息
    QString netMask;
    QString gateWay;
    QStringList dns = {"35.16.1.49", "35.17.1.57"};
};
