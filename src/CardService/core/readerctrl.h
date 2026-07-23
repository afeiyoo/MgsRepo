#pragma once

#include <QMutex>
#include <QObject>

#include "def/defines.h"
#include "helper/ILaneCardCtrl.h"

class ICardCtrl32;
class ReaderCtrl : public QObject
{
    Q_OBJECT
public:
    explicit ReaderCtrl(QObject *parent = nullptr);
    ~ReaderCtrl() override;

    // 初始化读卡器
    bool initReader();
    // 读PSAM卡
    bool readPsam(ST_PsamReadResult &result);
    // 关闭读卡器
    void closeReader();
    // PSAM卡授权
    int authPsam(const QString &cosCmd, QString &hexReply);
    // 身份卡验证
    int verifyCard(const QString &cardNum, const QString &password, int dealStatus);
    // 读卡信息
    bool readCard(TReadCardInfoResult &result);

private:
    bool isApiConfigMatched(const ST_ConfigSnap &snap) const;
    bool openReader(const ST_ConfigSnap &snap);
    bool readPsamInfo(QByteArray &termNo, QByteArray &psamNo, uchar &needAuth, QByteArray &random);
    QString getCPUCardNum();
    int readCardInfo(TReadCardInfoResult &cardReadResult, bool isEntranceLane);

private:
    QMutex m_mutex;
    // 读卡器是否已打开
    bool m_isReaderOpened = false;
    // 读卡器操作对象
    ICardCtrl32 *m_readerCtrlAPI = nullptr;
    // 当前QXCardCtrl实例创建时使用的动态库配置
    QString m_apiName;
    int m_apiType = -1;
    // 读卡器实际打开时使用的PSAM卡槽
    int m_slotNo = 0;
};
