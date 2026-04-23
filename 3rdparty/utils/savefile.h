#pragma once

#include <QTemporaryFile>

#include <memory>

namespace Utils {

class SaveFile : public QFile
{
    Q_OBJECT

public:
    explicit SaveFile(const QString &filename);
    ~SaveFile() override;

    // 重写 QFile::open()。创建并打开一个临时文件用于写入，设置好权限，准备保存
    bool open(OpenMode flags = QIODevice::WriteOnly) override;

    // 如果用户放弃保存，或发生错误，关闭并删除临时文件，并清理状态
    void rollback();
    // 将写入临时文件的内容提交为最终的目标文件
    bool commit();

    // 读取当前进程的 umask（用户文件创建权限掩码） 设置并保存，以便新文件能正确设置权限
    static void initializeUmask();

private:
    // 保存目标文件名
    const QString m_finalFileName;
    // 临时文件，数据写入的实际目标
    std::unique_ptr<QTemporaryFile> m_tempFile;
    // 是否已提交/回滚，防止重复操作
    bool m_finalized = true;
    // 当前文件权限
    static QFile::Permissions m_umask;
};

} // namespace Utils
