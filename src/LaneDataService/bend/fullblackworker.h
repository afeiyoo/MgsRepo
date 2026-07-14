#pragma once

#include "utils/optional.h"
#include <QObject>

class FullBlackWorker : public QObject
{
    Q_OBJECT
public:
    explicit FullBlackWorker(QObject *parent = nullptr);
    ~FullBlackWorker() override;

public slots:
    void onCheckNewFullBlack();
    void onCheckCurFullBlack();
    void onLoadFullBlackRes(bool ok, int batchNo);

private:
    // 获取全量目录下的全量最大版本号
    Utils::optional<int> getMaxBatchNoFromFiles(const QString &fullBlackPath) const;

    // 同一时间只允许加载一个全量文件；用于过滤重复检查和过期的加载结果
    bool m_loading = false;
    int m_pendingBatchNo = 0;
};
