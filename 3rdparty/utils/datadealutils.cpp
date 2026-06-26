#include "datadealutils.h"

#include <QCryptographicHash>
#include <QDateTime>
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    #include <QRandomGenerator>
#endif
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMetaProperty>
#include <QTextCodec>
#include <QVariant>

using namespace Utils;

DataDealUtils::DataDealUtils(QObject *parent)
    : QObject{parent}
{}

DataDealUtils::~DataDealUtils() {}

QString DataDealUtils::cryptoMD5(const QString &s, bool bUtf8 /*= true */)
{
    QByteArray input = bUtf8 ? s.toUtf8() : s.toLocal8Bit();
    QByteArray hash = QCryptographicHash::hash(input, QCryptographicHash::Md5);
    return QString(hash.toHex()).toUpper();
}

quint16 DataDealUtils::getModbus16(quint8 *data, int len)
{
    const quint16 table16[256] = {0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1,
                                  0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40,
                                  0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1,
                                  0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
                                  0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1,
                                  0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40,
                                  0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1,
                                  0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
                                  0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0,
                                  0x2080, 0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740,
                                  0xA501, 0x65C0, 0x6480, 0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0,
                                  0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
                                  0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1,
                                  0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181, 0x5140,
                                  0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440, 0x9C01, 0x5CC0,
                                  0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
                                  0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0,
                                  0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341,
                                  0x4100, 0x81C1, 0x8081, 0x4040};

    quint16 crc16 = 0xFFFF;
    quint8 temp;

    while (len-- > 0) {
        temp = crc16 & 0xff;
        crc16 = (crc16 >> 8) ^ table16[(temp ^ *data++) & 0xff];
    }

    return crc16;
}

quint16 DataDealUtils::getXModem16(quint8 *data, int len)
{
    const quint16 table16[256] = {0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD,
                                  0xE1CE, 0xF1EF, 0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6, 0x9339, 0x8318, 0xB37B, 0xA35A,
                                  0xD3BD, 0xC39C, 0xF3FF, 0xE3DE, 0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485, 0xA56A, 0xB54B,
                                  0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D, 0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
                                  0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC, 0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861,
                                  0x2802, 0x3823, 0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B, 0x5AF5, 0x4AD4, 0x7AB7, 0x6A96,
                                  0x1A71, 0x0A50, 0x3A33, 0x2A12, 0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A, 0x6CA6, 0x7C87,
                                  0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
                                  0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70, 0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A,
                                  0x9F59, 0x8F78, 0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F, 0x1080, 0x00A1, 0x30C2, 0x20E3,
                                  0x5004, 0x4025, 0x7046, 0x6067, 0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, 0x02B1, 0x1290,
                                  0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256, 0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
                                  0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E,
                                  0xC71D, 0xD73C, 0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634, 0xD94C, 0xC96D, 0xF90E, 0xE92F,
                                  0x99C8, 0x89E9, 0xB98A, 0xA9AB, 0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3, 0xCB7D, 0xDB5C,
                                  0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A, 0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
                                  0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, 0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83,
                                  0x1CE0, 0x0CC1, 0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 0x6E17, 0x7E36, 0x4E55, 0x5E74,
                                  0x2E93, 0x3EB2, 0x0ED1, 0x1EF0};

    quint16 crc = 0x0000;
    for (int i = 0; i < len; ++i) {
        quint8 idx = static_cast<quint8>((crc >> 8) ^ data[i]);
        crc = static_cast<quint16>((crc << 8) ^ table16[idx]);
    }
    return crc;
}

quint16 DataDealUtils::getX2516(quint8 *data, int len)
{
    const quint16 table16[256] = {0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF, 0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5,
                                  0xE97E, 0xF8F7, 0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E, 0x9CC9, 0x8D40, 0xBFDB, 0xAE52,
                                  0xDAED, 0xCB64, 0xF9FF, 0xE876, 0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD, 0xAD4A, 0xBCC3,
                                  0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5, 0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
                                  0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974, 0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9,
                                  0x2732, 0x36BB, 0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3, 0x5285, 0x430C, 0x7197, 0x601E,
                                  0x14A1, 0x0528, 0x37B3, 0x263A, 0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72, 0x6306, 0x728F,
                                  0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9, 0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
                                  0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738, 0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862,
                                  0x9AF9, 0x8B70, 0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7, 0x0840, 0x19C9, 0x2B52, 0x3ADB,
                                  0x4E64, 0x5FED, 0x6D76, 0x7CFF, 0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036, 0x18C1, 0x0948,
                                  0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E, 0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
                                  0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD, 0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226,
                                  0xD0BD, 0xC134, 0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C, 0xC60C, 0xD785, 0xE51E, 0xF497,
                                  0x8028, 0x91A1, 0xA33A, 0xB2B3, 0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB, 0xD68D, 0xC704,
                                  0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232, 0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
                                  0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1, 0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB,
                                  0x0E70, 0x1FF9, 0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330, 0x7BC7, 0x6A4E, 0x58D5, 0x495C,
                                  0x3DE3, 0x2C6A, 0x1EF1, 0x0F78};

    quint16 crc = 0xFFFF;
    while (len-- > 0) {
        quint8 idx = static_cast<quint8>(crc ^ *data++);
        crc = (crc >> 8) ^ table16[idx];
    }
    return crc ^ 0xFFFF;
}

QByteArray DataDealUtils::getCRCCode(const QByteArray &data, int type /*= 3*/)
{
    quint16 result = 0x0000;
    if (type == 1)
        result = getModbus16((quint8 *) data.data(), data.length());
    else if (type == 2)
        result = getXModem16((quint8 *) data.data(), data.length());
    else if (type == 3)
        result = getX2516((quint8 *) data.data(), data.length());

    return DataDealUtils::ushortToByte(result);
}

QString DataDealUtils::getXorEncryptDecrypt(const QString &str, char key)
{
    QByteArray data = str.toLatin1();
    int size = data.size();
    for (int i = 0; i < size; i++) {
        data[i] = data[i] ^ key;
    }

    return QLatin1String(data);
}

uchar DataDealUtils::getOrCode(const QByteArray &data)
{
    int len = data.length();
    uchar result = 0;

    for (int i = 0; i < len; i++) {
        result ^= data.at(i);
    }

    return result;
}

uchar DataDealUtils::getCheckCode(const QByteArray &data)
{
    int len = data.length();
    uchar temp = 0;

    for (uchar i = 0; i < len; i++) {
        temp += data.at(i);
    }

    return temp % 256;
}

QString DataDealUtils::curDateTimeStr(const QString &format /*= "yyyy-MM-dd hh:mm:ss" */)
{
    return QDateTime::currentDateTime().toString(format);
}

QString DataDealUtils::curDateStr(const QString &format /*= "yyyy-MM-dd hh:mm:ss" */)
{
    QDateTime dt(QDate::currentDate(), QTime(0, 0, 0));
    return dt.toString(format);
}

QDateTime DataDealUtils::curDateTime()
{
    return QDateTime::currentDateTime();
}

QDateTime DataDealUtils::curDate()
{
    QDateTime dt(QDate::currentDate(), QTime(0, 0, 0));
    return dt;
}

uint DataDealUtils::curUnixDateTime()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    return QDateTime::currentSecsSinceEpoch();
#else
    return QDateTime::currentDateTimeUtc().toTime_t();
#endif
}

QString DataDealUtils::padValue(quint32 value, int width /*=-1 */)
{
    if (width <= 0) {
        return QString::number(value);
    }

    QString str = QString::number(value);
    return str.rightJustified(width, QChar('0'));
}

int DataDealUtils::strHexToDecimal(const QString &strHex)
{
    bool ok;
    return strHex.toInt(&ok, 16);
}

int DataDealUtils::strDecimalToDecimal(const QString &strDecimal)
{
    bool ok;
    return strDecimal.toInt(&ok, 10);
}

int DataDealUtils::strBinToDecimal(const QString &strBin)
{
    bool ok;
    return strBin.toInt(&ok, 2);
}

QString DataDealUtils::strHexToStrBin(const QString &strHex)
{
    uchar decimal = strHexToDecimal(strHex);
    QString bin = QString::number(decimal, 2);
    uchar len = bin.length();

    if (len < 8) {
        for (int i = 0; i < 8 - len; i++) {
            bin = "0" + bin;
        }
    }

    return bin;
}

QString DataDealUtils::decimalToStrBin1(int decimal)
{
    QString bin = QString::number(decimal, 2);
    uchar len = bin.length();

    if (len <= 8) {
        for (int i = 0; i < 8 - len; i++) {
            bin = "0" + bin;
        }
    }

    return bin;
}

QString DataDealUtils::decimalToStrBin2(int decimal)
{
    QString bin = QString::number(decimal, 2);
    uchar len = bin.length();

    if (len <= 16) {
        for (int i = 0; i < 16 - len; i++) {
            bin = "0" + bin;
        }
    }

    return bin;
}

QString DataDealUtils::decimalToStrHex(int decimal)
{
    QString temp = QString::number(decimal, 16);
    if (temp.length() == 1) {
        temp = "0" + temp;
    }

    return temp;
}

QByteArray DataDealUtils::intToByte(int i)
{
    QByteArray result;
    result.resize(4);
    result[3] = (uchar) (0x000000ff & i);
    result[2] = (uchar) ((0x0000ff00 & i) >> 8);
    result[1] = (uchar) ((0x00ff0000 & i) >> 16);
    result[0] = (uchar) ((0xff000000 & i) >> 24);
    return result;
}

QByteArray DataDealUtils::intToByteRec(int i)
{
    QByteArray result;
    result.resize(4);
    result[0] = (uchar) (0x000000ff & i);
    result[1] = (uchar) ((0x0000ff00 & i) >> 8);
    result[2] = (uchar) ((0x00ff0000 & i) >> 16);
    result[3] = (uchar) ((0xff000000 & i) >> 24);
    return result;
}

int DataDealUtils::byteToInt(const QByteArray &data)
{
    int i = data.at(3) & 0x000000ff;
    i |= ((data.at(2) << 8) & 0x0000ff00);
    i |= ((data.at(1) << 16) & 0x00ff0000);
    i |= ((data.at(0) << 24) & 0xff000000);
    return i;
}

int DataDealUtils::byteToIntRec(const QByteArray &data)
{
    int i = data.at(0) & 0x000000ff;
    i |= ((data.at(1) << 8) & 0x0000ff00);
    i |= ((data.at(2) << 16) & 0x00ff0000);
    i |= ((data.at(3) << 24) & 0xff000000);
    return i;
}

quint32 DataDealUtils::byteToUInt(const QByteArray &data)
{
    quint32 i = data.at(3) & 0x000000ff;
    i |= ((data.at(2) << 8) & 0x0000ff00);
    i |= ((data.at(1) << 16) & 0x00ff0000);
    i |= ((data.at(0) << 24) & 0xff000000);
    return i;
}

quint32 DataDealUtils::byteToUIntRec(const QByteArray &data)
{
    quint32 i = data.at(0) & 0x000000ff;
    i |= ((data.at(1) << 8) & 0x0000ff00);
    i |= ((data.at(2) << 16) & 0x00ff0000);
    i |= ((data.at(3) << 24) & 0xff000000);
    return i;
}

QByteArray DataDealUtils::ushortToByte(ushort i)
{
    QByteArray result;
    result.resize(2);
    result[1] = (uchar) (0x000000ff & i);
    result[0] = (uchar) ((0x0000ff00 & i) >> 8);
    return result;
}

QByteArray DataDealUtils::ushortToByteRec(ushort i)
{
    QByteArray result;
    result.resize(2);
    result[0] = (uchar) (0x000000ff & i);
    result[1] = (uchar) ((0x0000ff00 & i) >> 8);
    return result;
}

int DataDealUtils::byteToUShort(const QByteArray &data)
{
    int i = data.at(1) & 0x000000FF;
    i |= ((data.at(0) << 8) & 0x0000FF00);

    if (i >= 32768) {
        i = i - 65536;
    }

    return i;
}

int DataDealUtils::byteToUShortRec(const QByteArray &data)
{
    int i = data.at(0) & 0x000000FF;
    i |= ((data.at(1) << 8) & 0x0000FF00);

    if (i >= 32768) {
        i = i - 65536;
    }

    return i;
}

QString DataDealUtils::byteArrayToAsciiStr(const QByteArray &data)
{
    QString temp;
    int len = data.size();

    for (int i = 0; i < len; i++) {
        // 0x20为空格,空格以下都是不可见字符
        char b = data.at(i);

        if (0x00 == b) {
            temp += QString("\\NUL");
        } else if (0x01 == b) {
            temp += QString("\\SOH");
        } else if (0x02 == b) {
            temp += QString("\\STX");
        } else if (0x03 == b) {
            temp += QString("\\ETX");
        } else if (0x04 == b) {
            temp += QString("\\EOT");
        } else if (0x05 == b) {
            temp += QString("\\ENQ");
        } else if (0x06 == b) {
            temp += QString("\\ACK");
        } else if (0x07 == b) {
            temp += QString("\\BEL");
        } else if (0x08 == b) {
            temp += QString("\\BS");
        } else if (0x09 == b) {
            temp += QString("\\HT");
        } else if (0x0A == b) {
            temp += QString("\\LF");
        } else if (0x0B == b) {
            temp += QString("\\VT");
        } else if (0x0C == b) {
            temp += QString("\\FF");
        } else if (0x0D == b) {
            temp += QString("\\CR");
        } else if (0x0E == b) {
            temp += QString("\\SO");
        } else if (0x0F == b) {
            temp += QString("\\SI");
        } else if (0x10 == b) {
            temp += QString("\\DLE");
        } else if (0x11 == b) {
            temp += QString("\\DC1");
        } else if (0x12 == b) {
            temp += QString("\\DC2");
        } else if (0x13 == b) {
            temp += QString("\\DC3");
        } else if (0x14 == b) {
            temp += QString("\\DC4");
        } else if (0x15 == b) {
            temp += QString("\\NAK");
        } else if (0x16 == b) {
            temp += QString("\\SYN");
        } else if (0x17 == b) {
            temp += QString("\\ETB");
        } else if (0x18 == b) {
            temp += QString("\\CAN");
        } else if (0x19 == b) {
            temp += QString("\\EM");
        } else if (0x1A == b) {
            temp += QString("\\SUB");
        } else if (0x1B == b) {
            temp += QString("\\ESC");
        } else if (0x1C == b) {
            temp += QString("\\FS");
        } else if (0x1D == b) {
            temp += QString("\\GS");
        } else if (0x1E == b) {
            temp += QString("\\RS");
        } else if (0x1F == b) {
            temp += QString("\\US");
        } else if (0x7F == b) {
            temp += QString("\\x7F");
        } else if (0x5C == b) {
            temp += QString("\\x5C");
        } else if (0x20 >= b) {
            temp += QString("\\x%1").arg(decimalToStrHex((quint8) b));
        } else {
            temp += QString("%1").arg(b);
        }
    }

    return temp.trimmed();
}

QString DataDealUtils::byteArrayToBCDStr(const QByteArray &data)
{
    QString result;
    for (auto b : data) {
        quint8 v = static_cast<quint8>(b);
        result += QString::number((v >> 4) & 0x0F); // 高4位
        result += QString::number(v & 0x0F);        // 低4位
    }
    return result;
}

QByteArray DataDealUtils::hexStrToByteArray(const QString &str)
{
    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len / 2);
    char lstr, hstr;

    for (int i = 0; i < len;) {
        hstr = str.at(i).toLatin1();
        if (hstr == ' ') {
            i++;
            continue;
        }

        i++;
        if (i >= len) {
            break;
        }

        lstr = str.at(i).toLatin1();
        hexdata = convertHexChar(hstr);
        lowhexdata = convertHexChar(lstr);

        if ((hexdata == 16) || (lowhexdata == 16)) {
            break;
        } else {
            hexdata = hexdata * 16 + lowhexdata;
        }

        i++;
        senddata[hexdatalen] = (char) hexdata;
        hexdatalen++;
    }

    senddata.resize(hexdatalen);
    return senddata;
}

char DataDealUtils::convertHexChar(char ch)
{
    if ((ch >= '0') && (ch <= '9')) {
        return ch - 0x30;
    } else if ((ch >= 'A') && (ch <= 'F')) {
        return ch - 'A' + 10;
    } else if ((ch >= 'a') && (ch <= 'f')) {
        return ch - 'a' + 10;
    } else {
        return (-1);
    }
}

QByteArray DataDealUtils::asciiStrToByteArray(const QString &str)
{
    QByteArray buffer;
    int len = str.length();
    QString letter;
    QString hex;

    for (int i = 0; i < len; i++) {
        letter = str.at(i);

        if (letter == "\\") {
            i++;
            letter = str.mid(i, 1);

            if (letter == "x") {
                i++;
                hex = str.mid(i, 2);
                buffer.append(strHexToDecimal(hex));
                i++;
                continue;
            } else if (letter == "N") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "U") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "L") { // NUL=0x00
                        buffer.append((char) 0x00);
                        continue;
                    }
                } else if (hex == "A") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "K") { // NAK=0x15
                        buffer.append(0x15);
                        continue;
                    }
                }
            } else if (letter == "S") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "O") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "H") { // SOH=0x01
                        buffer.append(0x01);
                        continue;
                    } else { // SO=0x0E
                        buffer.append(0x0E);
                        i--;
                        continue;
                    }
                } else if (hex == "T") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "X") { // STX=0x02
                        buffer.append(0x02);
                        continue;
                    }
                } else if (hex == "I") { // SI=0x0F
                    buffer.append(0x0F);
                    continue;
                } else if (hex == "Y") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "N") { // SYN=0x16
                        buffer.append(0x16);
                        continue;
                    }
                } else if (hex == "U") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "B") { // SUB=0x1A
                        buffer.append(0x1A);
                        continue;
                    }
                }
            } else if (letter == "E") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "T") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "X") { // ETX=0x03
                        buffer.append(0x03);
                        continue;
                    } else if (hex == "B") { // ETB=0x17
                        buffer.append(0x17);
                        continue;
                    }
                } else if (hex == "O") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "T") { // EOT=0x04
                        buffer.append(0x04);
                        continue;
                    }
                } else if (hex == "N") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "Q") { // ENQ=0x05
                        buffer.append(0x05);
                        continue;
                    }
                } else if (hex == "M") { // EM=0x19
                    buffer.append(0x19);
                    continue;
                } else if (hex == "S") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "C") { // ESC=0x1B
                        buffer.append(0x1B);
                        continue;
                    }
                }
            } else if (letter == "A") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "C") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "K") { // ACK=0x06
                        buffer.append(0x06);
                        continue;
                    }
                }
            } else if (letter == "B") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "E") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "L") { // BEL=0x07
                        buffer.append(0x07);
                        continue;
                    }
                } else if (hex == "S") { // BS=0x08
                    buffer.append(0x08);
                    continue;
                }
            } else if (letter == "C") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "R") { // CR=0x0D
                    buffer.append(0x0D);
                    continue;
                } else if (hex == "A") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "N") { // CAN=0x18
                        buffer.append(0x18);
                        continue;
                    }
                }
            } else if (letter == "D") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "L") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "E") { // DLE=0x10
                        buffer.append(0x10);
                        continue;
                    }
                } else if (hex == "C") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "1") { // DC1=0x11
                        buffer.append(0x11);
                        continue;
                    } else if (hex == "2") { // DC2=0x12
                        buffer.append(0x12);
                        continue;
                    } else if (hex == "3") { // DC3=0x13
                        buffer.append(0x13);
                        continue;
                    } else if (hex == "4") { // DC2=0x14
                        buffer.append(0x14);
                        continue;
                    }
                }
            } else if (letter == "F") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "F") { // FF=0x0C
                    buffer.append(0x0C);
                    continue;
                } else if (hex == "S") { // FS=0x1C
                    buffer.append(0x1C);
                    continue;
                }
            } else if (letter == "H") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "T") { // HT=0x09
                    buffer.append(0x09);
                    continue;
                }
            } else if (letter == "L") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "F") { // LF=0x0A
                    buffer.append(0x0A);
                    continue;
                }
            } else if (letter == "G") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "S") { // GS=0x1D
                    buffer.append(0x1D);
                    continue;
                }
            } else if (letter == "R") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "S") { // RS=0x1E
                    buffer.append(0x1E);
                    continue;
                }
            } else if (letter == "U") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "S") { // US=0x1F
                    buffer.append(0x1F);
                    continue;
                }
            } else if (letter == "V") {
                i++;
                hex = str.mid(i, 1);

                if (hex == "T") { // VT=0x0B
                    buffer.append(0x0B);
                    continue;
                }
            } else if (letter == "\\") {
                // 如果连着的是多个\\则对应添加\对应的16进制0x5C
                buffer.append(0x5C);
                continue;
            } else {
                // 将对应的\[前面的\\也要加入
                buffer.append(0x5C);
                buffer.append(letter.toLatin1());
                continue;
            }
        }

        buffer.append(str.mid(i, 1).toLatin1());
    }

    return buffer;
}

QString DataDealUtils::byteArrayToHexStr(const QByteArray &data, bool withBlack)
{
    QString temp = "";
    QString hex = data.toHex();

    for (int i = 0; i < hex.length(); i = i + 2) {
        temp += hex.mid(i, 2) + " ";
    }

    if (!withBlack)
        return temp.replace(" ", "").toUpper();

    return temp.trimmed().toUpper();
}

QString DataDealUtils::trimmed(const QString &text, short type)
{
    QString temp = text;
    QString pattern;
    if (type == -1) {
        pattern = "^ +\\s*";
    } else if (type == 0) {
        pattern = "\\s";
    } else if (type == 1) {
        pattern = "\\s* +$";
    } else if (type == 2) {
        temp = temp.trimmed();
    } else if (type == 3) {
        temp = temp.simplified();
    }
    // 调用正则表达式移除空格
    if (!pattern.isEmpty()) {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
        temp.remove(QRegularExpression(pattern));
#else
        temp.remove(QRegExp(pattern));
#endif
    }

    return temp;
}

int DataDealUtils::getChineseCountFromString(const QString &data, int checkLen)
{
    int nCount = 0;
    int limit = qMin(checkLen, data.size());
    for (int i = 0; i < limit; ++i) {
        QChar ch = data.at(i);
        // 中文字符Unicode区间
        if (ch.unicode() >= 0x4E00 && ch.unicode() <= 0x9FFF)
            nCount++;
    }
    return nCount;
}

int DataDealUtils::getRandomNum(quint32 boundary)
{
    if (boundary == 0)
        return 0;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return QRandomGenerator::global()->bounded(boundary);
#else
    return qrand() % boundary;
#endif
}

int DataDealUtils::calcStrMatchScore(const QString &fullStr, const QString &partStr)
{
    if (fullStr.isEmpty() || partStr.isEmpty())
        return 0;

    // 完全包含，直接认为100%
    if (fullStr.contains(partStr))
        return 100;

    if (fullStr.size() < partStr.size())
        return 0;

    int maxMatchCnt = 0;
    // 滑动窗口比较
    for (int offset = 0; offset <= fullStr.size() - partStr.size(); ++offset) {
        int matchCnt = 0;

        for (int i = 0; i < partStr.size(); ++i) {
            if (fullStr.at(offset + i) == partStr.at(i)) {
                ++matchCnt;
            }
        }

        if (matchCnt > maxMatchCnt)
            maxMatchCnt = matchCnt;

        qDebug().noquote() << maxMatchCnt << matchCnt;
    }

    return maxMatchCnt * 100 / partStr.size();
}

QByteArray DataDealUtils::encodeString(const QString &text, int coding)
{
    if (coding == 1) {
        QTextCodec *codec = QTextCodec::codecForName("GBK");
        return codec ? codec->fromUnicode(text) : text.toLocal8Bit();
    } else if (coding == 2) {
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        return codec ? codec->fromUnicode(text) : text.toUtf8();
    }
    return text.toLocal8Bit();
}

QString DataDealUtils::decodeByteArray(const QByteArray &data, int coding)
{
    switch (coding) {
    case 1: {
        static QTextCodec *codec = QTextCodec::codecForName("GBK");
        return codec ? codec->toUnicode(data) : QString::fromLocal8Bit(data);
    } break;
    case 2: {
        return QString::fromUtf8(data);
    } break;
    default: {
        return QString::fromLocal8Bit(data);
    } break;
    }
}

void DataDealUtils::appendStrWithSubStr(QString &str, const QString &subStr, const QString &delimiter)
{
    if (subStr.isEmpty())
        return;
    if (str.length())
        str += delimiter;
    str += subStr;
}

QString DataDealUtils::getInsertSql(const QObject *obj)
{
    if (!obj)
        return QString();

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QStringList autoIncCols = obj->property("auto_inc").toString().split(",", Qt::SkipEmptyParts);
#else
    QStringList autoIncCols = obj->property("auto_inc").toString().split(",", QString::SkipEmptyParts);
#endif
    for (QString &col : autoIncCols) {
        col = col.trimmed();
    }

    QString tableName = obj->metaObject()->className();

    QStringList colList;
    QStringList valList;

    for (int i = obj->metaObject()->propertyOffset(); i < obj->metaObject()->propertyCount(); ++i) {
        QMetaProperty prp = obj->metaObject()->property(i);
        QString colName = QString::fromLatin1(prp.name());

        if (colName == "objectName" || colName == "tbl_pk" || colName == "auto_inc" || autoIncCols.contains(colName))
            continue;

        QVariant val = obj->property(prp.name());

        colList << colName.toLower();
        valList << formatSqlValue(val);
    }

    // 没有可插入字段
    if (colList.isEmpty())
        return QString();

    QString sql = QString("INSERT INTO %1(%2) VALUES(%3)").arg(tableName.toLower(), colList.join(","), valList.join(","));

    return sql;
}

QString DataDealUtils::getUpdateSql(const QObject *obj)
{
    if (!obj)
        return QString();

    QString tableName = obj->metaObject()->className();

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QStringList pkList = obj->property("tbl_pk").toString().split(",", Qt::SkipEmptyParts);
    QStringList autoIncCols = obj->property("auto_inc").toString().split(",", Qt::SkipEmptyParts);
#else
    QStringList pkList = obj->property("tbl_pk").toString().split(",", QString::SkipEmptyParts);
    QStringList autoIncCols = obj->property("auto_inc").toString().split(",", QString::SkipEmptyParts);
#endif
    for (QString &col : autoIncCols) {
        col = col.trimmed();
    }
    for (QString &pk : pkList) {
        pk = pk.trimmed();
    }

    // 构建 SET 子句
    QStringList setClauses;
    for (int i = obj->metaObject()->propertyOffset(); i < obj->metaObject()->propertyCount(); ++i) {
        QMetaProperty prp = obj->metaObject()->property(i);
        QString colName = QString::fromLatin1(prp.name());

        if (colName == "objectName" || colName == "tbl_pk" || colName == "auto_inc" || autoIncCols.contains(colName))
            continue;
        // 跳过主键字段，不在 SET 中更新
        if (pkList.contains(colName))
            continue;
        QVariant val = obj->property(prp.name());
        QString formattedVal = formatSqlValue(val);

        setClauses << QString("%1=%2").arg(colName.toLower(), formattedVal);
    }

    // 没有可更新字段
    if (setClauses.isEmpty())
        return QString();

    // 构建 WHERE 子句
    QStringList whereClauses;
    whereClauses << QStringLiteral("1=1");
    for (const QString &pk : pkList) {
        QVariant val = obj->property(pk.toUtf8().constData());
        QString formattedVal = formatSqlValue(val);
        whereClauses << QString("%1=%2").arg(pk.toLower(), formattedVal);
    }

    QString sql = QString("UPDATE %1 SET %2 WHERE %3").arg(tableName.toLower(), setClauses.join(","), whereClauses.join(" AND "));
    return sql;
}

QString DataDealUtils::fullExecutedQuery(const QSqlQuery &query)
{
    QString sql = query.lastQuery();
    if (sql.isEmpty())
        return sql;

    auto bvals = query.boundValues();
    if (bvals.isEmpty())
        return sql;

    QString result = sql;

    // SQL 是否包含 ? 占位符
    bool isPositional = result.contains('?');

    if (isPositional) {
        // 位置绑定，按顺序替换 ?
        QList<QVariant> values = bvals.values(); // 忽略键名
        for (const QVariant &val : values) {
            QString v;
            if (val.isNull()) {
                v = "NULL";
            } else if (val.type() == QVariant::String || val.type() == QVariant::DateTime || val.type() == QVariant::Date
                       || val.type() == QVariant::Time) {
                v = "'" + val.toString().replace("'", "''") + "'";
            } else {
                v = val.toString();
            }

            int pos = result.indexOf('?');
            if (pos != -1)
                result.replace(pos, 1, v);
        }
    } else {
        // 命名绑定
        for (auto it = bvals.begin(); it != bvals.end(); ++it) {
            QString v;
            if (it.value().isNull()) {
                v = "NULL";
            } else if (it.value().type() == QVariant::String || it.value().type() == QVariant::DateTime || it.value().type() == QVariant::Date
                       || it.value().type() == QVariant::Time) {
                v = "'" + it.value().toString().replace("'", "''") + "'";
            } else {
                v = it.value().toString();
            }

            result.replace(it.key(), v);
        }
    }

    return result;
}

QVariantMap DataDealUtils::jsonToMap(const QByteArray &data, bool &ok, QString &errDesc)
{
    ok = false;
    errDesc.clear();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);

    if (err.error != QJsonParseError::NoError) {
        errDesc = "Json转换QVariantMap失败: " + err.errorString();
        return {};
    }

    if (!doc.isObject()) {
        errDesc = "Json转换QVariantMap失败: 根节点不是对象";
        return {};
    }

    ok = true;
    return doc.object().toVariantMap();
}

QVariantList DataDealUtils::jsonToList(const QByteArray &data, bool &ok, QString &errDesc)
{
    ok = false;
    errDesc.clear();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);

    if (err.error != QJsonParseError::NoError) {
        errDesc = "Json转换QVariantList失败: " + err.errorString();
        return {};
    }

    if (!doc.isArray()) {
        errDesc = "Json转换QVariantList失败: 根节点不是数组";
        return {};
    }

    ok = true;
    return doc.array().toVariantList();
}

QByteArray DataDealUtils::mapToJson(const QVariantMap &map, QJsonDocument::JsonFormat format)
{
    QJsonObject obj = QJsonObject::fromVariantMap(map);
    QJsonDocument doc(obj);
    return doc.toJson(format);
}

QByteArray DataDealUtils::listToJson(const QVariantList &list, QJsonDocument::JsonFormat format)
{
    QJsonArray array = QJsonArray::fromVariantList(list);
    QJsonDocument doc(array);
    return doc.toJson(format);
}

QString DataDealUtils::formatSqlValue(const QVariant &val)
{
    if (!val.isValid() || val.isNull())
        return "NULL";

    switch (val.type()) {
    case QVariant::String:
        return "'" + val.toString().replace("'", "''") + "'";
    case QVariant::Date:
        return "'" + val.toDate().toString("yyyy-MM-dd") + "'";
    case QVariant::DateTime:
        return "'" + val.toDateTime().toString("yyyy-MM-dd HH:mm:ss") + "'";
    case QVariant::Bool:
        return val.toBool() ? "1" : "0";
    default:
        return val.toString();
    }
}
