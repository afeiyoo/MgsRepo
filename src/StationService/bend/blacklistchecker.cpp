#include "blacklistchecker.h"

#include <QDir>

#include "HttpClient/src/http.h"
#include "Logger.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"

using namespace Utils;

BlackListChecker::BlackListChecker(QObject *parent)
    : QThread{parent}
{
    m_delJsonTime = DataDealUtils::curUnixDateTime();
    m_checkJsonTime = DataDealUtils::curUnixDateTime();
}

BlackListChecker::~BlackListChecker() {}

void BlackListChecker::checkJsonToDelete()
{
    QDateTime curDate = DataDealUtils::curDateTime();
    QString saveDirPath = QString("%1/download").arg(FileUtils::curApplicationDirPath()); // 增量文件存储路径
    QDate expireDate = curDate.addDays(-GM_INS->m_conf->m_saveDays).date();               // 到期文件的日期

    QDir dir(saveDirPath);
    if (!dir.exists())
        return;

    // 一天只删除一次
    if (!m_curDateStr.isEmpty() && curDate.toString("yyyyMMdd") == m_curDateStr)
        return;
    m_curDateStr = curDate.toString("yyyyMMdd");

    QFileInfoList dirList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (const QFileInfo &dirInfo, dirList) {
        QString name = dirInfo.fileName();
        QDate d = QDate::fromString(name, "yyyyMMdd");
        if (!d.isValid())
            continue;

        if (d < expireDate) {
            QString sDelPath = dirInfo.absoluteFilePath();
            QString errStr;
            bool ans = FileUtils::removeRecursively(FileName::fromString(sDelPath), &errStr);
            if (ans) {
                LOG_INFO().noquote() << "删除过期增量目录成功:" << sDelPath;
            } else {
                LOG_ERROR().noquote() << "删除过期增量目录失败:" << errStr;
            }
        }
    }
}

void BlackListChecker::getBlackJsonMsg()
{
    QString curVersion = GM_INS->getCurBlackVersion();
    if (curVersion.isEmpty()) {
        curVersion = DataDealUtils::curDateTimeStr("yyyyMMddhh") + "00";
    }

    QDateTime date = QDateTime::fromString(curVersion, "yyyyMMddhhmm");
    if (checkVersionIsValid(curVersion)) {
        // 当前版本号文件已经存在,下载下一个版本（5分钟为一个版本）
        date = date.addSecs(5 * 60);
    } else {
        LOG_INFO().noquote() << QString("%1版本增量文件异常,重新下载").arg(curVersion);
    }

    if (date > QDateTime::currentDateTime()) {
        LOG_INFO().noquote() << QString("版本号%1大于当前日期%2,数据未准备好")
                                    .arg(date.toString("yyyyMMddhhmm"), DataDealUtils::curDateTimeStr("yyyyMMddhhmm"));
        return;
    }

    int remainCount = 0;
    QString reqVersion = date.toString("yyyyMMddhhmm");
    QString reqUrl = GM_INS->m_conf->m_blackCheckUrl + "/" + reqVersion;
    do {
        LOG_INFO().noquote() << "开始请求获取增量状态名单信息 Url: " << reqUrl << " ReqMsg: " << reqVersion;

        if (remainCount != 0)
            reqUrl = GM_INS->m_conf->m_blackCheckUrl + "/" + reqVersion + "/" + QString::number(remainCount);

        QByteArray retMsg;
        Http client;
        bool ok = client.getSync(retMsg, QUrl(reqUrl));
        if (!ok || retMsg.isEmpty()) {
            LOG_INFO().noquote() << QString("%1版本增量文件下载失败").arg(reqVersion);
            return;
        }

        // 创建保存目录
        QString versionDir = QString("%1/download/%2").arg(QCoreApplication::applicationDirPath()).arg(reqVersion.left(8));
        FileUtils::makeSureDirExist(FileName::fromString(versionDir));

        bool jsonOk = false;
        QString jsonErrDesc;
        QVariantMap resMap = DataDealUtils::jsonToMap(retMsg, jsonOk, jsonErrDesc);
        if (!jsonOk) {
            LOG_ERROR().noquote() << QString("%1增量文件内容异常!").arg(reqVersion);
            return;
        }

        QString dataType;
        if (resMap.contains("dataType"))
            dataType = resMap["dataType"].toString();
        if (resMap.contains("remainCount"))
            remainCount = resMap["remainCount"].toInt();

        if (dataType.isEmpty()) {
            LOG_ERROR().noquote() << "dataType值异常!";
            return;
        }

        FileName file = FileName::fromString(QString("%1/%2_%3.json").arg(versionDir).arg(reqVersion).arg(dataType));
        FileSaver saver(file.toString());
        if (!saver.write(retMsg)) {
            LOG_ERROR().noquote() << "数据写入文件失败:" << file.fileName();
            return;
        }
        if (saver.finalize()) {
            LOG_INFO().noquote() << QString("下载%1,%2版本完成").arg(reqVersion).arg(dataType);
            GM_INS->saveCurBlackVersion(reqVersion);
        } else {
            LOG_INFO().noquote() << QString("下载%1,%2版本失败").arg(reqVersion).arg(dataType);
            return;
        }
    } while (remainCount > 0);

    return;
}

bool BlackListChecker::checkVersionIsValid(const QString &version) const
{
    QString dirPath = QString("%1/download/%2/").arg(QCoreApplication::applicationDirPath()).arg(version.left(8));
    FileName file = FileName::fromString(dirPath + QString("%1_queryETCBlack.json").arg(version));
    if (!file.exists()) {
        LOG_ERROR().noquote() << "增量文件不存在:" << file.fileName();
        return false;
    }

    FileReader reader;
    QString errStr;
    if (!reader.fetch(file.toString(), &errStr)) {
        LOG_ERROR().noquote() << QString("增量文件%1读取失败:").arg(file.fileName()) << errStr;
        return false;
    }

    bool jsonOk = false;
    QString jsonErrDesc;
    DataDealUtils::jsonToMap(reader.data(), jsonOk, jsonErrDesc);

    if (!jsonOk) {
        LOG_ERROR().noquote() << QString("增量文件%1数据异常:").arg(file.fileName()) << jsonErrDesc;
        return false;
    }

    return true;
}

void BlackListChecker::run()
{
    while (1) {
        // 检查过期文件，每五分钟检查一次
        if (DataDealUtils::curUnixDateTime() - m_delJsonTime > 5 * 60) {
            m_delJsonTime = DataDealUtils::curUnixDateTime();
            checkJsonToDelete();
        }
        // 检查增量文件，每五分钟检查一次
        if (DataDealUtils::curUnixDateTime() - m_checkJsonTime > 5 * 60) {
            m_checkJsonTime = DataDealUtils::curUnixDateTime();
            getBlackJsonMsg();
        }

        QThread::sleep(10);
    }
}
