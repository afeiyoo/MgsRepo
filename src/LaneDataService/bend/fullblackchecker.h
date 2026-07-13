#pragma once

#include "utils/optional.h"
#include <QObject>

namespace EasyQtSql {
class SqlFactory;
}

class FullBlackChecker : public QObject
{
    Q_OBJECT
public:
    explicit FullBlackChecker(QObject *parent = nullptr);
    ~FullBlackChecker() override;

    void init();

public slots:
    // 检查全量
    void onCheckFullBlack();

private:
    // 获取最新全量版本
    Utils::optional<int> getMaxBatchNoFromFiles();
    // 加载新全量文件
    bool loadNewFile(const QString &filePath);
    // 清理过期全量文件
    void pruneFiles(const QString &excludeFile);

private:
    QThread *m_td = nullptr;
};
