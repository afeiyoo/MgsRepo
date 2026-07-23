//---------------------------------------------------------------------------
//Module: QT通用工具，Author:Huanglr  FromDate:2014-05-06
//Remark:
//---------------------------------------------------------------------------

#include "CommonTool.h"
#include <QDir>
#include <QtCore>
#include <algorithm>
//---------------------------------------------------------------------------

#define MAX_TEMP_BUFLEN  512

CommonTool::CommonTool()
{
    m_ini_file=NULL;
    m_utf = QTextCodec::codecForName("UTF-8");
    //QTextCodec::setCodecForLocale(utf8);
    //QTextCodec::setCodecForCStrings(utf8);
//    m_gbk = QTextCodec::codecForName("gb2312");//gb2312可显示中文不多，GBK包含了gb2312和其他中文
    m_gbk = QTextCodec::codecForName("GBK");
}

//--------------读写配置文件-------------------------------------------------
void  CommonTool::OpenIniFile(QString ini_filename,int nType)
{
   if( !QDir().exists(ini_filename)) return;
   if( m_ini_file!=NULL){
       delete m_ini_file;
       m_ini_file = NULL;
   }
   m_ini_file=new QSettings(ini_filename,QSettings::IniFormat);
   if(nType == 1) m_ini_file->setIniCodec("GBK");
   else m_ini_file->setIniCodec("UTF8");
}

void  CommonTool::CloseIniFile()
{
  if (m_ini_file==NULL) return;
  //delete m_ini_file;
}

int  CommonTool::IniReadInt(QString section,QString key,int def)
{
   if( m_ini_file==NULL ) return def;
   QString tmp = IniReadStr(section,key,QString::number(def));
   if(tmp.isEmpty()) return def;
   return tmp.toInt();
}

QString  CommonTool::IniReadStr(QString section,QString key,QString def)
{
   if( m_ini_file == NULL ) return def;
   QString tmp = m_ini_file->value(section+"/"+key,QVariant(def)).toString();
   if(tmp.isEmpty()) return def;
   int nPos = tmp.indexOf("#");
   if( nPos>=0) tmp = tmp.left(nPos);
   return tmp.trimmed();
}

void  CommonTool::IniWriteInt(QString section,QString key,int Value)
{
  if( m_ini_file==NULL ) return;
  m_ini_file->setValue(section+"/"+key,QVariant(Value));
}
   
void  CommonTool::IniWriteStr(QString section,QString key,QString Value)
{
   if(m_ini_file==NULL) return;
   m_ini_file->setValue(section+"/"+key,QVariant(Value));
}

// 识别中文代码
QString CommonTool::knowChinese(const QByteArray &ba)
{
    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString text = codec->toUnicode( ba.constData(), ba.size(), &state);
    if (state.invalidChars > 0)
    {
        text = QTextCodec::codecForName("GBK")->toUnicode(ba);
    }
    else
    {
        text = ba;
    }

    return text;
}


//----------一般类工具--------------------------------------------------------
int  CommonTool::posIC(const QString str,const QString sub)
{
  return str.toUpper().indexOf(sub.toUpper());
}

int  CommonTool::StrPosEx(QString src,QString chd,QString div)
{
   src = div+src+div;
   chd = div+chd+div;
   return posIC(src,chd);
}

QString  CommonTool::GetPrintHex(char *hex)
{
    char tmp[MAX_TEMP_BUFLEN]={0};
    int j=0;
    for(unsigned int i=0;i<strlen(hex);i++)
    {
       if(j>MAX_TEMP_BUFLEN-2) break;
       if(i%2==0) {
          tmp[j++]=' ';
       }
       tmp[j++] = hex[i];
    }
    return QString(tmp);
}

QString  CommonTool::makeXmlNode(const QString field,QString value)
{
   if(value.isEmpty() || value.length()==0) return "<"+field+"/>";
   else return "<"+field+">"+value+"</"+field+">";
}

QDateTime  CommonTool::StrToDateTimeX(QString v)
{
  v.replace("{","");
  v.replace("}","");
  v=v.trimmed();
  if(v.indexOf(":")<0) v += " 00:00:00";
  return QDateTime::fromString(v,"yyyy-MM-dd HH:mm:ss");
}

QString CommonTool::FmtStr(char *fmt,...)
{
   char str[1024]={0};
   try{
      va_list  argp;
      va_start(argp,fmt);
      vsnprintf(str,1024,fmt,argp);
      va_end(argp);
   }
   catch(...){
   }
   return QString(str);
}

QString CommonTool::gbk_to_utf(const char *s,int nLen)
{
   QString sUnicode;
   if(nLen>0){
       char *sTemp=(char*)malloc(nLen+1);
       memcpy(sTemp,s,nLen);
       sUnicode = m_gbk->toUnicode(sTemp);
       free(sTemp);
   }
   else sUnicode = m_gbk->toUnicode(s);
   return m_utf->fromUnicode(sUnicode);
}

QByteArray CommonTool::utf_to_gbk(const char *s,int nLen)
{
    QString sUnicode;
    if(nLen>0){
        char *sTemp=(char*)malloc(nLen+1);
        memcpy(sTemp,s,nLen);
        sUnicode = m_utf->toUnicode(sTemp);
        free(sTemp);
    }
    else sUnicode = m_utf->toUnicode(s);
    return m_gbk->fromUnicode(sUnicode);

}

//-----------注意:qt将时间转为unix时间时不加时区，应加上8*3600------------------------------------------------------------
unsigned char CommonTool::dec2bcd(unsigned char dec){
   //return (dec>>4)*10+(dec&0xf);
   return dec/16*10+dec%16;
}

QDateTime  CommonTool::Str2DateTime(QString sDate)
{
   return QDateTime::fromString(sDate,"yyyyMMddHHmmss");
}

int  CommonTool::Str2DosTime(QString sDate)
{
   //DOS时间是从1980-1-1算起,315532800是1970-1-1到1980-1-1流逝的时间
   QDateTime dt=Str2DateTime(sDate);
   //return DateTimeToUnix(dt) - 315532800;
   return dt.toTime_t() - 315532800;
}

int  CommonTool::Str2UnixTime(QString sDate)
{
   QDateTime dt=Str2DateTime(sDate);
   return dt.toTime_t();
}

//本地时间，不考虑时区，315532800为1970-1-1到1980-1-1流逝的秒数
//dos时间考虑了时区，而unix时间则不考虑。
int  CommonTool::unix2dos_time(time_t t)
{
   return t-315532800 +8*3600;
}

//不包含时区
int  CommonTool::dos2unix_time(int t)
{
   //315504000 = 315532800 - 8*3600
   return t+315532800 - 8*3600;
}

QDateTime  CommonTool::dos2datetime(int t)
{
   int tUnix = dos2unix_time(t)+8*3600;  //转化为本地时间要加时区(东8区)
   //return (tUnix/86400.0)+25569.0;       //25569.0为1970-01-01,86400表示24小时
   return QDateTime::fromTime_t(tUnix);
}

QDateTime  CommonTool::unix2datetime(int t,bool toLocal)
{
//   if(toLocal) t = t+8*3600;
 //return (t/86400.0)+25569.0;
   QDateTime dt = QDateTime::fromTime_t(t);
   if(!toLocal) {
      dt.setTimeSpec(Qt::UTC);
      dt = QDateTime::fromTime_t(t);
   }
   return dt;  //fromTime_t  toTime_t() 默认就是本地时间跟unix时间互转
}

char*  CommonTool::GetpStrDate(time_t t,bool hasDiv,bool hasTime)
{
   char dtStr[50];
   memset(dtStr,0,50);
   char sFmt[30]={0};
   if(hasTime && hasDiv) strcpy(sFmt,"%Y-%m-%d %H:%M:%S");
   else if(hasTime && !hasDiv) strcpy(sFmt,"%Y%m%d%H%M%S");
   else if(!hasTime && hasDiv) strcpy(sFmt,"%Y-%m-%d");
   else strcpy(sFmt,"%Y%m%d");
   struct tm *tmInfo=localtime(&t);
   if(strftime(dtStr,50,sFmt,tmInfo)==0) strcpy(dtStr,"00000000000000");
   return dtStr;
}

QString  CommonTool::GetStrDate(time_t t,bool hasDiv,bool hasTime)
{
   char dtStr[50];
   memset(dtStr,0,50);
   char sFmt[30]={0};
   if(hasTime && hasDiv) strcpy(sFmt,"%Y-%m-%d %H:%M:%S");
   else if(hasTime && !hasDiv) strcpy(sFmt,"%Y%m%d%H%M%S");
   else if(!hasTime && hasDiv) strcpy(sFmt,"%Y-%m-%d");
   else strcpy(sFmt,"%Y%m%d");
   struct tm *tmInfo=localtime(&t);
   if(strftime(dtStr,50,sFmt,tmInfo)==0) strcpy(dtStr,"00000000000000");

   return QString::fromUtf8(dtStr);
}

uint CommonTool::getCurUnixTime()
{
//    return time_t()+3600*8;  //加上东八区本地时间
    return QDateTime::currentDateTime().toTime_t();
}

void   CommonTool::GetDirFiles(QString sPath,QString sExt,QStringList *fileList,int nMaxFiles)
{
   //清空
   fileList->clear();
   //检测目录
   QDir dir(sPath);
   if(!dir.exists()) return;
   QStringList filterList;
   filterList<<sExt;
   dir.setFilter(QDir::Files | QDir::NoSymLinks);
   dir.setNameFilters(filterList);
   //生成列表
   int nCount = dir.count();
   if(nCount ==0) return;
   if(nMaxFiles>0 && nCount>nMaxFiles)  nCount = nMaxFiles;
   for(int i=0;i<nCount;i++) fileList->append(sPath+"/"+dir[i]);
}

QStringList CommonTool::GetDirFiles(QString sPath,QString sExt)
{
    QDir dir(sPath);
    QStringList nameFilters;
    nameFilters << sExt;
    return dir.entryList(nameFilters, QDir::Files|QDir::Readable| QDir::NoSymLinks, QDir::Name);
}

void CommonTool::RemoveHisFiles(QString sPath,QString sExt,int days)
{
    QDateTime dt = QDateTime::currentDateTime().addDays(0-days);
    QStringList files = GetDirFiles(sPath,sExt);
    if(files.count()==0) return ;
    QFileInfo fi;   //注意不能用fi();
    QString aFile;
    QDir dir(sPath);
    for(int i=0;i<files.count();i++){
        aFile = files.at(i);
        fi.setFile(dir,aFile);
        if(fi.lastModified()<dt) dir.remove(sPath+"/"+aFile);
    }
}
 void CommonTool::CreateDirIfNotExist(QString qstrDirPath)
 {
     QDir folder;
     bool exist = folder.exists(qstrDirPath);
     if(!exist){
         bool ok = folder.mkdir(qstrDirPath);
         if(ok) qDebug("mkdir:%s success\n",qstrDirPath.toStdString().c_str());
         else qDebug("mkdir:%s error\n",qstrDirPath.toStdString().c_str());
     }
     else{
         qDebug("%s exists\n",qstrDirPath.toStdString().c_str());
     }
 }
//-------------------------20220606.huanglr.append======================//
QString CommonTool::createPathEx(QString path)
{
     qDebug()<<path;
     QDir dir(path);
     if(dir.exists(path)) return path;
     QString parentDir = createPathEx(path.mid(0, path.lastIndexOf('/')));
     QString dirName = path.mid(path.lastIndexOf('/') + 1);
     QDir parentPath(parentDir);
     if (!dirName.isEmpty()) parentPath.mkpath(dirName);
     return parentDir + "/" + dirName;
}

bool CommonTool::removePathAndFile(QString path)
{
    if(path.isEmpty()) return false;
    QDir dir(path);
    if(!dir.exists()) return false;
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    //遍历文件信息
    foreach(QFileInfo file, fileList){
        if (file.isFile()){
            file.dir().remove(file.fileName());  // 是文件，删除
        }
        else{
            removePathAndFile(file.absoluteFilePath());  // 递归删除
            file.dir().rmdir(file.absoluteFilePath());
        }
    }
    return true;
}

void CommonTool::removeHisDatePath(QString parentPath, int days, bool isShortDate)
{
    QDateTime dt = QDateTime::currentDateTime().addDays(0-days);
    int maxDate = dt.toString("yyyyMMdd").toInt();
    //目录下所有子目录
    QDir dir(parentPath);
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList();
    QString sDate="";
    foreach(QFileInfo file, fileList){
        if(isShortDate){
            sDate = QString::number(QDate::currentDate().year()).left(2);
        }
        sDate = sDate+file.fileName();
        if(file.isDir() && sDate.toInt()<maxDate){
           removePathAndFile(file.absoluteFilePath());
        }
    }
}

bool CommonTool::checkFileExist(QString fullFileName)
{
   QFileInfo file(fullFileName);
   return file.isFile();
}

bool CommonTool::checkPathExist(QString fullPathName)
{
    QFileInfo file(fullPathName);
    return file.isDir();
}

//====================End.==============================================//

QString CommonTool::GetSubString(QString sInfo,int len) //一个中文按2个字节来算
{
    BYTE buff[50] = {0};
    BYTE temp[50] = {0};int tempIndex=0;
    int nLen = len;
    memcpy(buff,sInfo.toStdString().c_str(),sInfo.toStdString().length());
    for (int i=0;i<nLen;i++){    //一个中文按3个字节来算
        if ((buff[i]&0x80)>>7){//判断最高位是否为1
            if(i==nLen || (i+2)>=nLen) break;
            temp[tempIndex]=buff[i];//取3个字符
            tempIndex++;
            temp[tempIndex]=buff[i+1];//取3个字符
            tempIndex++;
            temp[tempIndex]=buff[i+2];//取3个字符
            tempIndex++;
            i+=2;
            nLen++;
        }else{
            temp[tempIndex]=buff[i];//否则只取1个
            tempIndex++;
        }
    }
    QString tmp = QString("%1").arg((char*)temp);
    return tmp;
}


QString CommonTool::GetAppDir()
{
   return QCoreApplication::applicationDirPath()+"/";
}

bool CommonTool::CheckNoOverNowEx(QDateTime dt,int nMilliSec)
{
   try{
      if(dt.addMSecs(nMilliSec)<QDateTime::currentDateTime()){
         return true;
      }
      return false;
   }catch(...){
      return false;
   }
}

bool CommonTool::CheckOverNowEx(QDateTime dt,int nNum,int Type)
{
   try{
      QDateTime dt2 = QDateTime::currentDateTime();
      QDateTime nm;
      if(Type == 0) nm = dt.addMSecs(nNum);
      else if(Type == 1) nm = dt.addSecs(nNum);
      else if(Type == 4) nm = dt.addDays(nNum);
      else if(Type == 5) nm = dt.addMonths(nNum);
      else if(Type == 6) nm = dt.addYears(nNum);
      if(dt2>nm) return true;
      else return false;
   }catch(...){
      return false;
   }
}

QString CommonTool::makeJsonNode(QString nodeName, QString nodeValue, bool isLast, bool bStr, bool bDt, bool bDtT,bool bLow)
{
    QString sValue = "\""+nodeValue+"\"";
    if(!bStr) sValue =  nodeValue;
    if(bDt){
        if(!bDtT) sValue.replace("T"," ");
    }
    if(bLow){
       if(isLast) return "\""+nodeName.toLower()+"\":"+sValue;
       return "\""+nodeName.toLower()+"\":"+sValue+",";
    }else{
        if(isLast) return "\""+nodeName+"\":"+sValue;
        return "\""+nodeName+"\":"+sValue+",";
    }
}

bool CommonTool::SetJsonDocument(QByteArray sJsonMsg,QJsonDocument &document,QJsonParseError &jsonError)
{
    document = QJsonDocument::fromJson(sJsonMsg, &jsonError); //传化为Json文档
    if (!document.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
      if (document.isObject()){
          return true;
      }
    }
    return false;

}

double CommonTool::GetJsonValueInt(QString sNode,QJsonObject ob)
{
    if (ob.contains(sNode)){
       QJsonValue value = ob.value(sNode);
       if (value.isDouble()){
            return value.toDouble();
       }
    }
    return -1;
}

QString CommonTool::GetJsonValueStr(QString sNode,QJsonObject ob)
{
    if (ob.contains(sNode)){
       QJsonValue value = ob.value(sNode);
       if (value.isString()){
          return value.toString();
       }
    }
    return "";

}


QString CommonTool::mkNode(QString nodeName,QVariant &nodeValue,bool isXmlNode)
{
   QString sValue="";
   bool bDate = (nodeValue.type()==QVariant::Date) || (nodeValue.type()==QVariant::DateTime);

   if(nodeValue.type()==QVariant::Date) sValue = nodeValue.toDate().toString("yyyy-MM-dd");
   else if(nodeValue.type()==QVariant::DateTime) sValue = nodeValue.toDateTime().toString("yyyy-MM-dd HH:mm:ss");
   else sValue = nodeValue.toString();

   if(isXmlNode && bDate) sValue = "{"+sValue+"}";
   else if(!isXmlNode && (bDate || nodeValue.type()==QVariant::String)) sValue = "\""+sValue+"\"";
   if(isXmlNode) return "<"+nodeName.toLower()+">"+sValue+"</"+nodeName.toLower()+">";
   else return "\""+nodeName.toLower()+"\":"+sValue+";";
}

QString CommonTool::obj2some(QObject *obj, uchar nType, bool bLow)
{
    QMetaProperty prp;
    QVariant val;
    QString sData="",colName="";
    for(int i=0;i<obj->metaObject()->propertyCount();i++ ){
       prp = obj->metaObject()->property(i);
       colName = QString(prp.name());
       if(colName == "objectName" || colName == "tbl_pk" || colName == "auto_inc") continue;
       val = obj->property(prp.name());
       if(nType==0) sData += mkNode(colName,val,true);
       else sData += makeJsonNode(colName,val.toString(),(i==(obj->metaObject()->propertyCount()-1)?true:false),((prp.type()==QMetaType::QString || prp.type()==QMetaType::QDateTime)?true:false),prp.type()==QMetaType::QDateTime,false,bLow);

    }
    if(nType ==0) return "<recode>"+sData+"</recode>";
    else return "{"+sData+"}";
}

QString CommonTool::obj2json(QObject *obj,bool bLow)
{
   return obj2some(obj,1,bLow);
}

bool CommonTool::JsonToObj(QObject *obj,QByteArray sJsonMsg)
{
//    qDebug()<<sJsonMsg;
    QJsonDocument document;
    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(sJsonMsg, &jsonError); //传化为Json文档
    if (document.isNull() || (jsonError.error != QJsonParseError::NoError)){
//       qDebug()<<"isNull "<<jsonError.errorString();
       return false;
    }

    if (!document.isObject()){
//         qDebug()<<"isObject";
       return false;
    }

    QMetaProperty prp;
    QJsonValue value;
    QString colName="";
    for(int i=0;i<obj->metaObject()->propertyCount();i++ ){
       prp = obj->metaObject()->property(i);
       colName = QString(prp.name());
       if(colName == "objectName" || colName == "tbl_pk" || colName == "auto_inc") continue;
       if (document.object().contains(colName)){
          value = document.object().value(colName);
//          qDebug()<<"col:"<<colName<<" Value:"<<value;
          obj->setProperty(prp.name(),value);
       }
    }
    return true;
}

bool CommonTool::JsonToObj(QObject *obj,QJsonObject sJsonObj)
{
    QMetaProperty prp;
    QJsonValue value;
    QString colName="";
    for(int i=0;i<obj->metaObject()->propertyCount();i++ ){
       prp = obj->metaObject()->property(i);
       colName = QString(prp.name());
       if(colName == "objectName" || colName == "tbl_pk" || colName == "auto_inc") continue;
       if (sJsonObj.contains(colName)){
          value = sJsonObj.value(colName);
          qDebug()<<"col:"<<colName<<" Value:"<<value;
          obj->setProperty(prp.name(),value);
       }
    }
    return true;

}


QString CommonTool::StrPas(char *pbuf) { return QString::fromUtf8(pbuf); }

QDateTime CommonTool::Now() { return QDateTime::currentDateTime(); }

qint64 CommonTool::GetTickCount() {
  return QDateTime::currentDateTime().toMSecsSinceEpoch();
}

QString CommonTool::IntToStr(int nNum ,int nStrLen)
{
   if(nStrLen == 0) return QString::number(nNum);
   QString sTemp = QString("").sprintf("%0%dd",nStrLen);
   return QString("").sprintf(sTemp.toStdString().c_str(),nNum);
}

int CommonTool::StrToIntDef(QString sValue, int ndefault) {
  bool ok;
  int nRet = sValue.toInt(&ok);
  if (ok)
    return nRet;
  else
    return ndefault;
}

bool CommonTool::TryStrToInt(QString str,int &nVal)
{
   bool ok;
   nVal = str.toInt(&ok);
   return ok;
}

float CommonTool::StrToFloatDef(QString sValue, float ndefault) {
  bool ok;
  float nRet = sValue.toFloat(&ok);
  if (ok)
    return nRet;
  else
    return ndefault;
}

QDateTime CommonTool::IncSecond(QDateTime dtTime, int nAddSecs) {
  return dtTime.addSecs(nAddSecs);
}

QDateTime CommonTool::IncDay(QDateTime dtTime, int nAddDays) {
  return dtTime.addDays(nAddDays);
}

QString CommonTool::FormatFloat(int nPoint, float fValue) {
  return QString::number(fValue, 'f', nPoint);
}

int CommonTool::MinuteOfTheDay(QDateTime dtTime) {
  return (dtTime.time().hour() * 60 + dtTime.time().minute());
}

int CommonTool::StrToInt(QString str){
    return str.toInt();

}

QString CommonTool::MD5(QString s,bool bUtf8)
{
  QString ret = "";
  QByteArray value;
  if(bUtf8) value = QCryptographicHash::hash(s.toUtf8(), QCryptographicHash::Md5);
  //toLocal8Bit 转成系统编码
  else value = QCryptographicHash::hash(s.toLocal8Bit(), QCryptographicHash::Md5);
  ret = value.toHex().data();
  return ret.toUpper();
}

int CommonTool::doubleToInt(double d)
{
    double intPart = floor(d);
    if ((d - intPart) >= (double)0.5)
    {
        return (intPart + 1);
    }
    else
    {
        return intPart;
    }
}

QString CommonTool::map2json(QVariantMap data)
{
    QString sJson="{\"this-json\":1";
    QVariantMap::const_iterator it = data.constBegin();
    while(it!=data.constEnd()){
        if(it.value().type()==QVariant::String)
           sJson += ",\""+it.key()+"\":\""+it.value().toString()+"\"";
        else if(it.value().type()==QVariant::Double)
            sJson += ",\""+it.key()+"\":"+QString::number(it.value().toDouble());
        else if(it.value().type()==QVariant::Date)
            sJson += ",\""+it.key()+"\":\""+it.value().toDate().toString("yyyy-MM-dd")+"\"";
        else if(it.value().type()==QVariant::DateTime)
            sJson += ",\""+it.key()+"\":\""+it.value().toDateTime().toString("yyyy-MM-dd hh:mm:ss")+"\"";
        else if(it.value().type()==QVariant::Int || it.value().type()==QVariant::UInt)
           sJson += ",\""+it.key()+"\":"+QString::number(it.value().toInt());
        else if(it.value().type() == QVariant::Map ){
           sJson += ",\""+it.key()+"\":"+map2json(it.value().toMap());
        }
        else if(it.value().type() == QVariant::List){
           QVariantList valuelist = it.value().toList();
           sJson += ", \""+it.key()+"\":";
           sJson +="[";
           QVariantList::const_iterator it2 = valuelist.constBegin();
           for(;it2!=valuelist.constEnd();it2++)
           {
               if(it2!=valuelist.constBegin())  sJson +=",";

               if(it2->type()==QVariant::String)
                  sJson += "\""+it2->toString()+"\"";
               else if(it2->type()==QVariant::Double)
                   sJson += QString::number(it2->toDouble());
               else if(it2->type()==QVariant::Date)
                   sJson += "\""+it2->toDate().toString("yyyy-MM-dd")+"\"";
               else if(it2->type()==QVariant::DateTime)
                   sJson += "\""+it2->toDateTime().toString("yyyy-MM-dd hh:mm:ss")+"\"";
               else if(it2->type()==QVariant::Int || it2->type()==QVariant::UInt)
                  sJson += QString::number(it2->toInt());
               else if(it2->type() == QVariant::Map){
                  sJson += map2json(it2->toMap());
               }

           }

           sJson +="]";
        }

        ++it;
    }
    sJson +="}";
    return sJson;
}

QString CommonTool::list2json(QVariantList aList)
{
    QVariantMap aMap;
    QString sJson="";
    for(int i=0;i<aList.count();i++){
        aMap = aList.at(i).toMap();
        if(i==0) sJson = map2json(aMap);
        else sJson +=","+map2json(aMap);
    }
    return "["+sJson+"]";
}

QVariantMap CommonTool::json2map(QString sJson)
{
    QVariantMap aMap;
    QJsonParseError err_rpt;
    QJsonDocument  doc = QJsonDocument::fromJson(sJson.toUtf8(), &err_rpt);
    if(err_rpt.error != QJsonParseError::NoError) return aMap;
    else return doc.object().toVariantMap();
}

int CommonTool::json2map(QByteArray buf,QVariantMap &data)
{
    data.clear();
    QJsonParseError err_rpt;
    QJsonDocument  doc = QJsonDocument::fromJson(buf, &err_rpt);
    if(err_rpt.error == QJsonParseError::NoError){
        data = doc.object().toVariantMap();
        return 0;
    }
    else return -1;

}

QString CommonTool::vtos(QVariant d)
{
    if(d.type()==QVariant::String)
        return d.toString();
    else if(d.type()==QVariant::Double)
        return QString::number(d.toDouble());
    else if(d.type()==QVariant::Date)
        return d.toDate().toString("yyyy-MM-dd");
    else if(d.type()==QVariant::DateTime)
        return d.toDateTime().toString("yyyy-MM-dd hh:mm:ss");
    else if(d.type()==QVariant::Int || d.type()==QVariant::UInt)
       return QString::number(d.toInt());
    else return "";  //其它不解析
}

void CommonTool::correctPlate(char *gbkPlate,unsigned char maxBufLen)
{
   //gbkPlate如://蓝粤B12345
   unsigned char nLen = maxBufLen;
   if(nLen>14) nLen =14;
   unsigned char idx =0;
   for(idx=0;idx<nLen;idx++){
       if(gbkPlate[idx] & 0x80) continue;
       else if(gbkPlate[idx]>='0' && gbkPlate[idx]<='9') continue;
       else if(gbkPlate[idx]>='A' && gbkPlate[idx]<='Z') continue;
       else break;
   }
   char sTemp[50]={0};
   strncpy(sTemp,gbkPlate,idx+1);
   memset(gbkPlate,0,maxBufLen);
   strncpy(gbkPlate,sTemp,idx+1);
}


/**
*    @brief:   格式化json字符串
*    @param:   未格式化json字符串
*    @returns: 返回格式化后的json
*    @author:  SheMinglei
*/
QString CommonTool::formatJson(QString sJson)
{
    QString res = "";
    int level = 0;
    auto getLevelStr = [](int level)->QString{
        QString temp = "";
        for (int i = 0; i < level; ++i) {
            temp += "    ";
        }
        return temp;
    };
    for(int i = 0; i < sJson.size(); i++){
        if(level > 0 && QChar('\n') == sJson[sJson.size() - 1])
            res += getLevelStr(level);
        if(sJson[i] == QChar('{') || sJson[i] == QChar('[')){
            res += sJson[i] + "\n";
            level++;
            res += getLevelStr(level);
        }
        else if(sJson[i] == QChar(',')){
            res += sJson[i] + "\n";
//            level++;
            res += getLevelStr(level);
        }
        else if(sJson[i] == QChar('}') || sJson[i] == QChar(']')){
            res += "\n";
            level--;
            res += getLevelStr(level);
            res += sJson[i];
        }
        else{
            res += sJson[i];
        }
    }
    return res;
}

double CommonTool::getStrSimilarity(QString str1, QString str2)
{
    int n = str1.size();
    int m = str2.size();
    if(n == 0 || m == 0)
        return 0.0;
    QVector<QVector<int>>dp(n + 1, QVector<int>(m + 1, 0));
    dp[0][0] = 0;//dp[x][y]代表将a字符串前x个字符修改成b字符串前y个字符
    //初始化dp
    for (int i = 1; i <= m; ++i)
        dp[0][i] = i;
    for (int i = 1; i <= n; ++i)
        dp[i][0] = i;
    //求str1和str2之间的相似性
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= m; ++j)
        {
            int one = dp[i - 1][j] + 1, two = dp[i][j - 1] + 1, three = dp[i - 1][j - 1];
            if (str1[i - 1] != str2[j - 1])
                three += 1;
            dp[i][j] = std::min(std::min(one, two), three);
        }
    }
    double dif = dp[n][m] - std::abs(n-m); //去除长度不一致部分
    return 1.00 - dif/double(std::min(n,m));
}

