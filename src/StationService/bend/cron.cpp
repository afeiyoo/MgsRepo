#include "cron.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QXmlStreamWriter>

#include "HttpClient/src/http.h"
#include "Logger.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "utils/datadealutils.h"
#include "utils/fileutils.h"

using namespace Utils;

Cron::Cron(QObject *parent)
    : QThread{parent}
{}

Cron::~Cron()
{
    if (isRunning()) {
        requestInterruption();
        wait(5000);
    }
}

void Cron::checkJsonToDelete()
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

int Cron::getIncrementFiles()
{
    QString curVersion = GM_INS->getCurBlackVersion();
    if (curVersion.isEmpty()) {
        curVersion = DataDealUtils::curDateTimeStr("yyyyMMddhh") + "00";
        LOG_INFO().noquote() << "增量版本号未配置: 从版本" << curVersion << "开始下载";
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
        return 0;
    }

    int remainCount = 0;
    int downloadCount = 0;
    QString reqVersion = date.toString("yyyyMMddhhmm");
    QString reqUrl = GM_INS->m_conf->m_incrementCheckUrl + "/" + reqVersion;
    do {
        LOG_ERROR().noquote() << "开始请求获取增量文件 Url: " << reqUrl << " 请求版本: " << reqVersion;

        if (remainCount != 0)
            reqUrl = GM_INS->m_conf->m_incrementCheckUrl + "/" + reqVersion + "/" + QString::number(remainCount);

        QByteArray retMsg;
        Http client;
        bool ok = client.getSync(retMsg, QUrl(reqUrl));
        if (!ok || retMsg.isEmpty()) {
            LOG_INFO().noquote() << QString("%1版本增量文件下载失败:").arg(reqVersion) << retMsg;
            return -1;
        }

        bool jsonOk = false;
        QString jsonErrDesc;
        QVariantMap resMap = DataDealUtils::jsonToMap(retMsg, &jsonOk, &jsonErrDesc);
        if (!jsonOk) {
            LOG_ERROR().noquote() << QString("%1版本增量文件内容异常:").arg(reqVersion) << jsonErrDesc;
            return -1;
        }

        QString dataType;
        if (resMap.contains("dataType"))
            dataType = resMap["dataType"].toString();
        if (resMap.contains("remainCount"))
            remainCount = resMap["remainCount"].toInt();

        if (dataType.isEmpty()) {
            LOG_ERROR().noquote() << "dataType值异常!";
            return -1;
        }

        // 创建保存目录
        QString versionDir = QString("%1/download/%2").arg(QCoreApplication::applicationDirPath()).arg(reqVersion.left(8));
        FileUtils::makeSureDirExist(FileName::fromString(versionDir));

        FileName file = FileName::fromString(QString("%1/%2_%3.json").arg(versionDir).arg(reqVersion).arg(dataType));
        FileSaver saver(file.toString());
        if (!saver.write(retMsg)) {
            LOG_ERROR().noquote() << "数据写入文件失败:" << file.fileName();
            return -1;
        }
        if (saver.finalize()) {
            LOG_INFO().noquote() << QString("增量文件%1保存成功").arg(file.fileName());
            GM_INS->saveCurBlackVersion(reqVersion);
            downloadCount++;
        } else {
            LOG_ERROR().noquote() << QString("增量文件%1保存失败").arg(file.fileName());
            return -1;
        }
    } while (remainCount > 0);

    return downloadCount;
}

bool Cron::checkVersionIsValid(const QString &version) const
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
    DataDealUtils::jsonToMap(reader.data(), &jsonOk, &jsonErrDesc);

    if (!jsonOk) {
        LOG_ERROR().noquote() << QString("增量文件%1数据异常:").arg(file.fileName()) << jsonErrDesc;
        return false;
    }

    return true;
}

int Cron::getFullFiles()
{
    // 获取远程版本号
    LOG_INFO().noquote() << "开始请求获取远程BlackUpdate.xml: " << GM_INS->m_conf->m_fullCheckUrl;

    QUrl reqUrl = QUrl(GM_INS->m_conf->m_fullCheckUrl);
    Http client;
    QByteArray retMsg;
    bool netOk = client.getSync(retMsg, reqUrl);
    if (!netOk || retMsg.isEmpty()) {
        LOG_ERROR().noquote() << "获取远程BlackUpdate.xml失败，原因:" << retMsg;
        return -1;
    }

    LOG_INFO().noquote() << "获取远程BlackUpdate.xml成功!";

    bool jsonOk = false;
    QString jsonErrDesc;
    QVariantMap resMap = DataDealUtils::jsonToMap(retMsg, &jsonOk, &jsonErrDesc);
    if (!jsonOk) {
        LOG_ERROR().noquote() << "远程BlackUpdate.xml解析异常:" << jsonErrDesc;
        return -1;
    }

    QString remoteVer;
    QString totalMD5;
    QVariantList fileList;
    QVariantList md5List;
    QVariantList urlList;
    if (resMap.contains("version"))
        remoteVer = resMap["version"].toString();
    if (resMap.contains("totalMD5"))
        totalMD5 = resMap["totalMD5"].toString();
    if (resMap.contains("fileList"))
        fileList = resMap["fileList"].toList();
    if (resMap.contains("md5List"))
        md5List = resMap["md5List"].toList();
    if (resMap.contains("urlList"))
        urlList = resMap["urlList"].toList();

    if (remoteVer.isEmpty()) {
        LOG_ERROR().noquote() << "远程BlackUpdate.xml中version值为空";
        return -1;
    }
    if (totalMD5.isEmpty()) {
        LOG_ERROR().noquote() << "远程BlackUpdate.xml中totalMD5值为空";
        return -1;
    }
    if (fileList.size() != md5List.size() || fileList.size() != urlList.size() || md5List.size() != urlList.size()) {
        LOG_ERROR().noquote() << "远程BlackUpdate.xml中urlList, md5List, fileList长度不一致";
        return -1;
    }

    // 获取本地版本号，进行比较分析
    bool startDownload = false;
    QString localVer;
    FileName infoFile = FileName::fromString(GM_INS->m_conf->m_fullSavePath + "/BlackUpdate.xml");
    if (!infoFile.exists()) {
        LOG_INFO().noquote() << "本地BlackUpdate.xml不存在，启动全量文件下载";
        startDownload = true;
    } else {
        FileReader reader;
        if (!reader.fetch(infoFile.toString())) {
            LOG_ERROR().noquote() << "读取本地BlackUpdate.xml失败:" << reader.errorString();
            return -1;
        }

        bool xmlOk = false;
        QString xmlErrDesc;
        QVariantMap infoMap = DataDealUtils::xmlToMap(reader.data(), &xmlOk, &xmlErrDesc);
        if (!xmlOk) {
            LOG_ERROR().noquote() << "解析本地BlackUpdate.xml失败:" << xmlErrDesc;
            return -1;
        }

        localVer = infoMap["batchno"].toString();

        LOG_INFO().noquote() << "远程全量版本号:" << remoteVer << "本地全量版本号:" << localVer;
        if (localVer.toUInt() < remoteVer.toUInt()) {
            LOG_INFO().noquote() << "远程全量版本号高于本地全量版本号，启动全量文件下载";
            startDownload = true;
        }
    }

    if (!startDownload) {
        LOG_INFO().noquote() << "经检查当前全量文件版本已是最新，无需下载！";
        return 0;
    }

    // 启动下载
    int downloadRes = downloadFullFiles(fileList, md5List, urlList);
    if (downloadRes < 0) {
        return -1;
    }

    // 所有全量文件下载成功，将json文件内容转换为XML文件，并保存
    if (!saveFullXml(resMap))
        return -1;

    // 删除旧版本全量文件
    removeOldFullFiles(localVer);

    return 0;
}

int Cron::downloadFullFiles(const QVariantList &fileList, const QVariantList &md5List, const QVariantList &urlList)
{
    Http client;
    QStringList downloadedFiles;

    for (int i = 0; i < urlList.size(); ++i) {
        QString fileName = fileList.at(i).toString();
        QUrl url(urlList.at(i).toString());

        QString filePath = QDir(GM_INS->m_conf->m_fullSavePath).filePath(fileName);
        FileName file = FileName::fromString(filePath);
        FileUtils::makeSureDirExist(file.parentDir());

        LOG_INFO().noquote() << "开始下载全量文件:" << url.toString() << "保存路径:" << filePath;

        QByteArray fileData;
        bool netOk = client.getSync(fileData, url);
        if (!netOk) {
            LOG_ERROR().noquote() << "下载全量文件失败:" << url.toString() << "原因:" << fileData;
            removeDownloadedFiles(downloadedFiles);
            return -1;
        }

        FileSaver saver(file.toString());
        if (!saver.write(fileData)) {
            LOG_ERROR().noquote() << "写入全量文件" << file.fileName() << "失败:" << saver.errorString();
            removeDownloadedFiles(downloadedFiles);
            return -1;
        }
        if (!saver.finalize()) {
            LOG_ERROR().noquote() << "保存全量文件" << file.fileName() << "失败:" << saver.errorString();
            removeDownloadedFiles(downloadedFiles);
            return -1;
        }

        downloadedFiles.append(filePath);
    }

    for (int i = 0; i < downloadedFiles.size(); ++i) {
        bool md5Ok = false;
        QString actualMd5 = DataDealUtils::bigFileMd5(downloadedFiles.at(i), &md5Ok);
        QString expectedMd5 = md5List.at(i).toString().trimmed().toUpper();

        FileName file = FileName::fromString(downloadedFiles.at(i));
        if (!md5Ok) {
            LOG_ERROR().noquote() << "对全量文件" << file.fileName() << "计算MD5失败!";
            removeDownloadedFiles(downloadedFiles);
            return -1;
        }
        if (actualMd5 != expectedMd5) {
            LOG_ERROR().noquote() << "全量文件MD5校验失败:" << file.fileName() << "期望:" << expectedMd5 << "实际:" << actualMd5;
            removeDownloadedFiles(downloadedFiles);
            return -1;
        }
    }

    LOG_INFO().noquote() << "全量文件全部下载并校验成功, 文件数:" << downloadedFiles.size();
    return 0;
}

void Cron::removeDownloadedFiles(const QStringList &filePaths)
{
    for (const QString &filePath : filePaths) {
        if (filePath.isEmpty())
            continue;

        FileName file = FileName::fromString(filePath);
        if (!file.exists())
            continue;

        QString errStr;
        bool ok = FileUtils::removeRecursively(file, &errStr);
        if (!ok) {
            LOG_ERROR().noquote() << "删除文件" << file.fileName() << "失败: " << errStr;
        }
    }
}

bool Cron::saveFullXml(const QVariantMap &map)
{
    QString version = map.value("version").toString();
    QString totalMD5 = map.value("totalMD5").toString();
    QVariantList fileList = map.value("fileList").toList();
    QVariantList md5List = map.value("md5List").toList();

    QString savePath = GM_INS->m_conf->m_fullSavePath + "/BlackUpdate.xml";
    FileName file = FileName::fromString(savePath);
    FileUtils::makeSureDirExist(file.parentDir());

    FileSaver saver(file.toString());

    QXmlStreamWriter writer(saver.file());
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);
    writer.setCodec(QTextCodec::codecForName("GBK"));

    writer.writeStartDocument("1.0");
    writer.writeStartElement("blackupdate");

    writer.writeTextElement("batchno", version);

    writer.writeStartElement("totalmd5");
    writer.writeTextElement("paramtype", "515");
    writer.writeTextElement("md5", totalMD5);
    writer.writeEndElement();

    for (int i = 0; i < fileList.size(); ++i) {
        QString zipFile = fileList.at(i).toString();
        QString md5 = md5List.at(i).toString();

        writer.writeStartElement("parameter");
        writer.writeTextElement("paramtype", "515");
        writer.writeTextElement("zipfile", zipFile);
        writer.writeTextElement("md5", md5);
        writer.writeTextElement("operatelevel", "4");
        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndDocument();

    saver.setResult(&writer);
    if (!saver.finalize()) {
        LOG_ERROR().noquote() << "保存BlackUpdate.xml失败:" << saver.errorString();
        return false;
    }

    LOG_INFO().noquote() << "保存BlackUpdate.xml成功:" << file.fileName();
    return true;
}

void Cron::removeOldFullFiles(const QString &ver)
{
    QDir dir(GM_INS->m_conf->m_fullSavePath);
    if (!dir.exists())
        return;

    dir.setFilter(QDir::Files);
    dir.setNameFilters({QString("*%2*").arg(ver)});

    QFileInfoList fileInfoList = dir.entryInfoList();
    for (const auto &fi : fileInfoList) {
        QString errStr;
        if (FileUtils::removeRecursively(FileName(fi), &errStr)) {
            LOG_INFO().noquote() << "成功删除旧版本" << ver << "全量文件:" << fi.fileName();
        } else {
            LOG_ERROR().noquote() << "未成功删除旧版本" << ver << "全量文件:" << fi.fileName();
        }
    }
}

void Cron::run()
{
    while (!isInterruptionRequested()) {
        // 检查过期文件，每三十分钟检查一次
        if (DataDealUtils::curUnixDateTime() - m_delJsonTime > 30 * 60) {
            m_delJsonTime = DataDealUtils::curUnixDateTime();
            checkJsonToDelete();
        }
        // 检查增量文件，每2.5分钟检查一次
        if (DataDealUtils::curUnixDateTime() - m_checkIncrementTime > 2 * 60 + 30) {
            m_checkIncrementTime = DataDealUtils::curUnixDateTime();
            getIncrementFiles();
        }
        // 检查全量文件，每30分钟检查一次
        if (DataDealUtils::curUnixDateTime() - m_checkFullTime > 30 * 60) {
            m_checkFullTime = DataDealUtils::curUnixDateTime();
            getFullFiles();
        }

        for (int i = 0; i < 10 && !isInterruptionRequested(); ++i)
            QThread::sleep(1);
    }
}
