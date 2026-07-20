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
    // 更新当前增量状态
    void setStatus(bool isValid, int status);

    // 请求获取增量数据
    QByteArray getDeltaBlackJson();

    bool saveDeltaBlackJson(const QByteArray &data);

private:
    // 当前增量是否正常 true:增量正常 false:增量异常
    bool m_isValid = false;
    // 0: 增量获取成功 => 增量正常
    // -1: 增量版本号和全量版本号都空，无法获取增量数据 => 取决上一次的增量状态
    // -2: 向站级服务请求获取增量数据失败（网络原因） => 取决与上一次的增量状态
    // -3: 向站级服务请求获取增量数据，但内容异常 => 取决于上一次的增量状态
    // -4: 增量状态查询结果异常 => 取决于上一次的增量状态
    int m_curStatus = -1;
    // 增量版本
    QString m_version;

    QSqlDatabase m_db;

    QTimer *m_timer = nullptr;
    Http *m_client = nullptr;
};
