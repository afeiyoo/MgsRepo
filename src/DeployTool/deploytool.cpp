#include "deploytool.h"

#include "EasyQtSql.h"
#include "Logger.h"
#include "define.h"
#include "utils/configutils.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"
#include "utils/networkutils.h"
#include "xlsxdocument.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QSqlDatabase>
#include <QStandardPaths>
#include <QTimer>

using namespace Utils;
using namespace QXlsx;
using namespace EasyQtSql;

DeployTool::DeployTool(QObject *parent)
    : IDeployTool(parent)
{
    m_conf = new ConfigUtils(this);

    m_networkProcess = new QProcess(this);
    m_networkProcess->setProcessChannelMode(QProcess::SeparateChannels);

    m_networkTimeoutTimer = new QTimer(this);
    m_networkTimeoutTimer->setSingleShot(true);

    connect(m_networkProcess, &QProcess::started, this, [this]() { emit networkUpdateStarted(); });

    connect(m_networkProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
                if (!m_networkUpdating)
                    return;

                m_networkTimeoutTimer->stop();
                const QString standardOutput = QString::fromLocal8Bit(m_networkProcess->readAllStandardOutput()).trimmed();
                const QString standardError = QString::fromLocal8Bit(m_networkProcess->readAllStandardError()).trimmed();

                if (m_networkTimedOut) {
                    const QString message = standardError.isEmpty() ? "网络配置脚本执行超时"
                                                                    : QStringLiteral("网络配置脚本执行超时: %1").arg(standardError);
                    finishNetworkUpdate(false, message);
                    return;
                }

                if (exitStatus != QProcess::NormalExit || exitCode != 0) {
                    QString detail = standardError;
                    if (detail.isEmpty())
                        detail = standardOutput;

                    const QString message = detail.isEmpty() ? QStringLiteral("网络配置脚本执行失败，退出码: %1").arg(exitCode)
                                                             : QStringLiteral("网络配置脚本执行失败，退出码: %1，错误: %2").arg(exitCode).arg(detail);
                    finishNetworkUpdate(false, message);
                    return;
                }

                finishNetworkUpdate(true, standardOutput.isEmpty() ? QStringLiteral("网络配置成功") : standardOutput);
            });

    connect(m_networkProcess, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        if (!m_networkUpdating || error != QProcess::FailedToStart)
            return;

        m_networkTimeoutTimer->stop();
        finishNetworkUpdate(false, QStringLiteral("无法启动网络配置脚本: %1").arg(m_networkProcess->errorString()));
    });

    connect(m_networkTimeoutTimer, &QTimer::timeout, this, [this]() {
        if (!m_networkUpdating)
            return;

        m_networkTimedOut = true;
        m_networkProcess->terminate();

        QTimer::singleShot(3000, this, [this]() {
            if (m_networkUpdating && m_networkProcess->state() != QProcess::NotRunning)
                m_networkProcess->kill();
        });
    });
}

DeployTool::~DeployTool()
{
    if (m_dbFactory)
        m_dbFactory->removeDataBase(DB_CONN_NAME);
}

bool DeployTool::loadDeployInfo(const QString &path, QString &errDesc)
{
    m_deployInfos.clear();

    Document xlsx(path);
    if (!xlsx.load()) {
        errDesc = "Excel文件加载失败";
        LOG_CERROR(L_CATE).noquote() << errDesc;
        return false;
    }

    if (!xlsx.selectSheet("Sheet1")) {
        errDesc = "找不到工作表 Sheet1";
        LOG_CERROR(L_CATE).noquote() << errDesc;
        return false;
    }

    for (int row = START_ROW;; ++row) {
        ST_DeployInfo deployInfo;
        deployInfo.baseInfo.stationID = xlsx.read(row, 1).toString().trimmed();
        if (deployInfo.baseInfo.stationID.isEmpty()) {
            // 站代码为空，则认为数据结束
            LOG_CINFO(L_CATE).noquote() << "第" << row << "行站代码为空，停止加载信息";
            break;
        }
        deployInfo.baseInfo.stationName = xlsx.read(row, 2).toString().trimmed();
        deployInfo.baseInfo.stationIP = xlsx.read(row, 3).toString().trimmed();
        deployInfo.baseInfo.laneID = xlsx.read(row, 4).toInt();
        deployInfo.baseInfo.laneType = getLaneType(xlsx.read(row, 5).toString().trimmed());
        deployInfo.baseInfo.roadNetNo = xlsx.read(row, 6).toString().trimmed();
        deployInfo.baseInfo.laneIP = xlsx.read(row, 7).toString().trimmed();
        deployInfo.baseInfo.laneNetmask = xlsx.read(row, 8).toString().trimmed();
        deployInfo.baseInfo.laneGateway = xlsx.read(row, 9).toString().trimmed();
        deployInfo.baseInfo.heartIP = xlsx.read(row, 10).toString().trimmed();
        deployInfo.baseInfo.isConvenientLane = (xlsx.read(row, 11).toString().trimmed() == "是");

        // 抓拍信息加载
        QString capProducer = xlsx.read(row, 12).toString().trimmed();
        QString capIP = xlsx.read(row, 13).toString().trimmed();
        // 产家非空并且IP非空，表示启用抓拍
        deployInfo.capture.isEnable = (!capProducer.isEmpty() && !capIP.isEmpty() && NetworkUtils::isIpV4Address(capIP));
        if (deployInfo.capture.isEnable) {
            deployInfo.capture.ip = capIP;
            deployInfo.capture.triggerMode = getCapTriggerMode(xlsx.read(row, 14).toString().trimmed());
            deployInfo.capture.stationHex = "3501" + deployInfo.baseInfo.stationID;
            deployInfo.capture.stationName = deployInfo.baseInfo.stationName;
            deployInfo.capture.laneID = deployInfo.baseInfo.laneID;
            deployInfo.capture.port = 9588;
            deployInfo.capture.type = 8;
        }

        // 称重信息加载
        QString weightComPort = xlsx.read(row, 15).toString().trimmed();
        // 串口非空，表示启用称重
        deployInfo.weight.isEnable = (!weightComPort.isEmpty());
        if (deployInfo.weight.isEnable) {
            deployInfo.weight.comPort = weightComPort;
            deployInfo.weight.addr = 100;
            deployInfo.weight.baudRate = 9600;
            deployInfo.weight.IWP = 1;
        }

        // 发卡机信息加载
        int cardRobotType = getRobotType(xlsx.read(row, 16).toString().trimmed());
        QString cardRobotIP = xlsx.read(row, 17).toString().trimmed();
        QString cardRobotComPort = xlsx.read(row, 18).toString().trimmed();
        bool cardRobotIPValid = (!cardRobotIP.isEmpty() && NetworkUtils::isIpV4Address(cardRobotIP));
        bool cardRobotComPortValid = (!cardRobotComPort.isEmpty());
        // 产家非空并且IP非空或串口非空，表示启用卡机
        deployInfo.cardRobot.isEnable = (cardRobotType > 0 && (cardRobotIPValid || cardRobotComPortValid));
        if (deployInfo.cardRobot.isEnable) {
            if (cardRobotIPValid) {
                deployInfo.cardRobot.ip = cardRobotIP;
                deployInfo.cardRobot.port = "9588";
                deployInfo.cardRobot.baudRate = 0;
            } else {
                deployInfo.cardRobot.ip = "";
                deployInfo.cardRobot.port = cardRobotComPort;
                deployInfo.cardRobot.baudRate = 9600;
            }
            deployInfo.cardRobot.readerApiFileName = "ICC_HTXX_ROBOT.so";
            deployInfo.cardRobot.upReaderPort = xlsx.read(row, 19).toString().trimmed();
            deployInfo.cardRobot.dnReaderPort = xlsx.read(row, 20).toString().trimmed();
            deployInfo.cardRobot.isShareReader = (xlsx.read(row, 21).toString().trimmed() == QStringLiteral("是"));
            deployInfo.cardRobot.producer = cardRobotType;
            deployInfo.cardRobot.option = 0;
            deployInfo.cardRobot.readerSlot = 2;
            deployInfo.cardRobot.readerApiType = 1;
        }

        // 字符叠加信息加载
        int overlapType = getOverlapType(xlsx.read(row, 22).toString().trimmed());
        QString overlapComPort = xlsx.read(row, 23).toString().trimmed();
        // 串口与产家非空，表示启用字符叠加设备
        deployInfo.overlap.isEnable = (!overlapComPort.isEmpty() && overlapType > 0);
        if (deployInfo.overlap.isEnable) {
            deployInfo.overlap.comPort = overlapComPort;
            deployInfo.overlap.type = overlapType;
            deployInfo.overlap.baudRate = 115200;
        }

        // 费显信息加载
        int feeScrType = getFeeScrType(xlsx.read(row, 24).toString().trimmed());
        QString feeScrIP = xlsx.read(row, 25).toString().trimmed();
        QString feeScrComPort = xlsx.read(row, 26).toString().trimmed();
        bool feeScrIPValid = (!feeScrIP.isEmpty() && NetworkUtils::isIpV4Address(feeScrIP));
        bool feeScrComPortValid = (!feeScrComPort.isEmpty());
        // 产家非空并且串口或IP非空，表示启用费显
        deployInfo.feeScr.isEnable = (feeScrType > 0 && (feeScrIPValid || feeScrComPortValid));
        if (deployInfo.feeScr.isEnable) {
            deployInfo.feeScr.type = feeScrType;
            deployInfo.feeScr.sleepMillisecond = 300;
            if (feeScrIPValid) {
                deployInfo.feeScr.port = feeScrIP;
                deployInfo.feeScr.baudRate = 9588;
            } else {
                deployInfo.feeScr.port = feeScrComPort;
                deployInfo.feeScr.baudRate = 9600;
            }
        }

        // 智能网关信息加载
        QString smartLaneCtrlIP = xlsx.read(row, 27).toString().trimmed();
        deployInfo.smartLaneCtrl.isEnable = (!smartLaneCtrlIP.isEmpty() && NetworkUtils::isIpV4Address(smartLaneCtrlIP));
        // IP非空，表示启用
        if (deployInfo.smartLaneCtrl.isEnable) {
            deployInfo.smartLaneCtrl.ip = smartLaneCtrlIP;
            deployInfo.smartLaneCtrl.port = 9588;
        }

        // 车控器信息加载 - 智能网关使用固定配置，非智能网关现场测量后决定
        if (deployInfo.smartLaneCtrl.isEnable) {
            if (deployInfo.baseInfo.laneType == 1 || deployInfo.baseInfo.laneType == 2) { // 混合车道
                deployInfo.switchDev.baseAddr = "3088";
                deployInfo.switchDev.beginCoil = "001";
                deployInfo.switchDev.ceilingLamp = "051";
                deployInfo.switchDev.coil1 = "";
                deployInfo.switchDev.coil2 = "";
                deployInfo.switchDev.coil3 = "";
                deployInfo.switchDev.coil4 = "";
                deployInfo.switchDev.coilNum = 0;
                deployInfo.switchDev.endCoil = "011";
                deployInfo.switchDev.passingLamp = "041";
                deployInfo.switchDev.railingDown1 = "011";
                deployInfo.switchDev.railingUp1 = "021";
                deployInfo.switchDev.type = 5;
                deployInfo.switchDev.warning = "061";
                deployInfo.switchDev.which = 3;
            } else { // ETC车道
                deployInfo.switchDev.baseAddr = "3088";
                deployInfo.switchDev.beginCoil = "011";
                deployInfo.switchDev.ceilingLamp = "051";
                deployInfo.switchDev.coil1 = "021";
                deployInfo.switchDev.coil2 = "031";
                deployInfo.switchDev.coil3 = "041";
                deployInfo.switchDev.coil4 = "051";
                deployInfo.switchDev.coilNum = 4;
                deployInfo.switchDev.endCoil = "011";
                deployInfo.switchDev.passingLamp = "041";
                deployInfo.switchDev.railingDown1 = "011";
                deployInfo.switchDev.railingUp1 = "021";
                deployInfo.switchDev.type = 5;
                deployInfo.switchDev.warning = "061";
                deployInfo.switchDev.which = 3;
            }
        }

        // 小黄人信息加载
        QString recognizerIP = xlsx.read(row, 28).toString().trimmed();
        deployInfo.recognizer.isEnable = (!recognizerIP.isEmpty() && NetworkUtils::isIpV4Address(recognizerIP));
        // IP非空，表示启用
        if (deployInfo.recognizer.isEnable) {
            deployInfo.recognizer.ip = recognizerIP;
            deployInfo.recognizer.port = 9588;
        }

        // 缴费机信息加载
        int payRobotType = getRobotType(xlsx.read(row, 29).toString().trimmed());
        QString payRobotIP = xlsx.read(row, 30).toString().trimmed();
        deployInfo.payRobot.isEnable = (payRobotType > 0 && !payRobotIP.isEmpty() && NetworkUtils::isIpV4Address(payRobotIP));
        // 产家非空并且IP非空，表示启用缴费机
        if (deployInfo.payRobot.isEnable) {
            deployInfo.payRobot.ip = payRobotIP;
            deployInfo.payRobot.port = "9588";
            deployInfo.payRobot.producer = payRobotType;
            deployInfo.payRobot.readerSlot = 2;
            deployInfo.payRobot.readerApiType = 1;
            deployInfo.payRobot.readerApiFileName = "ICC_HTXX_ROBOT.so";
            deployInfo.payRobot.upReaderPort = xlsx.read(row, 31).toString().trimmed();
            deployInfo.payRobot.dnReaderPort = xlsx.read(row, 32).toString().trimmed();
            deployInfo.payRobot.isShareReader = (xlsx.read(row, 33).toString().trimmed() == QStringLiteral("是"));
        }

        // 天线配置
        QString rsuIP = xlsx.read(row, 34).toString().trimmed();
        deployInfo.rsu.isEnable = (!rsuIP.isEmpty() && NetworkUtils::isIpV4Address(rsuIP));
        // IP非空，表示启用天线
        if (deployInfo.rsu.isEnable) {
            deployInfo.rsu.ip = rsuIP;
            deployInfo.rsu.power = xlsx.read(row, 35).toInt();
            deployInfo.rsu.port = 9527;
        }

        // 桌面读卡器配置
        int readerType = getReaderType(xlsx.read(row, 36).toString().trimmed());
        QString readerComPort = xlsx.read(row, 37).toString().trimmed();
        deployInfo.reader.isEnable = (readerType > 0 && !readerComPort.isEmpty());
        // 产家与串口非空，表示启用桌面读卡器
        if (deployInfo.reader.isEnable) {
            deployInfo.reader.slot = 2;
            deployInfo.reader.comPort = readerComPort;
            deployInfo.reader.apiFileName = "ICC_HTXX.so";
            deployInfo.reader.apiType = 1;
        }

        // Redis配置
        QString redisIP = xlsx.read(row, 38).toString().trimmed();
        deployInfo.redis.isEnable = (!redisIP.isEmpty() && NetworkUtils::isIpV4Address(redisIP));
        if (deployInfo.redis.isEnable) {
            deployInfo.redis.ip = redisIP;
            deployInfo.redis.dbName = "0";
            deployInfo.redis.dbUser = "test";
            deployInfo.redis.password = "Mgskj@202101";
            deployInfo.redis.port = 6379;
        }

        //承载门架配置
        deployInfo.baseInfo.flagHexNo = xlsx.read(row, 39).toString().trimmed();
        deployInfo.baseInfo.flagName = xlsx.read(row, 40).toString().trimmed();

        // 折叠情报板配置
        QString infoboardIP = xlsx.read(row, 41).toString().trimmed();
        deployInfo.infoboard.isEnable = (!infoboardIP.isEmpty() && NetworkUtils::isIpV4Address(infoboardIP));
        if (deployInfo.infoboard.isEnable) {
            deployInfo.infoboard.ip = infoboardIP;
            deployInfo.infoboard.port = 9588;
            deployInfo.infoboard.type = 10;
        }

        m_deployInfos.insert(QString("%1_%2").arg(deployInfo.baseInfo.stationID).arg(deployInfo.baseInfo.laneID), deployInfo);
    }

    LOG_CINFO(L_CATE).noquote() << "Excel文件加载完成: 共有配置数据" << m_deployInfos.size() << "条";
    return true;
}

ST_DeployInfo DeployTool::getCurDeployInfo(const QString &stationID, int laneID)
{
    QString key = QString("%1_%2").arg(stationID, laneID);
    return m_deployInfos[key];
}

bool DeployTool::saveDeviceCtrlFile(const ST_DeployInfo &info, const QString &path, QString &errDesc)
{
    QVariantMap oneMap;

    // 抓拍相机配置
    if (info.capture.isEnable) {
        QVariantMap aMap;
        aMap["LaneID"] = QString::number(info.capture.laneID);
        aMap["StationHex"] = info.capture.stationHex;
        aMap["StationName"] = info.capture.stationName;
        aMap["IP"] = info.capture.ip;
        aMap["Port"] = QString::number(info.capture.port);
        aMap["TriggerMode"] = QString::number(info.capture.triggerMode);
        aMap["Type"] = QString::number(info.capture.type);

        oneMap["Capture"] = aMap;
    }
    // 卡机配置
    if (info.cardRobot.isEnable && info.baseInfo.laneType == 1) {
        QVariantMap robotMap;
        robotMap["BaudRate"] = QString::number(info.cardRobot.baudRate);
        robotMap["ComPort"] = info.cardRobot.port;
        robotMap["IP"] = info.cardRobot.ip;
        robotMap["Option"] = QString::number(info.cardRobot.option);

        oneMap["CardRobot"] = robotMap;

        QVariantMap upReaderMap;
        upReaderMap["ApiFileName"] = info.cardRobot.readerApiFileName;
        upReaderMap["ApiType"] = QString::number(info.cardRobot.readerApiType);
        upReaderMap["SlotNo"] = QString::number(info.cardRobot.readerSlot);

        QVariantMap dnReaderMap;
        dnReaderMap["ApiFileName"] = info.cardRobot.readerApiFileName;
        dnReaderMap["ApiType"] = QString::number(info.cardRobot.readerApiType);
        dnReaderMap["SlotNo"] = QString::number(info.cardRobot.readerSlot);

        if (info.cardRobot.isShareReader && !info.cardRobot.upReaderPort.isEmpty()) {
            dnReaderMap["ComPort"] = "Test1";
        } else if (info.cardRobot.isShareReader && !info.cardRobot.dnReaderPort.isEmpty()) {
            upReaderMap["ComPort"] = "Test2";
        } else {
            upReaderMap["ComPort"] = info.cardRobot.upReaderPort;
            dnReaderMap["ComPort"] = info.cardRobot.dnReaderPort;
        }

        oneMap["CardRobotUpCardReader"] = upReaderMap;
        oneMap["CardRobotDnCardReader"] = dnReaderMap;
    }
    // 缴费机配置
    if (info.payRobot.isEnable && info.baseInfo.laneType == 2) {
        QVariantMap upReaderMap;
        upReaderMap["ApiFileName"] = info.cardRobot.readerApiFileName;
        upReaderMap["ApiType"] = QString::number(info.cardRobot.readerApiType);
        upReaderMap["SlotNo"] = QString::number(info.cardRobot.readerSlot);

        QVariantMap dnReaderMap;
        dnReaderMap["ApiFileName"] = info.cardRobot.readerApiFileName;
        dnReaderMap["ApiType"] = QString::number(info.cardRobot.readerApiType);
        dnReaderMap["SlotNo"] = QString::number(info.cardRobot.readerSlot);

        if (info.cardRobot.isShareReader && !info.cardRobot.upReaderPort.isEmpty()) {
            dnReaderMap["ComPort"] = "Test1";
        } else if (info.cardRobot.isShareReader && !info.cardRobot.dnReaderPort.isEmpty()) {
            upReaderMap["ComPort"] = "Test2";
        } else {
            upReaderMap["ComPort"] = info.cardRobot.upReaderPort;
            dnReaderMap["ComPort"] = info.cardRobot.dnReaderPort;
        }

        oneMap["CardRobotUpCardReader"] = upReaderMap;
        oneMap["CardRobotDnCardReader"] = dnReaderMap;
    }
    // 字符叠加配置
    if (info.overlap.isEnable) {
        QVariantMap aMap;
        aMap["BaudRate"] = QString::number(info.overlap.baudRate);
        aMap["Type"] = QString::number(info.overlap.type);
        aMap["ComPort"] = info.overlap.comPort;

        oneMap["CharOverlap"] = aMap;
    }
    // 费显配置
    if (info.feeScr.isEnable) {
        QVariantMap aMap;
        aMap["SleepMillisecond"] = QString::number(info.feeScr.sleepMillisecond);
        aMap["Type"] = QString::number(info.feeScr.type);
        aMap["BaudRate"] = QString::number(info.feeScr.baudRate);
        aMap["ComPort"] = info.feeScr.port;

        oneMap["FeeScr"] = aMap;
    }
    // 称重配置
    if (info.weight.isEnable) {
        QVariantMap aMap;
        aMap["Addr"] = QString::number(info.weight.addr);
        aMap["BaudRate"] = QString::number(info.weight.baudRate);
        aMap["IWP"] = QString::number(info.weight.IWP);
        aMap["ComPort"] = info.weight.comPort;

        oneMap["Weight"] = aMap;
    }
    // 智能网关配置
    if (info.smartLaneCtrl.isEnable) {
        QVariantMap aMap;
        aMap["IP"] = info.smartLaneCtrl.ip;
        aMap["Port"] = QString::number(info.smartLaneCtrl.port);

        oneMap["SmartLaneController"] = aMap;
    }
    // 车控器配置
    if (info.switchDev.isEnable) {
        QVariantMap aMap;
        aMap["BaseAddr"] = info.switchDev.baseAddr;
        aMap["BeginCoil"] = info.switchDev.beginCoil;
        aMap["CeilingLamp"] = info.switchDev.ceilingLamp;
        aMap["Coil1"] = info.switchDev.coil1;
        aMap["Coil2"] = info.switchDev.coil2;
        aMap["Coil3"] = info.switchDev.coil3;
        aMap["Coil4"] = info.switchDev.coil4;
        aMap["CoilNum"] = QString::number(info.switchDev.coilNum);
        aMap["EndCoil"] = info.switchDev.endCoil;
        aMap["PassingLamp"] = info.switchDev.passingLamp;
        aMap["RailingDown1"] = info.switchDev.railingDown1;
        aMap["RailingUp1"] = info.switchDev.railingUp1;
        aMap["Type"] = QString::number(info.switchDev.type);
        aMap["Warning"] = info.switchDev.warning;
        aMap["Which"] = QString::number(info.switchDev.which);

        oneMap["SwitchDev"] = aMap;
    }
    // 折叠情报板配置
    if (info.infoboard.isEnable) {
        QVariantMap aMap;
        aMap["BaudRate"] = info.infoboard.ip;
        aMap["ComPort"] = QString::number(info.infoboard.port);
        aMap["Type"] = QString::number(info.infoboard.type);

        oneMap["InfoBoard"] = aMap;
    }
    oneMap["LaneType"] = info.baseInfo.laneType;

    QByteArray data = DataDealUtils::mapToJson(oneMap, QJsonDocument::Indented);
    return saveFile(path, data, errDesc);
}

bool DeployTool::saveLaneUIFile(const ST_DeployInfo &info, const QString &path, QString &errDesc)
{
    if (info.baseInfo.laneType == 3 || info.baseInfo.laneType == 4) {
        LOG_CINFO(L_CATE).noquote() << "ETC车道不需要生成LaneUI.json文件";
        return true;
    }

    QVariantMap oneMap;
    if (info.reader.isEnable) {
        QVariantMap aMap;
        aMap["ApiFileName"] = info.reader.apiFileName;
        aMap["ApiType"] = QString::number(info.reader.apiType);
        aMap["ComPort"] = info.reader.comPort;
        aMap["SlotNo"] = QString::number(info.reader.slot);

        oneMap["CardReader"] = aMap;
    }
    if (info.payRobot.isEnable)
        oneMap["Theme"] = "SPT"; // 启用了缴费机，则界面需要切换缴费机模式

    oneMap["LaneType"] = info.baseInfo.laneType;
    oneMap["CfgVersion"] = "202312231614";

    QByteArray data = DataDealUtils::mapToJson(oneMap, QJsonDocument::Indented);
    return saveFile(path, data, errDesc);
}

bool DeployTool::saveLaneBaseConfigFile(const ST_DeployInfo &info, const QString &path, QString &errDesc)
{
    QString content;
    QTextStream stream(&content);

    stream << "[BaseInfo]\n"
           << "LaneID=" << info.baseInfo.laneID << "\n"
           << "LaneType=" << info.baseInfo.laneType << "\n"
           << "StationID=" << info.baseInfo.stationID << "\n"
           << "StationName=" << info.baseInfo.stationName << "\n"
           << "\n"
           << "[LaneDB]\n"
           << "dbhost=" << info.db.ip << "\n"
           << "dbname=" << info.db.dbName << "\n"
           << "dbpasswd=" << info.db.dbPassword << "\n"
           << "dbport=" << info.db.dbPort << "\n"
           << "dbtype=" << info.db.dbType << "\n"
           << "dbuser=" << info.db.dbUser << "\n"
           << "\n";

    if (info.redis.isEnable) {
        stream << "[RedisDB]\n"
               << "dbhost=" << info.redis.ip << "\n"
               << "dbname=" << info.redis.dbName << "\n"
               << "dbpasswd=" << info.redis.password << "\n"
               << "dbport=" << info.redis.port << "\n"
               << "dbuser=" << info.redis.dbUser << "\n";
    }

    return saveFile(path, content.toUtf8(), errDesc);
}

bool DeployTool::saveDtpAgentFile(const ST_DeployInfo &info, const QString &fullBlackName, const QString &path, QString &errDesc)
{
    QString content;
    QTextStream stream(&content);

    stream << "[DataBase]\n"
           << "DBHost=127.0.0.1\n"
           << "DBName=TollLaneDB\n"
           << "DBPasswd=79DFFD04AD9CC740228D427F8CB5FE86\n"
           << "DBPort=0\n"
           << "DBType=2\n"
           << "DBUser=tlman\n"
           << "\n"
           << QString("[Node_%1]\n").arg(info.baseInfo.stationID) << QString("NodeIP=%1\n").arg(info.baseInfo.stationIP) << "NodePort=13591\n"
           << "\n"
           << QString("[Node_%1%2]\n").arg(info.baseInfo.stationID).arg(info.baseInfo.laneID, 2, 10, QLatin1Char('0'))
           << QString("LogicID=%1\n").arg(info.baseInfo.laneID, 2, 10, QLatin1Char('0')) << QString("NodeIP=%1\n").arg(info.baseInfo.stationIP)
           << "NodePort=13591\n"
           << "\n"
           << "[Nodes]\n"
           << "ChildNode=\n"
           << QString("ParentNode=%1\n").arg(info.baseInfo.stationID)
           << QString("SelfNode=%1%2\n").arg(info.baseInfo.stationID).arg(info.baseInfo.laneID, 2, 10, QLatin1Char('0')) << "SiblingNode=\n"
           << "\n"
           << "[SyncDB]\n"
           << "DBExecWait=10\n"
           << QString("DBHost=%1\n").arg(info.baseInfo.stationIP) << "DBName=TollStationDB\n"
           << "DBPasswd=79DFFD04AD9CC740228D427F8CB5FE86\n"
           << "DBPort=0\n"
           << "DBType=1\n"
           << "DBUser=tsman\n"
           << "\n"
           << "[System]\n"
           << QString("Black515=%1\n").arg(fullBlackName) << "SubSysID=4\n"
           << "deal_threads=TradeQ:1, CommandQ:2, MonitorQ:1, ParamQ:2\n"
           << "down_path=/lane_data/fjeit/dtpagent/download\n"
           << "err_path=/lane_data/fjeit/dtpagent/err\n"
           << "log_level=0\n"
           << "recv_path=/lane_data/fjeit/dtpagent/recv\n"
           << "recv_queues=ParamQ, CommandQ\n"
           << "send_path=/lane_data/fjeit/dtpagent/send\n"
           << "sibling_queue=\n"
           << "upload_path=/lane_data/fjeit/LaneMainLocal/upload\n";

    return saveFile(path, content.toUtf8(), errDesc);
}

bool DeployTool::saveStartFile(const ST_DeployInfo &info, const QString &ver, const QString &path, QString &errDesc)
{
    QVariantMap oneMap;
    oneMap["apptype"] = "local";
    oneMap["dtpagent-exepath"] = "/lane_data/fjeit/DtpAgent/DtpAgent.sh";
    oneMap["dtpagent-service"] = 1;
    oneMap["dtpagent-version"] = 23072501;
    oneMap["dtpagent-workdir"] = "/lane_data/fjeit/DtpAgent";
    QString ftpPath;
    if (info.baseInfo.laneType == 1) {
        ftpPath = "LinuxMtcIn";
    } else if (info.baseInfo.laneType == 2) {
        ftpPath = "LinuxMtcOut";
    } else {
        ftpPath = "LinuxEtc";
    }
    oneMap["ftp-path"] = ftpPath;
    oneMap["ftp-pwd"] = "ds18ftp";
    oneMap["ftp-user"] = "ftp";
    oneMap["lane-front-path"] = "/lane_data/fjeit/LaneMainLocal";
    oneMap["lane-front-pro-path"] = "/lane_data/fjeit/LaneMainLocal/LaneMainLocal.sh";
    oneMap["lane-id"] = info.baseInfo.laneID;
    oneMap["lane-type"] = info.baseInfo.laneType;
    oneMap["lanefront-version"] = ver.toInt();
    oneMap["server1"] = info.baseInfo.stationIP;

    QByteArray data = DataDealUtils::mapToJson(oneMap, QJsonDocument::Indented);
    return saveFile(path, data, errDesc);
}

bool DeployTool::updateNetwork(const ST_DeployInfo &info, const QString &interfaceName, QString &errDesc)
{
    errDesc.clear();

    if (m_networkUpdating) {
        errDesc = "网络配置任务正在执行，请勿重复操作";
        return false;
    }

    const QString scriptPath = QDir(QCoreApplication::applicationDirPath()).filePath("script/update_network.sh");
    const QFileInfo scriptInfo(scriptPath);
    if (!scriptInfo.exists() || !scriptInfo.isFile()) {
        errDesc = QStringLiteral("网络配置脚本不存在: %1").arg(scriptPath);
        LOG_CERROR(L_CATE).noquote() << errDesc;
        return false;
    }

    const QString bash = QStandardPaths::findExecutable("bash");
    if (bash.isEmpty()) {
        errDesc = "系统未安装bash，无法执行网络配置脚本";
        LOG_CERROR(L_CATE).noquote() << errDesc;
        return false;
    }

    const QStringList arguments = {scriptPath,
                                   interfaceName.trimmed(),
                                   info.baseInfo.laneIP.trimmed(),
                                   info.baseInfo.laneNetmask.trimmed(),
                                   info.baseInfo.laneGateway.trimmed(),
                                   DNS_1,
                                   DNS_2};

    m_networkProcess->readAllStandardOutput();
    m_networkProcess->readAllStandardError();
    m_networkProcess->setProgram(bash);
    m_networkProcess->setArguments(arguments);

    m_networkTimedOut = false;
    m_networkUpdating = true;
    m_networkProcess->start();            // 启动异步执行脚本
    m_networkTimeoutTimer->start(120000); // 120s超时
    return true;
}

bool DeployTool::syncFeeRate(const QString &filePath, const QString &stationID, QString &errDesc)
{
    errDesc.clear();

    const QFileInfo sourceInfo(filePath);
    if (!sourceInfo.exists() || !sourceInfo.isFile()) {
        errDesc = QString("%1文件不存在").arg(filePath);
        return false;
    }

    FileReader reader;
    if (reader.fetch(filePath, &errDesc)) {
        LOG_CERROR(L_CATE).noquote() << "读取费率文件失败:" << errDesc;
        return false;
    }
    QByteArray fileData = reader.data();

    // 在原始字节中只替换站代码，保持GBK编码、XML声明和排版不变。
    static const QByteArray fromNodeBegin = "<fromnode>";
    static const QByteArray fromNodeEnd = "</fromnode>";
    const int nodeBegin = fileData.indexOf(fromNodeBegin);
    if (nodeBegin < 0) {
        errDesc = QStringLiteral("费率文件中未找到<fromnode>节点");
        return false;
    }

    const int contentBegin = nodeBegin + fromNodeBegin.size();
    const int contentEnd = fileData.indexOf(fromNodeEnd, contentBegin);
    if (contentEnd < 0) {
        errDesc = QStringLiteral("费率文件中的<fromnode>节点不完整");
        return false;
    }
    fileData.replace(contentBegin, contentEnd - contentBegin, stationID.toLatin1());

    // 文件名只替换最后一个下划线后的原站代码。
    const QString sourceBaseName = sourceInfo.completeBaseName();
    const int stationSeparator = sourceBaseName.lastIndexOf(QLatin1Char('_'));
    if (stationSeparator < 0) {
        errDesc = QStringLiteral("费率文件名格式错误: %1").arg(sourceInfo.fileName());
        return false;
    }

    const QString targetFileName = sourceBaseName.left(stationSeparator + 1) + stationID + QLatin1Char('.') + sourceInfo.suffix();
    const QString targetPath = QDir("/lane_data/fjeit/LaneMainLocal/upload").filePath(targetFileName);

    if (!saveFile(targetPath, fileData, errDesc))
        return false;

    LOG_CINFO(L_CATE).noquote() << "590费率文件同步成功:" << targetPath;
    return true;
}

bool DeployTool::syncDBConfig(const ST_DeployInfo &info, QString &errDesc)
{
    errDesc.clear();

    const SqlFactory::DBSetting setting("QMYSQL", info.db.ip, info.db.dbPort, info.db.dbUser, info.db.dbPassword, info.db.dbName);
    m_dbFactory = SqlFactory::getInstance()->config(setting, DB_CONN_NAME);
    if (!m_dbFactory) {
        errDesc = QString("数据库连接 %1 初始化失败: SqlFactory为空").arg(DB_CONN_NAME);
        LOG_CERROR(L_CATE).noquote() << errDesc;
        return false;
    }
    QSqlDatabase sdb = m_dbFactory->getDatabase(DB_CONN_NAME);
    if (!sdb.isValid()) {
        errDesc = QString("数据库连接 %1 初始化失败: 无效的数据库连接").arg(DB_CONN_NAME);
        LOG_CERROR(L_CATE).noquote() << errDesc;
        return false;
    }
    if (!sdb.isOpen()) {
        errDesc = QString("数据库连接 %1 初始化失败: %2").arg(DB_CONN_NAME).arg(sdb.lastError().text());
        LOG_CERROR(L_CATE).noquote() << errDesc;
        return false;
    }

    QStringList errors;

    // 同步t_laneconfig表
    const QString sqlLaneconfig = QStringLiteral("INSERT INTO t_laneconfig "
                                                 "(stationId, laneId, LaneType, ItemKey, ItemValue, reserve, updateTime) "
                                                 "VALUES (:stationId, :laneId, :laneType, :itemKey, :itemValue, NULL, CURRENT_TIMESTAMP) "
                                                 "ON DUPLICATE KEY UPDATE "
                                                 "ItemValue = VALUES(ItemValue), updateTime = CURRENT_TIMESTAMP");
    // 更新小黄人配置
    if (info.recognizer.isEnable) {
        const QVariantMap params1 = {{"stationId", info.baseInfo.stationID},
                                     {"laneId", info.baseInfo.laneID},
                                     {"laneType", info.baseInfo.laneType},
                                     {"itemKey", "VehRecognizer_IP"},
                                     {"itemValue", info.recognizer.ip}};
        const QVariantMap params2 = {{"stationId", info.baseInfo.stationID},
                                     {"laneId", info.baseInfo.laneID},
                                     {"laneType", info.baseInfo.laneType},
                                     {"itemKey", "VehRecognizer_Port"},
                                     {"itemValue", info.recognizer.port}};

        QString itemError;
        if (!executeUpsert(sdb, "t_laneconfig.VehRecognizer_IP", sqlLaneconfig, params1, itemError))
            errors.append(itemError);
        if (!executeUpsert(sdb, "t_laneconfig.VehRecognizer_Port", sqlLaneconfig, params2, itemError))
            errors.append(itemError);
    }
    // 更新天线信息
    if (info.rsu.isEnable) {
        const QVariantMap params1 = {{"stationId", info.baseInfo.stationID},
                                     {"laneId", info.baseInfo.laneID},
                                     {"laneType", info.baseInfo.laneType},
                                     {"itemKey", "NearRsu_IP"},
                                     {"itemValue", info.rsu.ip}};
        const QVariantMap params2 = {{"stationId", info.baseInfo.stationID},
                                     {"laneId", info.baseInfo.laneID},
                                     {"laneType", info.baseInfo.laneType},
                                     {"itemKey", "NearRsu_TxPower"},
                                     {"itemValue", info.rsu.power}};
        const QVariantMap params3 = {{"stationId", info.baseInfo.stationID},
                                     {"laneId", info.baseInfo.laneID},
                                     {"laneType", info.baseInfo.laneType},
                                     {"itemKey", "NearRsu_Port"},
                                     {"itemValue", info.rsu.port}};
        QString itemError;
        if (!executeUpsert(sdb, "t_laneconfig.NearRsu_IP", sqlLaneconfig, params1, itemError))
            errors.append(itemError);
        if (!executeUpsert(sdb, "t_laneconfig.NearRsu_TxPower", sqlLaneconfig, params2, itemError))
            errors.append(itemError);
        if (!executeUpsert(sdb, "t_laneconfig.NearRsu_Port", sqlLaneconfig, params3, itemError))
            errors.append(itemError);
    }
    // 更新缴费机信息
    if (info.payRobot.isEnable) {
        const QVariantMap params1 = {{"stationId", info.baseInfo.stationID},
                                     {"laneId", info.baseInfo.laneID},
                                     {"laneType", info.baseInfo.laneType},
                                     {"itemKey", "SPT_IP"},
                                     {"itemValue", info.payRobot.ip}};
        const QVariantMap params2 = {{"stationId", info.baseInfo.stationID},
                                     {"laneId", info.baseInfo.laneID},
                                     {"laneType", info.baseInfo.laneType},
                                     {"itemKey", "SPT_port"},
                                     {"itemValue", info.payRobot.port}};
        QString itemError;
        if (!executeUpsert(sdb, "t_laneconfig.SPT_IP", sqlLaneconfig, params1, itemError))
            errors.append(itemError);
        if (!executeUpsert(sdb, "t_laneconfig.SPT_port", sqlLaneconfig, params2, itemError))
            errors.append(itemError);
    }

    // 同步t_lanebaseenv表
    const QString sqlLanebaseenv = QStringLiteral("INSERT INTO t_lanebaseenv (EnvKey, EnvName, EnvValue, reserve) "
                                                  "VALUES (:envKey, :envName, :envValue, NULL) "
                                                  "ON DUPLICATE KEY UPDATE "
                                                  "EnvName = VALUES(EnvName), EnvValue = VALUES(EnvValue)");
    const QVariantMap params = {{"envKey", "PSD_LaneHeartBeatUrl"},
                                {"envName", "心跳数据上传URL地址"},
                                {"envValue", QString("http://%1:19115/api/lane/heartbeat").arg(info.baseInfo.heartIP)}};
    QString itemError;
    if (!executeUpsert(sdb, "t_lanebaseenv.PSD_LaneHeartBeatUrl", sqlLanebaseenv, params, itemError))
        errors.append(itemError);

    if (!errors.isEmpty()) {
        errDesc = errors.join(QLatin1Char('\n'));
        LOG_CERROR(L_CATE).noquote() << "同步数据库配置存在失败项:" << errDesc;
        return false;
    }

    LOG_CINFO(L_CATE).noquote() << "数据库配置同步完成";
    return true;
}

bool DeployTool::executeUpsert(const QSqlDatabase &sdb, const QString &itemName, const QString &sql, const QVariantMap &params, QString &errDesc) const
{
    errDesc.clear();

    try {
        Database db(sdb);
        PreparedQuery query = db.prepare(sql);
        QueryResult result = query.exec(params);
        if (result.lastError().isValid()) {
            errDesc = QStringLiteral("同步%1失败: %2").arg(itemName, result.lastError().text());
            return false;
        }

        LOG_CINFO(L_CATE).noquote() << "执行SQL:" << DataDealUtils::fullExecutedQuery(result.unwrappedQuery());
        return true;
    } catch (const DBException &e) {
        errDesc = QStringLiteral("同步%1失败: %2").arg(itemName, e.lastError.text());
        return false;
    }
}

bool DeployTool::isNetworkUpdating() const
{
    return m_networkUpdating;
}

void DeployTool::finishNetworkUpdate(bool success, const QString &message)
{
    if (!m_networkUpdating)
        return;

    m_networkUpdating = false;
    m_networkTimedOut = false;

    if (success) {
        LOG_CINFO(L_CATE).noquote() << message;
    } else {
        LOG_CERROR(L_CATE).noquote() << message;
    }

    emit networkUpdateFinished(success, message);
}

int DeployTool::getLaneType(const QString &str) const
{
    if (str.trimmed() == QStringLiteral("混合入口")) {
        return 1;
    } else if (str.trimmed() == QStringLiteral("混合出口")) {
        return 2;
    } else if (str.trimmed() == QStringLiteral("ETC入口")) {
        return 3;
    } else if (str.trimmed() == QStringLiteral("ETC出口")) {
        return 4;
    } else {
        LOG_CWARNING(L_CATE).noquote() << "未知车道类型:" << str;
        return 0;
    }
}

int DeployTool::getCapTriggerMode(const QString &str) const
{
    if (str == QStringLiteral("视频触发")) {
        return 0;
    } else if (str == QStringLiteral("线圈触发")) {
        return 1;
    } else {
        LOG_CWARNING(L_CATE).noquote() << "未知抓拍触发模式:" << str;
        return -1;
    }
}

int DeployTool::getOverlapType(const QString &str) const
{
    if (str == QStringLiteral("海康")) {
        return 1;
    } else if (str == QStringLiteral("新软")) {
        return 2;
    } else if (str == QStringLiteral("华工1")) {
        return 3;
    } else if (str == QStringLiteral("华工2")) {
        return 4;
    } else if (str == QStringLiteral("广州英沙")) {
        return 5;
    } else {
        LOG_CWARNING(L_CATE).noquote() << "未知字符叠加类型:" << str;
        return 0;
    }
}

int DeployTool::getFeeScrType(const QString &str) const
{
    if (str == QStringLiteral("新大陆")) {
        return 1;
    } else if (str == QStringLiteral("英沙")) {
        return 2;
    } else if (str == QStringLiteral("华工")) {
        return 3;
    } else if (str == QStringLiteral("埃特斯")) {
        return 7;
    } else if (str == QStringLiteral("MGSKJ")) {
        return 8;
    } else if (str == QStringLiteral("MGSKJ2")) {
        return 9;
    } else if (str == QStringLiteral("一体化栏杆机")) {
        return 15;
    } else {
        LOG_CWARNING(L_CATE).noquote() << "未知费显类型:" << str;
        return 0;
    }
}

int DeployTool::getReaderType(const QString &str) const
{
    if (str == QStringLiteral("航天金卡")) {
        return 1;
    } else if (str == QStringLiteral("汇豪")) {
        return 2;
    } else {
        LOG_CWARNING(L_CATE).noquote() << "未知读卡器类型:" << str;
        return 0;
    }
}

int DeployTool::getRobotType(const QString &str) const
{
    if (str == QStringLiteral("朗为")) {
        return 1;
    } else if (str == QStringLiteral("U创")) {
        return 2;
    } else if (str == QStringLiteral("汇豪")) {
        return 3;
    } else if (str == QStringLiteral("福赛特")) {
        return 4;
    } else if (str == QStringLiteral("特微")) {
        return 5;
    } else {
        LOG_CWARNING(L_CATE).noquote() << "未知卡机类型:" << str;
        return 0;
    }
}

bool DeployTool::saveFile(const QString &path, const QByteArray &data, QString &errDesc) const
{
    errDesc.clear();

    const QFileInfo targetInfo(path);
    QDir targetDir = targetInfo.dir();
    if (!targetDir.exists() && !targetDir.mkpath(QStringLiteral("."))) {
        errDesc = QStringLiteral("创建目录失败: %1").arg(targetDir.absolutePath());
        return false;
    }

    QString fileName = targetInfo.fileName();
    LOG_CINFO(L_CATE).noquote() << "待写入" << fileName << "数据:" << data;

    FileSaver saver(targetInfo.absoluteFilePath());
    if (!saver.write(data)) {
        LOG_CERROR(L_CATE).noquote() << "将数据写入到" << fileName << "失败:" << saver.errorString();
        errDesc = QStringLiteral("写入%1失败: %2").arg(fileName, saver.errorString());
        return false;
    }
    if (!saver.finalize()) {
        LOG_CERROR(L_CATE).noquote() << "将数据保存到" << fileName << "失败:" << saver.errorString();
        errDesc = QStringLiteral("保存%1失败: %2").arg(fileName, saver.errorString());
        return false;
    }

    LOG_CINFO(L_CATE).noquote() << "信息成功写入" << fileName;
    return true;
}

// --------------------------------------------------------
IDeployTool *createDeployTool()
{
    return new DeployTool();
}

void destroyDeployTool(IDeployTool *tool)
{
    delete tool;
}
