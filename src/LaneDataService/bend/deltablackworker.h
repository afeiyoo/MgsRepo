#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QTimer>

class Http;

class DeltaBlackWorker : public QObject
{
    Q_OBJECT
public:
    explicit DeltaBlackWorker(QObject *parent = nullptr);
    ~DeltaBlackWorker() override;

public slots:
    void onInit();
    void onCheckDeltaBlack();

private:
    bool batchUpsertDeltaBlack(int operateTable, const QVariantList &blackDetails);

    // 更新当前增量状态
    void setStatus(bool isValid, int status);

    // 请求获取增量数据
    QByteArray getDeltaBlackJson();

    // 保存增量数据
    bool saveDeltaBlackJson(const QByteArray &data);

    // 获取增量版本号
    QString getDeltaBlackVersion();
    // 更新增量版本号
    int updateDeltaBlackVersion(const QString &ver);

private:
    // 当前增量是否正常 true:增量正常 false:增量异常
    bool m_isValid = false;
    // 0: 增量获取成功 => 增量正常
    // -1: 增量SQLite文件不存在 => 增量异常
    // -2: 初始化增量SQLite数据库失败 => 增量异常
    // -3: 增量版本号和全量版本号都空，无法获取增量数据 => 取决上一次的增量状态
    // -4: 向站级服务请求获取增量数据失败（网络原因） => 取决与上一次的增量状态
    // -5: 向站级服务请求获取增量数据，但内容异常 => 取决于上一次的增量状态
    // -6: 站级服务返回无数据、参数错误或系统异常 => 取决于上一次的增量状态
    // -7: 增量数据插入SQLite数据库失败 => 取决于上一次的增量状态
    // -8: 增量版本号更新失败 => 取决上一次的增量状态
    int m_curStatus = -1;
    // 增量版本
    QString m_version;

    QSqlDatabase m_dao;

    QTimer *m_timer = nullptr;
    Http *m_client = nullptr;
};
