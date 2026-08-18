//---------------------------------------------------------------------------
//通用工具类,Author:Huanglr,Date:2014-05-24  LastUpdate:2019-04-26
//---------------------------------------------------------------------------
#ifndef Common_Tool_H_
#define Common_Tool_H_
//---------------------------------------------------------------------------

#include "math.h"
#include "stdio.h"
#include "time.h"
#include <QSettings>
#include <QDateTime>
#include <QTextCodec>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>

typedef unsigned char BYTE;
typedef unsigned int  UINT;
typedef unsigned short USHORT;

class CommonTool
{
private:
   QSettings *m_ini_file;
   QTextCodec *m_gbk;
   QTextCodec *m_utf;
public:
   CommonTool();
   ~CommonTool(){}
public:
   void    OpenIniFile(QString ini_filename, int nType=0);//0:UTF-8 1:GBK
   void    CloseIniFile();
   int     IniReadInt(QString section,QString key,int def=0);
   QString  IniReadStr(QString section,QString key,QString def);
   void    IniWriteInt(QString section,QString key,int Value);
   void    IniWriteStr(QString section,QString key,QString Value);
   QString knowChinese(const QByteArray &ba);
public:
   int  posIC(const QString str,const QString sub);
   int  StrPosEx(QString src,QString chd,QString div=",");
   QString  makeXmlNode(const QString field,QString value);
   //格式化字符串
   QString FmtStr(char *fmt,...);
   QString gbk_to_utf(const char *s,int nLen=-1);
   QByteArray utf_to_gbk(const char *s, int nLen=-1); //QString会把gbk的中文变成乱码,不能用QString保存gbk格式数据
   //字串转时间,格式yyyy-mm-dd hh:nn:ss
   QDateTime  StrToDateTimeX(QString v);
   //字串转时间,格式yyyymmddhhnnss
   QDateTime  Str2DateTime(QString sDate);
   //字串转Dos时间,包含时区,格式yyyymmddhhnnss
   int   Str2DosTime(QString sDate);
   //字串转Unix时间,包含时区,格式yyyymmddhhnnss
   int   Str2UnixTime(QString sDate);
   int    unix2dos_time(time_t t);
   int    dos2unix_time(int t);
   QDateTime  dos2datetime(int t);
   QDateTime  unix2datetime(int t,bool toLocal=true);
   char*    GetpStrDate(time_t t,bool hasDiv,bool hasTime);
   QString GetStrDate(time_t t,bool hasDiv,bool hasTime);
   unsigned char dec2bcd(unsigned char dec);
   uint getCurUnixTime();
   //获取带空格Hex字符串
   QString  GetPrintHex(char *hex);
   //查找目录下的文件(nMaxFiles为0表示不限制,sExt必须为"*.xml"类似格式)
   void   GetDirFiles(QString sPath,QString sExt,QStringList *fileList,int nMaxFiles=0);
   QStringList GetDirFiles(QString sPath,QString sExt);
   //删除过期文件
   void   RemoveHisFiles(QString sPath,QString sExt,int days);
   QString GetSubString(QString sInfo,int len);//截取字符串一定长度，一个中文按2个字节来算
   QString GetAppDir();
   void CreateDirIfNotExist(QString qstrDirPath); //只能建一层目录
   //-----------20220606.huanglr.append----------------------------------------//
   QString createPathEx(QString path);         //递归创建多层目录,目录用/表示！
   bool removePathAndFile(QString path);       //递归删除目录及目录下所有文件！
   void removeHisDatePath(QString parentPath,int days,bool isShortDate=false); //删除过期日期目录及目录下所有文件。
   bool checkFileExist(QString fullFileName);
   bool checkPathExist(QString fullPathName);
   //--------------------------------------------------------------------------//
   bool CheckNoOverNowEx(QDateTime dt,int nMilliSec);
   bool CheckOverNowEx(QDateTime dt,int nNum,int Type); //检查dt时间加上对应时间后与当前时间比较，没超过true
   QString makeJsonNode(QString nodeName, QString nodeValue, bool isLast=false, bool bStr=true, bool bDt=false, bool bDtT=false, bool bLow=false);
   //JSON
   bool SetJsonDocument(QByteArray sJsonMsg,QJsonDocument &document,QJsonParseError &jsonError);
   double GetJsonValueInt(QString sNode,QJsonObject ob);
   QString GetJsonValueStr(QString sNode,QJsonObject ob);
   QString mkNode(QString nodeName,QVariant &nodeValue,bool isXmlNode=true);
   QString obj2json(QObject *obj, bool bLow=false);
   QString obj2some(QObject *obj,uchar nType,bool bLow=false);
   bool JsonToObj(QObject *obj,QByteArray sJsonMsg);
   bool JsonToObj(QObject *obj,QJsonObject sJsonObj);
   //
   QString StrPas(char* pbuf);
   QDateTime Now();
   qint64 GetTickCount();
   QString IntToStr(int nNum, int nStrLen=0);
   int StrToIntDef(QString sValue,int ndefault=0);
   float StrToFloatDef(QString sValue,float ndefault);
   QDateTime IncSecond(QDateTime dtTime, int nAddSecs);
   QDateTime IncDay(QDateTime dtTime, int nAddDays=1);
   QString FormatFloat(int nPoint,float fValue);
   int MinuteOfTheDay(QDateTime dtTime);
   int StrToInt(QString str);
   bool TryStrToInt(QString str,int &nVal);
   //
   QString MD5(QString s, bool bUtf8=true);
   int doubleToInt(double d);
   //map与json之间的转换
   QString map2json(QVariantMap data);
   QVariantMap json2map(QString sJson);
   QString list2json(QVariantList aList);
   int json2map(QByteArray buf,QVariantMap &data);
   QString vtos(QVariant d);
   void correctPlate(char *gbkPlate,unsigned char maxBufLen);
   //json格式化
   QString formatJson(QString sJson);
   //获取两个字符串之间的相似性,返回的是相似率.
   double getStrSimilarity(QString str1,QString str2);
};

#endif
