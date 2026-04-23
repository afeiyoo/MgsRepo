#pragma once

#include "hostosinfo.h"

#include <QCoreApplication>
#include <QMetaType>
#include <QStringList>
#include <QXmlStreamWriter> // Mac.

#include <functional>
#include <memory>

namespace Utils {
class FileName;
}

QT_BEGIN_NAMESPACE
class QDataStream;
class QDateTime;
class QDir;
class QFile;
class QFileInfo;
class QTemporaryFile;
class QTextStream;
class QWidget;

QDebug operator<<(QDebug dbg, const Utils::FileName &c);

// for withNtfsPermissions
#ifdef Q_OS_WIN
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif

QT_END_NAMESPACE

namespace Utils {

class FileName : private QString
{
public:
    FileName();
    explicit FileName(const QFileInfo &info);
    // 根据文件路径构造 FileName 对象。
    static FileName fromString(const QString &filename);
    // 根据文件路径构造 FileName 对象，如果文件名没有扩展名，则追加扩展名defaultExtension。
    static FileName fromString(const QString &filename, const QString &defaultExtension);
    // 根据文件路径（Latin1 编码的字节流）构造 FileName 对象。
    static FileName fromLatin1(const QByteArray &filename);
    // 根据文件路径（由用户输入的路径，可能不规范。支持~目录）构造 FileName 对象
    static FileName fromUserInput(const QString &filename);
    // 根据文件路径（UTF8 编码的字节流）构造 FileName 对象。
    static FileName fromUtf8(const char *filename, int filenameSize = -1);
    // 将 FileName 对象转为 QFileInfo 对象
    QFileInfo toFileInfo() const;
    // 将 FileName 对象转为 QString 对象
    const QString &toString() const;
    // 返回用户可读路径（如 Windows 下变成反斜杠分隔符）
    QString toUserOutput() const;
    // 返回单一文件名。pathComponents参数可以指定返回路径的层级
    // 例如：路径 = "/home/user/projects/src/file.cpp"
    // | `pathComponents` | 返回值                              | 说明            |
    // | ---------------- | ---------------------------------- | ------------- |
    // | -1               | `/home/user/projects/src/file.cpp` | 小于 0，返回完整路径   |
    // | 0                | `file.cpp`                         | 最后 1 个组件（文件名） |
    // | 1                | `src/file.cpp`                     | 最后 2 个组件      |
    // | 2                | `projects/src/file.cpp`            | 最后 3 个组件      |
    // | 10               | `/home/user/projects/src/file.cpp` | 超过总组件数，返回全路径  |
    QString fileName(int pathComponents = 0) const;
    // 判断该路径对应的文件是否存在
    bool exists() const;
    // 返回父目录（等价于 ../），若为根目录则返回空。
    FileName parentDir() const;

    // 比较两个路径字符串。所有比较都根据 HostOsInfo::fileNameCaseSensitivity() 判断大小写是否敏感
    bool operator==(const FileName &other) const;
    bool operator!=(const FileName &other) const;
    bool operator<(const FileName &other) const;
    bool operator<=(const FileName &other) const;
    bool operator>(const FileName &other) const;
    bool operator>=(const FileName &other) const;

    // 拼接字符串生成新的 FileName。
    FileName operator+(const QString &s) const;

    // 判断当前路径是否是 s 的子路径。
    bool isChildOf(const FileName &s) const;
    // 判断当前路径是否是文件夹 dir 的子路径
    bool isChildOf(const QDir &dir) const;

    // 判断路径是否以字符串 s 结尾
    bool endsWith(const QString &s) const;

    // 如果当前路径是 parent 的子路径，则返回基于路径 parent 的相对路径。否则返回空。
    FileName relativeChildPath(const FileName &parent) const;
    // 安全拼接子路径，自动补 /
    FileName &appendPath(const QString &s);
    // 直接拼接字符串或字符到路径
    FileName &appendString(const QString &str);
    FileName &appendString(QChar str);

    using QString::chop;
    using QString::clear;
    using QString::count;
    using QString::isEmpty;
    using QString::isNull;
    using QString::length;
    using QString::size;

private:
    FileName(const QString &string);
};

// 运算符重载，输出 FileName 对象的实际路径
QTextStream &operator<<(QTextStream &s, const FileName &fn);

using FileNameList = QList<FileName>;

class FileUtils
{
public:
    // 递归删除文件或文件夹，并返回操作是否成功。参数 error 可选，用于保存错误信息
    static bool removeRecursively(const FileName &filePath, QString *error = nullptr);
    // 将指定目录 srcFilePath 下的文件或文件夹递归复制到目标目录 tgtFilePath 下。
    // 参数 error 可选，用于保存错误信息。可以通过 copyHelper 指定自定义的复制行为。
    static bool copyRecursively(const FileName &srcFilePath, const FileName &tgtFilePath, QString *error = nullptr,
                                const std::function<bool(QFileInfo, QFileInfo, QString *)> &copyHelper = nullptr);
    // 判断某个文件或目录中的任意文件是否比指定时间戳更新，是则返回 true，否则返回 false
    static bool isFileNewerThan(const FileName &filePath, const QDateTime &timeStamp);
    // 如果路径 path 是一个符号链接（symlink），就追踪它指向的目标路径。
    // 如果目标路径本身又是一个符号链接，就继续递归地解析，直到：找到最终的真实文件/目录；或者解析深度超过 16 层（防止无限循环）
    static FileName resolveSymlinks(const FileName &path);
    // 解析路径 path 中的每一部分（文件夹或文件）的符号链接，同时返回规范化路径。
    // 如果路径 path 不存在，则返回原路径 path。
    // 例如：/home/myuser/../user/docs/./file.txt，其中/myuser是/user的符号链接，则解析结果为/home/user/docs/file.txt
    static FileName canonicalPath(const FileName &path);
    // 将路径转为本地平台格式（如 Windows 用 \），并将 home 路径简写为 ~
    static QString shortNativePath(const FileName &path);
    // 将字符串转换为文件系统友好的格式，只保留字母数字、下划线等。
    static QString fileSystemFriendlyName(const QString &name);
    // 查找第一个对 qmake 不友好的字符（非字母数字、下划线、点、短横线），并返回索引
    static int indexOfQmakeUnfriendly(const QString &name, int startpos = 0);
    // 将字符串转换为对 qmake 友好的名字，替换非法字符为 _
    static QString qmakeFriendlyName(const QString &name);
    // 修改文件权限为用户可写
    static bool makeWritable(const FileName &path);
    // 返回系统上大小写规范化后的路径（仅 Windows / macOS 有意义）
    static QString normalizePathName(const QString &name);

    // 判断路径 fileName 是否是相对路径。
    static bool isRelativePath(const QString &fileName);
    // 判断路径 fileName 是否是绝对路径
    static bool isAbsolutePath(const QString &fileName) { return !isRelativePath(fileName); }
    // 基于路径 baseDir，将相对路径解析为绝对路径，并返回
    static QString resolvePath(const QString &baseDir, const QString &fileName);
    // 返回两个路径的最长公共父路径。
    static FileName commonPath(const FileName &oldCommonPath, const FileName &fileName);
    // 检查目录是否存在，不存在则创建，存在则返回
    static void makeSureDirExist(const FileName &dirPath);
    // 获取目录下相关的后缀的文件
    static FileNameList getFilesWithSuffix(const FileName &dirPath, const QString &suffix);
    // 自动清除指定路径下的超期文件（以后缀进行匹配）
    static void autoDeleteFiles(const QString &path, const QString &fileExtension, qint64 expiredHour, QString *lastError);
    // 获取当前可执行程序所在目录
    static QString curApplicationDirPath();
};

// for actually finding out if e.g. directories are writable on Windows
#ifdef Q_OS_WIN

template<typename T>
T withNtfsPermissions(const std::function<T()> &task)
{
    qt_ntfs_permission_lookup++;
    T result = task();
    qt_ntfs_permission_lookup--;
    return result;
}

template<>
void withNtfsPermissions(const std::function<void()> &task);

#else // Q_OS_WIN

template<typename T>
T withNtfsPermissions(const std::function<T()> &task)
{
    return task();
}

#endif // Q_OS_WIN

class FileReader
{
    Q_DECLARE_TR_FUNCTIONS(Utils::FileUtils) // sic!
public:
    // 读取 Qt 资源文件，失败时返回空数据
    static QByteArray fetchQrc(const QString &fileName); // Only for internal resources
    // 读取文件（可传入 mode，支持 QIODevice::ReadOnly 与 QIODevice::Text）
    bool fetch(const QString &fileName,
               QIODevice::OpenMode mode = QIODevice::NotOpen); // QIODevice::ReadOnly is implicit
    // 读取文件（可返回读取错误信息）
    bool fetch(const QString &fileName, QIODevice::OpenMode mode, QString *errorString);
    // 读取文件，以 ReadOnly 打开
    bool fetch(const QString &fileName, QString *errorString) { return fetch(fileName, QIODevice::NotOpen, errorString); }
#ifdef QT_GUI_LIB
    // 读取文件（可传入 mode，支持 QIODevice::ReadOnly 与 QIODevice::Text），错误时弹窗提示
    bool fetch(const QString &fileName, QIODevice::OpenMode mode, QWidget *parent);
    // 读取文件，以 ReadOnly 打开，错误时弹窗提示
    bool fetch(const QString &fileName, QWidget *parent) { return fetch(fileName, QIODevice::NotOpen, parent); }
#endif // QT_GUI_LIB
    // 返回读取到的文件内容
    const QByteArray &data() const { return m_data; }
    // 返回最近一次的错误信息
    const QString &errorString() const { return m_errorString; }

private:
    // 读取到的原始数据
    QByteArray m_data;
    // 读取过程中的错误信息
    QString m_errorString;
};

class FileSaverBase
{
    Q_DECLARE_TR_FUNCTIONS(Utils::FileUtils) // sic!
public:
    FileSaverBase();
    virtual ~FileSaverBase();

    QString fileName() const { return m_fileName; }
    bool hasError() const { return m_hasError; }
    QString errorString() const { return m_errorString; }

    // 收尾函数，用于关闭文件，释放资源
    virtual bool finalize();
    bool finalize(QString *errStr);
#ifdef QT_GUI_LIB
    bool finalize(QWidget *parent);
#endif
    // 写入数据到文件对象中。并设置写入
    bool write(const char *data, int len);
    bool write(const QByteArray &bytes);

    // 检查写入是否成功，并设置 m_hasError 标记与错误信息
    bool setResult(QTextStream *stream);
    bool setResult(QDataStream *stream);
    bool setResult(QXmlStreamWriter *stream);
    bool setResult(bool ok);

    // 提供对底层 QFile 对象的访问接口，便于用户手动构造 QTextStream, QXmlStreamWriter 等写入接口
    QFile *file() { return m_file.get(); }

protected:
    // 指向当前操作的文件对象
    std::unique_ptr<QFile> m_file;
    // 当前操作文件的完整路径名
    QString m_fileName;
    // 最近一次的错误信息
    QString m_errorString;
    // 标记是否保存失败
    bool m_hasError = false;

private:
    Q_DISABLE_COPY(FileSaverBase)
};

class FileSaver : public FileSaverBase
{
    Q_DECLARE_TR_FUNCTIONS(Utils::FileUtils) // sic!
public:
    // 构造函数根据是否使用安全写入逻辑，选择创建合适的文件对象，并尝试打开文件，为写入操作做好准备。
    // 默认文件打开方式是 WriteOnly
    explicit FileSaver(const QString &filename, QIODevice::OpenMode mode = QIODevice::NotOpen);
    // 完成写入工作，关闭文件并提交（或回滚）数据，释放资源
    bool finalize() override;
    using FileSaverBase::finalize;

private:
    // 是否支持数据安全写入
    bool m_isSafe;
};

class TempFileSaver : public FileSaverBase
{
    Q_DECLARE_TR_FUNCTIONS(Utils::FileUtils) // sic!
public:
    explicit TempFileSaver(const QString &templ = QString());
    ~TempFileSaver() override;

    void setAutoRemove(bool on) { m_autoRemove = on; }

private:
    // 是否自动删除临时文件
    bool m_autoRemove = true;
};

} // namespace Utils

QT_BEGIN_NAMESPACE
uint qHash(const Utils::FileName &a);
QT_END_NAMESPACE

namespace std {
template<>
struct hash<Utils::FileName>
{
    using argument_type = Utils::FileName;
    using result_type = size_t;
    result_type operator()(const argument_type &fn) const
    {
        if (Utils::HostOsInfo::fileNameCaseSensitivity() == Qt::CaseInsensitive)
            return hash<string>()(fn.toString().toUpper().toStdString());
        return hash<string>()(fn.toString().toStdString());
    }
};
} // namespace std

Q_DECLARE_METATYPE(Utils::FileName)
