#include "savefile.h"
#include "fileutils.h"
#include "qtcassert.h"
#ifdef Q_OS_WIN
#include <io.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace Utils {

QFile::Permissions SaveFile::m_umask = nullptr;

SaveFile::SaveFile(const QString &filename)
    : m_finalFileName(filename)
{}

SaveFile::~SaveFile()
{
    // 如果对象销毁前，没有commit()成功，则调用rollback()，清除临时文件，防止残留
    QTC_ASSERT(m_finalized, rollback());
}

bool SaveFile::open(OpenMode flags)
{
    QTC_ASSERT(!m_finalFileName.isEmpty(), return false);

    // 如果目标文件存在，但无法以读写方式打开，则说明当前程序没有写权限，或文件被锁住了
    // 不能继续写，返回失败
    QFile ofi(m_finalFileName);
    if (ofi.exists() && !ofi.open(QIODevice::ReadWrite)) {
        setErrorString(ofi.errorString());
        return false;
    }

    // 创建临时文件并打开
    m_tempFile.reset(new QTemporaryFile(m_finalFileName));
    m_tempFile->setAutoRemove(false);
    if (!m_tempFile->open())
        return false;
    setFileName(m_tempFile->fileName());

    if (!QFile::open(flags))
        return false;

    m_finalized = false; // 标记当前文件未提交，在析构时需要清理

    // 设置临时文件权限
    if (ofi.exists()) {
        setPermissions(ofi.permissions()); // Ignore errors
    } else {
        Permissions permAll = QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther
                              | QFile::WriteOwner | QFile::WriteGroup | QFile::WriteOther;

        // set permissions with respect to the current umask
        setPermissions(permAll & ~m_umask);
    }

    return true;
}

void SaveFile::rollback()
{
    close();
    if (m_tempFile)
        m_tempFile->remove();
    m_finalized = true;
}

bool SaveFile::commit()
{
    QTC_ASSERT(!m_finalized && m_tempFile, return false;);
    m_finalized = true;

    if (!flush()) {
        close();
        m_tempFile->remove();
        return false;
    }
#ifdef Q_OS_WIN
    FlushFileBuffers(reinterpret_cast<HANDLE>(_get_osfhandle(handle())));
#elif _POSIX_SYNCHRONIZED_IO > 0
    fdatasync(handle());
#else
    fsync(handle());
#endif
    close();
    m_tempFile->close();
    if (error() != NoError) {
        m_tempFile->remove();
        return false;
    }

    QString finalFileName = FileUtils::resolveSymlinks(FileName::fromString(m_finalFileName))
                                .toString();

#ifdef Q_OS_WIN
    // Release the file lock
    m_tempFile.reset();
    // 原子地将临时文件替换为目标文件
    bool result = ReplaceFile(finalFileName.toStdWString().data(),
                              fileName().toStdWString().data(),
                              nullptr,
                              REPLACEFILE_IGNORE_MERGE_ERRORS,
                              nullptr,
                              nullptr);
    if (!result) {
        DWORD replaceErrorCode = GetLastError();
        QString errorStr;
        if (!QFile::exists(finalFileName)) {
            // Replace failed because finalFileName does not exist, try rename.
            if (!(result = rename(finalFileName)))
                errorStr = errorString();
        } else {
            if (replaceErrorCode == ERROR_UNABLE_TO_REMOVE_REPLACED) {
                // If we do not get the rights to remove the original final file we still might try
                // to replace the file contents
                result = MoveFileEx(fileName().toStdWString().data(),
                                    finalFileName.toStdWString().data(),
                                    MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING
                                        | MOVEFILE_WRITE_THROUGH);
                if (!result)
                    replaceErrorCode = GetLastError();
            }
            if (!result) {
                wchar_t messageBuffer[256];
                FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                               nullptr,
                               replaceErrorCode,
                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               messageBuffer,
                               sizeof(messageBuffer),
                               nullptr);
                errorStr = QString::fromWCharArray(messageBuffer);
            }
        }
        if (!result) {
            remove();
            setErrorString(errorStr);
        }
    }

    return result;
#else
    const QString backupName = finalFileName + '~';

    // Back up current file.
    // If it's opened by another application, the lock follows the move.
    if (QFile::exists(finalFileName)) {
        // Kill old backup. Might be useful if creator crashed before removing backup.
        QFile::remove(backupName);
        QFile finalFile(finalFileName);
        if (!finalFile.rename(backupName)) {
            m_tempFile->remove();
            setErrorString(finalFile.errorString());
            return false;
        }
    }

    bool result = true;
    if (!m_tempFile->rename(finalFileName)) {
        // The case when someone else was able to create finalFileName after we've renamed it.
        // Higher level call may try to save this file again but here we do nothing and
        // return false while keeping the error string from last rename call.
        const QString &renameError = m_tempFile->errorString();
        m_tempFile->remove();
        setErrorString(renameError);
        QFile::rename(backupName, finalFileName); // rollback to backup if possible ...
        return false;                             // ... or keep the backup copy at least
    }

    QFile::remove(backupName);

    return result;
#endif
}

void SaveFile::initializeUmask()
{
#ifdef Q_OS_WIN
    m_umask = QFile::WriteGroup | QFile::WriteOther;
#else
    // Get the current process' file creation mask (umask)
    // umask() is not thread safe so this has to be done by single threaded
    // application initialization
    mode_t mask = umask(0); // get current umask
    umask(mask);            // set it back

    m_umask = ((mask & S_IRUSR) ? QFile::ReadOwner : QFlags<QFile::Permission>(0))
              | ((mask & S_IWUSR) ? QFile::WriteOwner : QFlags<QFile::Permission>(0))
              | ((mask & S_IXUSR) ? QFile::ExeOwner : QFlags<QFile::Permission>(0))
              | ((mask & S_IRGRP) ? QFile::ReadGroup : QFlags<QFile::Permission>(0))
              | ((mask & S_IWGRP) ? QFile::WriteGroup : QFlags<QFile::Permission>(0))
              | ((mask & S_IXGRP) ? QFile::ExeGroup : QFlags<QFile::Permission>(0))
              | ((mask & S_IROTH) ? QFile::ReadOther : QFlags<QFile::Permission>(0))
              | ((mask & S_IWOTH) ? QFile::WriteOther : QFlags<QFile::Permission>(0))
              | ((mask & S_IXOTH) ? QFile::ExeOther : QFlags<QFile::Permission>(0));
#endif
}

} // namespace Utils
