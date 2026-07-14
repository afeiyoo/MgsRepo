#pragma once

#include <QObject>

class SignalManager : public QObject
{
    Q_OBJECT
public:
    explicit SignalManager(QObject *parent = nullptr);

signals:
    // 启动全量更新检查
    void sigCheckNewFullBlack();
    // 加载当前版本全量
    void sigCheckCurFullBlack();
    // 加载全量文件
    void sigLoadFullBlack(const QString &path, int batchNo);
    // 全量文件加载结果
    void sigLoadFullBlackRes(bool ok, int batchNo);
    // 更新全量状态
    void sigUpdateFullBlackStatus(int status);
};
