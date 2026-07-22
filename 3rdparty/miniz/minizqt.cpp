#include "minizqt.h"

#include <limits>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QSet>
#include <QStorageInfo>

#include "miniz.h"

namespace {

void setError(QString *errorString, const QString &message)
{
    if (errorString)
        *errorString = message;
}

size_t readArchiveData(void *opaque, mz_uint64 offset, void *buffer, size_t size)
{
    QFile *file = static_cast<QFile *>(opaque);
    if (!file || offset > static_cast<mz_uint64>(std::numeric_limits<qint64>::max()) || !file->seek(static_cast<qint64>(offset))) {
        return 0;
    }

    const qint64 readSize = file->read(static_cast<char *>(buffer), static_cast<qint64>(size));
    return readSize > 0 ? static_cast<size_t>(readSize) : 0;
}

size_t writeExtractedData(void *opaque, mz_uint64 offset, const void *buffer, size_t size)
{
    QSaveFile *file = static_cast<QSaveFile *>(opaque);
    if (!file || offset > static_cast<mz_uint64>(std::numeric_limits<qint64>::max())
        || (file->pos() != static_cast<qint64>(offset) && !file->seek(static_cast<qint64>(offset)))) {
        return 0;
    }

    const qint64 written = file->write(static_cast<const char *>(buffer), static_cast<qint64>(size));
    return written > 0 ? static_cast<size_t>(written) : 0;
}

QString archiveError(mz_zip_archive *archive)
{
    return QString::fromLatin1(mz_zip_get_error_string(mz_zip_get_last_error(archive)));
}

class ArchiveReader
{
public:
    ArchiveReader() = default;
    Q_DISABLE_COPY(ArchiveReader)

    ~ArchiveReader()
    {
        if (m_initialized)
            mz_zip_reader_end(&m_archive);
    }

    bool open(const QString &archivePath, QString *errorString)
    {
        m_file.setFileName(archivePath);
        if (!m_file.open(QIODevice::ReadOnly)) {
            setError(errorString, QStringLiteral("打开ZIP失败: %1").arg(m_file.errorString()));
            return false;
        }

        mz_zip_zero_struct(&m_archive);
        m_archive.m_pRead = readArchiveData;
        m_archive.m_pIO_opaque = &m_file;
        if (!mz_zip_reader_init(&m_archive, static_cast<mz_uint64>(m_file.size()), 0)) {
            setError(errorString, QStringLiteral("初始化ZIP失败: %1").arg(archiveError(&m_archive)));
            return false;
        }
        m_initialized = true;
        return true;
    }

    mz_zip_archive *archive() { return &m_archive; }

private:
    QFile m_file;
    mz_zip_archive m_archive;
    bool m_initialized = false;
};

bool readEntry(mz_zip_archive *archive, mz_uint index, MinizQt::ArchiveEntry *entry, QString *errorString)
{
    mz_zip_archive_file_stat stat;
    if (!mz_zip_reader_file_stat(archive, index, &stat)) {
        setError(errorString, QStringLiteral("读取ZIP条目信息失败: %1").arg(archiveError(archive)));
        return false;
    }

    const mz_uint nameSize = mz_zip_reader_get_filename(archive, index, nullptr, 0);
    QByteArray fileNameBytes(static_cast<int>(nameSize), '\0');
    if (nameSize == 0 || mz_zip_reader_get_filename(archive, index, fileNameBytes.data(), nameSize) == 0) {
        setError(errorString, QStringLiteral("读取ZIP条目名称失败"));
        return false;
    }

    fileNameBytes.chop(1);
    entry->index = static_cast<int>(index);
    entry->fileName = QString::fromUtf8(fileNameBytes.constData(), fileNameBytes.size()).replace('\\', '/');
    entry->compressedSize = stat.m_comp_size;
    entry->uncompressedSize = stat.m_uncomp_size;
    entry->isDirectory = stat.m_is_directory;
    entry->isEncrypted = stat.m_is_encrypted;
    entry->isSupported = stat.m_is_supported;
    return true;
}

bool isSafeRelativePath(const QString &fileName)
{
    if (fileName.isEmpty() || fileName.contains(QChar::Null) || QDir::isAbsolutePath(fileName) || fileName.startsWith('/')
        || fileName.contains(':')) {
        return false;
    }

    const QString cleanPath = QDir::cleanPath(fileName);
    return cleanPath != QStringLiteral(".") && cleanPath != QStringLiteral("..") && !cleanPath.startsWith(QStringLiteral("../"));
}

QList<MinizQt::ArchiveEntry> readEntries(ArchiveReader *reader, bool *ok, QString *errorString)
{
    if (ok)
        *ok = false;

    QList<MinizQt::ArchiveEntry> entries;
    const mz_uint count = mz_zip_reader_get_num_files(reader->archive());
    if (count > static_cast<mz_uint>(std::numeric_limits<int>::max())) {
        setError(errorString, QStringLiteral("ZIP条目数量过多"));
        return {};
    }

    entries.reserve(static_cast<int>(count));
    for (mz_uint i = 0; i < count; ++i) {
        MinizQt::ArchiveEntry entry;
        if (!readEntry(reader->archive(), i, &entry, errorString))
            return {};
        entries.append(entry);
    }

    if (ok)
        *ok = true;
    return entries;
}

bool extractEntry(ArchiveReader *reader,
                  const MinizQt::ArchiveEntry &entry,
                  const QString &outputFilePath,
                  QString *errorString)
{
    if (entry.index < 0 || static_cast<mz_uint>(entry.index) >= mz_zip_reader_get_num_files(reader->archive())) {
        setError(errorString, QStringLiteral("ZIP条目索引无效: %1").arg(entry.index));
        return false;
    }
    if (entry.isDirectory || entry.isEncrypted || !entry.isSupported) {
        setError(errorString, QStringLiteral("ZIP条目不可解压: %1").arg(entry.fileName));
        return false;
    }

    const QFileInfo outputInfo(outputFilePath);
    if (outputFilePath.isEmpty() || !QDir().mkpath(outputInfo.absolutePath())) {
        setError(errorString, QStringLiteral("创建解压目录失败: %1").arg(outputInfo.absolutePath()));
        return false;
    }

    const QStorageInfo storage(outputInfo.absolutePath());
    if (storage.isValid() && storage.bytesAvailable() >= 0
        && entry.uncompressedSize > static_cast<quint64>(storage.bytesAvailable())) {
        setError(errorString, QStringLiteral("磁盘空间不足，无法解压: %1").arg(entry.fileName));
        return false;
    }

    QSaveFile output(outputFilePath);
    if (!output.open(QIODevice::WriteOnly)) {
        setError(errorString, QStringLiteral("创建解压文件失败: %1").arg(output.errorString()));
        return false;
    }
    if (!mz_zip_reader_extract_to_callback(reader->archive(), static_cast<mz_uint>(entry.index), writeExtractedData, &output, 0)) {
        output.cancelWriting();
        setError(errorString, QStringLiteral("解压ZIP条目失败: %1").arg(archiveError(reader->archive())));
        return false;
    }
    if (static_cast<quint64>(output.size()) != entry.uncompressedSize) {
        output.cancelWriting();
        setError(errorString, QStringLiteral("解压文件大小不正确: %1").arg(entry.fileName));
        return false;
    }
    if (!output.commit()) {
        setError(errorString, QStringLiteral("保存解压文件失败: %1").arg(output.errorString()));
        return false;
    }
    return true;
}

} // namespace

namespace MinizQt {

QList<ArchiveEntry> archiveEntries(const QString &archivePath, bool *ok, QString *errorString)
{
    if (ok)
        *ok = false;
    if (errorString)
        errorString->clear();

    ArchiveReader reader;
    if (!reader.open(archivePath, errorString))
        return {};

    return readEntries(&reader, ok, errorString);
}

bool extractFile(const QString &archivePath, int entryIndex, const QString &outputFilePath, QString *errorString)
{
    if (errorString)
        errorString->clear();

    ArchiveReader reader;
    if (!reader.open(archivePath, errorString))
        return false;

    if (entryIndex < 0 || static_cast<mz_uint>(entryIndex) >= mz_zip_reader_get_num_files(reader.archive())) {
        setError(errorString, QStringLiteral("ZIP条目索引无效: %1").arg(entryIndex));
        return false;
    }

    ArchiveEntry entry;
    if (!readEntry(reader.archive(), static_cast<mz_uint>(entryIndex), &entry, errorString))
        return false;
    return extractEntry(&reader, entry, outputFilePath, errorString);
}

bool extractFile(const QString &archivePath, const QString &entryName, const QString &outputFilePath, QString *errorString)
{
    if (errorString)
        errorString->clear();

    ArchiveReader reader;
    if (!reader.open(archivePath, errorString))
        return false;

    bool entriesOk = false;
    const QList<ArchiveEntry> entries = readEntries(&reader, &entriesOk, errorString);
    if (!entriesOk)
        return false;

    const QString normalizedName = QString(entryName).replace('\\', '/');
    const ArchiveEntry *matchedEntry = nullptr;
    for (const ArchiveEntry &entry : entries) {
        if (entry.fileName != normalizedName)
            continue;
        if (matchedEntry) {
            setError(errorString, QStringLiteral("ZIP中存在重复条目: %1").arg(entryName));
            return false;
        }
        matchedEntry = &entry;
    }
    if (!matchedEntry) {
        setError(errorString, QStringLiteral("ZIP中不存在条目: %1").arg(entryName));
        return false;
    }
    return extractEntry(&reader, *matchedEntry, outputFilePath, errorString);
}

bool extractAll(const QString &archivePath, const QString &destinationPath, QString *errorString)
{
    if (errorString)
        errorString->clear();

    ArchiveReader reader;
    if (!reader.open(archivePath, errorString))
        return false;

    bool entriesOk = false;
    const QList<ArchiveEntry> entries = readEntries(&reader, &entriesOk, errorString);
    if (!entriesOk)
        return false;
    QSet<QString> outputPaths;
    for (const ArchiveEntry &entry : entries) {
        if (!isSafeRelativePath(entry.fileName)) {
            setError(errorString, QStringLiteral("ZIP条目路径不安全: %1").arg(entry.fileName));
            return false;
        }
        if (!entry.isDirectory && (entry.isEncrypted || !entry.isSupported)) {
            setError(errorString, QStringLiteral("ZIP条目不可解压: %1").arg(entry.fileName));
            return false;
        }

        const QString cleanPath = QDir::cleanPath(entry.fileName);
#ifdef Q_OS_WIN
        const QString pathKey = cleanPath.toCaseFolded();
#else
        const QString pathKey = cleanPath;
#endif
        if (outputPaths.contains(pathKey)) {
            setError(errorString, QStringLiteral("ZIP中存在重复路径: %1").arg(entry.fileName));
            return false;
        }
        outputPaths.insert(pathKey);
    }

    if (!QDir().mkpath(destinationPath)) {
        setError(errorString, QStringLiteral("创建解压目录失败: %1").arg(destinationPath));
        return false;
    }

    const QDir destination(destinationPath);
    for (const ArchiveEntry &entry : entries) {
        const QString outputPath = destination.filePath(QDir::cleanPath(entry.fileName));
        if (entry.isDirectory) {
            if (!QDir().mkpath(outputPath)) {
                setError(errorString, QStringLiteral("创建解压子目录失败: %1").arg(outputPath));
                return false;
            }
            continue;
        }
        if (!extractEntry(&reader, entry, outputPath, errorString))
            return false;
    }
    return true;
}

} // namespace MinizQt
