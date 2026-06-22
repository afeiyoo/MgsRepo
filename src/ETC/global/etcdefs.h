#pragma once
#include <QObject>

//枚举类导出  兼容QT5低版本
#define E_BEGIN_ENUM_CREATE(CLASS) \
    class CLASS : public QObject \
    { \
        Q_OBJECT \
    public:

#define E_END_ENUM_CREATE(CLASS) \
private: \
    Q_DISABLE_COPY(CLASS) \
    } \
    ;

#define E_ENUM_CREATE(CLASS) Q_ENUM(CLASS)

struct ST_TradeInfo
{
    QString tradeID;  // 交易号
    QString vehPlate; // 车牌
    uint vehClass;    // 车型
    qint64 updateTime;
};
