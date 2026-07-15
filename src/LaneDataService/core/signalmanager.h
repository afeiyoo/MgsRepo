#pragma once

#include <QObject>

class SignalManager : public QObject
{
    Q_OBJECT
public:
    explicit SignalManager(QObject *parent = nullptr);

signals:
    // 启动全量更新检查
    void sigCheckFullBlack(bool isFirst, bool curFullBlackStatus);
    // 加载全量文件
    void sigLoadFullBlack(const QString &path, int batchNo);
    // 全量文件加载结果
    void sigLoadFullBlackRes(bool ok, int batchNo, const QString &version);
    // 更新全量状态
    void sigUpdateFullBlackStatus(bool status, const QString &desc);
    // 更新全量版本
    void sigUpdateFullBlackVersion(const QString &version);
};
