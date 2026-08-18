#include "readerctrl.h"

#include "Logger.h"
#include "config/config.h"
#include "core/globalmanager.h"
#include "def/defines.h"
#include "helper/CCardTransform.h"
#include "mgslibs/CXCardCtrlDef.h"
#include "utils/datadealutils.h"

using namespace Utils;

ReaderCtrl::ReaderCtrl(QObject *parent)
    : QObject{parent}
{}

ReaderCtrl::~ReaderCtrl()
{
    if (m_readerCtrlAPI && m_isReaderOpened) {
        m_readerCtrlAPI->CloseReader();
        m_isReaderOpened = false;
    }

    m_readerCtrlAPI = nullptr;
}

bool ReaderCtrl::initReader()
{
    QMutexLocker locker(&m_mutex);
    ST_ConfigSnap snap = GM_INS->m_config->getSnap();
    LOG_INFO().noquote() << "读卡器初始化 apiType:" << snap.apiType << "apiName:" << snap.apiName << "comPort:" << snap.comPort
                         << "slotNo:" << snap.slotNo << "laneType:" << snap.laneType;

    return openReader(snap);
}

bool ReaderCtrl::isApiConfigMatched(const ST_ConfigSnap &snap) const
{
    return !m_readerCtrlAPI || (m_apiName == snap.apiName.trimmed() && m_apiType == snap.apiType);
}

bool ReaderCtrl::openReader(const ST_ConfigSnap &snap)
{
    const QString apiName = snap.apiName.trimmed();
    const QString comPort = snap.comPort.trimmed();
    QByteArray apiNameBytes = apiName.toLocal8Bit();
    QByteArray comPortBytes = comPort.toLocal8Bit();

    if (apiName.isEmpty()) {
        LOG_ERROR().noquote() << "读卡器初始化失败: apiName为空";
        return false;
    }
    if (comPort.isEmpty()) {
        LOG_ERROR().noquote() << "读卡器初始化失败: comPort错误";
        return false;
    }
    if (snap.slotNo < 1 || snap.slotNo > 4) {
        LOG_ERROR().noquote() << "读卡器初始化失败: slotNo必须在1~4之间，当前值" << snap.slotNo;
        return false;
    }

    if (!isApiConfigMatched(snap)) {
        LOG_ERROR().noquote() << "读卡器初始化失败: 运行期不支持切换apiName或apiType，请重启服务"
                              << "当前apiName" << m_apiName << "当前apiType" << m_apiType << "新apiName" << apiName << "新apiType" << snap.apiType;
        return false;
    }

    if (!m_readerCtrlAPI) {
        m_readerCtrlAPI = GetCardCtrl32Ex(apiNameBytes.data(), snap.apiType);
        if (!m_readerCtrlAPI) {
            LOG_ERROR().noquote() << "读卡器初始化失败: 创建QXCardCtrl实例失败";
            return false;
        }
        m_apiName = apiName;
        m_apiType = snap.apiType;
    }

    if (m_isReaderOpened) {
        m_readerCtrlAPI->CloseReader();
        m_isReaderOpened = false;
    }

    int res = m_readerCtrlAPI->OpenReader(comPortBytes.data(), snap.slotNo);
    if (res != 0) {
        LOG_ERROR().noquote() << QString("读卡器初始化失败: 打开读卡器返回错误(状态 %1)").arg(res);
        m_isReaderOpened = false;
        return false;
    }

    LOG_INFO().noquote() << QString("读卡器初始成功(状态 %1)").arg(res);
    m_isReaderOpened = true;
    m_slotNo = snap.slotNo;
    return true;
}

bool ReaderCtrl::readPsam(ST_PsamReadResult &result)
{
    QMutexLocker locker(&m_mutex);
    LOG_INFO().noquote() << "读取PSAM卡信息...";

    // 清空ST_PsamReadResult
    result = ST_PsamReadResult{};

    const ST_ConfigSnap snap = GM_INS->m_config->getSnap();
    if (!isApiConfigMatched(snap)) {
        LOG_ERROR().noquote() << "读取PSAM卡信息失败: apiName或apiType已变更，请重启服务";
        result.errorCode = 4;
        return false;
    }

    if (!m_isReaderOpened) {
        LOG_INFO().noquote() << "读卡器未打开，尝试重新打开读卡器";
        if (!openReader(snap)) {
            result.errorCode = 4;
            return false;
        }
    }

    // 读取PSAM卡信息
    if (!readPsamInfo(result.termNo, result.psamNo, result.isNeedAuth, result.random)) {
        result.errorCode = 1;
        return false;
    }

    result.errorCode = 0;
    return true;
}

void ReaderCtrl::closeReader()
{
    QMutexLocker locker(&m_mutex);
    LOG_INFO().noquote() << "关闭读卡器...";
    if (m_isReaderOpened)
        m_readerCtrlAPI->CloseReader();
    m_isReaderOpened = false;
}

int ReaderCtrl::authPsam(const QString &cosCmd, QString &hexReply)
{
    QMutexLocker locker(&m_mutex);
    LOG_INFO().noquote() << "PSAM卡授权...";

    hexReply.clear();

    const ST_ConfigSnap snap = GM_INS->m_config->getSnap();
    if (!isApiConfigMatched(snap)) {
        LOG_ERROR().noquote() << "PSAM卡授权失败: apiName或apiType已变更，请重启服务";
        return 1;
    }

    if (!m_isReaderOpened) {
        LOG_INFO().noquote() << "读卡器未打开，尝试重新打开读卡器";
        if (!openReader(snap)) {
            return 1;
        }
    }

    // 开始进行PSAM卡授权
    QByteArray cosCmdBytes = cosCmd.toUpper().toLocal8Bit();
    char buffer[256] = {};
    int authRes = m_readerCtrlAPI->PsamExternalVerify(m_slotNo, cosCmdBytes.data(), buffer);
    hexReply = DataDealUtils::bufferToByteArray(buffer, sizeof(buffer));
    if (authRes != 0) {
        LOG_ERROR().noquote() << "PSAM卡授权失败，错误码" << authRes << "HexReply" << hexReply;
        return 1;
    }
    return 0;
}

int ReaderCtrl::verifyCard(const QString &cardNum, const QString &password, int dealStatus)
{
    QMutexLocker locker(&m_mutex);
    LOG_INFO().noquote() << "身份卡验证...";

    LOG_INFO().noquote() << "身份卡验证参数: cardNum" << cardNum << "password" << password << "dealStatus" << dealStatus;
    const ST_ConfigSnap snap = GM_INS->m_config->getSnap();
    if (!isApiConfigMatched(snap)) {
        LOG_ERROR().noquote() << "身份卡验证失败: apiName或apiType已变更，请重启服务";
        return -105;
    }

    if (!m_isReaderOpened) {
        LOG_INFO().noquote() << "读卡器未打开，尝试重新打开读卡器";
        if (!openReader(snap)) {
            return -106;
        }
    }

    int result = 0;
    int cardType = m_readerCtrlAPI->OpenCardEx();
    LOG_INFO().noquote() << "获取卡类型返回:" << cardType;
    if (cardType == -1) {
        LOG_ERROR().noquote() << "当前无卡";
        result = -101;
    } else if (cardType == -2) {
        LOG_ERROR().noquote() << "当前坏卡";
        result = -102;
    } else if (cardType == 0 || cardType == 1 || cardType == 4) {
        LOG_ERROR().noquote() << "非身份卡";
        result = -103;
    } else if (cardType == 2) {
        QString curCardNum = getCPUCardNum();
        if (curCardNum != cardNum) {
            LOG_ERROR().noquote() << QString("当前身份卡号 %1 与远程辅助发送的卡号不一致,无法验证").arg(curCardNum);
            result = -100;
        } else {
            result = m_readerCtrlAPI->CPUVerify(password.toLocal8Bit().data(), dealStatus);
            LOG_INFO().noquote() << "身份卡验证结果返回:" << result;
        }
    } else {
        LOG_INFO().noquote() << "获取卡类型时发生错误";
        result = -104;
    }

    m_readerCtrlAPI->CloseCard();
    return result;
}

bool ReaderCtrl::readCard(TReadCardInfoResult &result)
{
    QMutexLocker locker(&m_mutex);
    LOG_INFO().noquote() << "读卡信息...";

    result = TReadCardInfoResult{};

    const ST_ConfigSnap snap = GM_INS->m_config->getSnap();
    if (!isApiConfigMatched(snap)) {
        LOG_ERROR().noquote() << "读卡信息失败: apiName或apiType已变更，请重启服务";
        result.ErrorCode = 4;
        return false;
    }

    if (!m_isReaderOpened) {
        LOG_INFO().noquote() << "读卡器未打开，尝试重新打开读卡器";
        if (!openReader(snap)) {
            result.ErrorCode = 4;
            return false;
        }
    }

    int readRes = readCardInfo(result, snap.laneType == 1);
    if (readRes != 0) {
        result.ErrorCode = readRes == -1 ? 3 : 1;
        LOG_ERROR().noquote() << "读取卡信息失败，错误类型" << readRes << "ErrorCode" << result.ErrorCode;
        return false;
    }

    return true;
}

bool ReaderCtrl::readPsamInfo(QByteArray &termNo, QByteArray &psamNo, uchar &needAuth, QByteArray &random)
{
    termNo.clear();
    psamNo.clear();
    random.clear();
    needAuth = 0;

    char buffer[32] = {};
    int ret = m_readerCtrlAPI->FetchTermNo(buffer);
    if (ret != 0) {
        LOG_ERROR().noquote() << "获取终端机编号失败，卡槽" << m_slotNo << "错误码" << ret;
        return false;
    }
    termNo = DataDealUtils::bufferToByteArray(buffer, sizeof(buffer));

    memset(buffer, 0, sizeof(buffer));
    ret = m_readerCtrlAPI->FetchPsamNo(buffer);
    if (ret != 0) {
        LOG_ERROR().noquote() << "获取PSAM卡号失败 错误码" << ret;
        return false;
    }
    psamNo = DataDealUtils::bufferToByteArray(buffer, sizeof(buffer));

    LOG_INFO().noquote() << "在卡槽" << m_slotNo << "检测到PSAM卡，并读取到PSAM卡信息: termNo" << termNo << "psamNo" << psamNo;

    LOG_INFO().noquote() << "检查PSAM授权状态...";
    ret = m_readerCtrlAPI->CheckPsamNeedAuth(m_slotNo);
    if (ret != 0 && ret != 1) {
        LOG_WARNING().noquote() << "检查PSAM授权状态失败: 错误码" << ret;
        return false;
    } else {
        LOG_INFO().noquote() << "检查PSAM授权状态成功:" << "PSAM卡" << psamNo << (ret == 1 ? "需要授权" : "不需要授权");
    }
    needAuth = ret;

    TPsamRand psamRand = {};
    int randResult = m_readerCtrlAPI->GetPsamRand(m_slotNo, psamRand);
    if (randResult != 0) {
        LOG_ERROR().noquote() << "获取PSAM卡版本及随机数失败，错误码" << randResult;
        return false;
    }

    const QByteArray randPsamNo = DataDealUtils::bufferToByteArray(psamRand.hexCardID, sizeof(psamRand.hexCardID));
    if (randPsamNo != psamNo) {
        LOG_ERROR().noquote() << "两次读取的PSAM卡号不一致" << psamNo << randPsamNo;
        return false;
    }

    if (needAuth == 1) {
        if (psamRand.CardVersion < 5) {
            LOG_ERROR().noquote() << "授权状态与PSAM卡版本不一致，卡版本" << psamRand.CardVersion;
            return false;
        }

        random = DataDealUtils::bufferToByteArray(psamRand.hexRand, sizeof(psamRand.hexRand));
        LOG_INFO().noquote() << "获取PSAM卡授权随机值成功:" << random << "cardVersion:" << psamRand.CardVersion
                             << "cardType:" << psamRand.CardKeyType;
    }

    m_readerCtrlAPI->Beep();
    return true;
}

QString ReaderCtrl::getCPUCardNum()
{
    TReadCardInfoResult readCardResult{};

    int res = m_readerCtrlAPI->ReadCPUInfo(readCardResult.CPUBaseData, readCardResult.CPUTollData, readCardResult.CPURemark, readCardResult.RestMoney);
    if (res == 0) {
        readCardResult.CardType = readCardResult.CPUBaseData[8];
    }

    // 解析读卡结果
    TCPCSysInfo cpcSysInfo;
    TCPCEnExInfo cpcEnExInfo;
    TCPCBaseInfo cpcBaseInfo;
    TCPCPassInfo cpcPassInfo;
    TCPCFeeInfo cpcFeeInfo;
    TCCBaseData ccBaseData;
    TCCTollData ccTollData;
    unsigned char flag = 0;
    unsigned char seats = 0;

    CCardTransform transformer;
    transformer.ParseCardInfoResult(readCardResult, cpcSysInfo, cpcBaseInfo, cpcEnExInfo, cpcPassInfo, cpcFeeInfo, ccBaseData, ccTollData, flag,
                                    seats);

    // 返回卡号
    QString cpuCardNum = QString(ccBaseData.hexCardNet) + QString(ccBaseData.hexCardNo);
    return cpuCardNum;
}

int ReaderCtrl::readCardInfo(TReadCardInfoResult &cardReadResult, bool isEntranceLane)
{
    const int physicalCardType = m_readerCtrlAPI->OpenCardEx();
    if (physicalCardType < 0) {
        LOG_ERROR().noquote() << "打开卡失败，错误码" << physicalCardType;
        return -1;
    }

    LOG_INFO().noquote() << "打开卡返回物理卡类型" << physicalCardType;

    int apiResult = 0;
    if (physicalCardType == 2) {
        apiResult = m_readerCtrlAPI->ReadCPUInfo(cardReadResult.CPUBaseData, cardReadResult.CPUTollData, cardReadResult.CPURemark,
                                                 cardReadResult.RestMoney);
        if (apiResult == 0) {
            const BYTE logicalCardType = static_cast<BYTE>(cardReadResult.CPUBaseData[8]);
            if (logicalCardType == 20 || logicalCardType == 22 || logicalCardType == 23) {
                cardReadResult.CardType = logicalCardType;
            } else {
                LOG_ERROR().noquote() << "CPU卡逻辑卡类型不受支持" << logicalCardType;
                apiResult = -2;
            }
        }
    } else if (physicalCardType == 4) {
        apiResult = m_readerCtrlAPI->ReadCPCInfo(cardReadResult.CPCSysInfo, cardReadResult.CPCBaseInfo, cardReadResult.CPCEnExInfo,
                                                 cardReadResult.CPCPassInfo, cardReadResult.CPCPassLen, cardReadResult.CPCFeeInfo,
                                                 cardReadResult.CPCFeeLen, isEntranceLane);
        if (apiResult == 0)
            cardReadResult.CardType = 15;
    } else {
        LOG_ERROR().noquote() << "不受支持的物理卡类型" << physicalCardType;
        apiResult = -2;
    }

    m_readerCtrlAPI->CloseCard();
    m_readerCtrlAPI->Beep();

    if (apiResult != 0) {
        LOG_ERROR().noquote() << "读取卡数据失败，物理卡类型" << physicalCardType << "错误码" << apiResult;
        return -2;
    }

    return 0;
}
