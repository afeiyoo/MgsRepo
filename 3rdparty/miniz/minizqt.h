#pragma once

#include <QList>
#include <QString>

namespace MinizQt {

struct ArchiveEntry
{
    int index = -1;
    QString fileName;
    quint64 compressedSize = 0;
    quint64 uncompressedSize = 0;
    bool isDirectory = false;
    bool isEncrypted = false;
    bool isSupported = false;
};

// 获取ZIP内文件列表和属性
QList<ArchiveEntry> archiveEntries(const QString &archivePath, bool *ok = nullptr, QString *errorString = nullptr);

// 流式解压指定条目到outputFilePath. 成功后，目标文件会被原子性地替换。
bool extractFile(const QString &archivePath, int entryIndex, const QString &outputFilePath, QString *errorString = nullptr);
bool extractFile(const QString &archivePath,
                 const QString &entryName,
                 const QString &outputFilePath,
                 QString *errorString = nullptr);

// 安全解压整个ZIP。绝对路径和遍历父级路径将被拒绝。
bool extractAll(const QString &archivePath, const QString &destinationPath, QString *errorString = nullptr);

} // namespace MinizQt
