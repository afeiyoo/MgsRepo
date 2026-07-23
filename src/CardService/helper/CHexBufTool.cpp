//---------------------------------------------------------------------------

#include "CHexBufTool.h"
#include "stdio.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

//---------------------------------------------------------------------------

CHexBufTool::CHexBufTool()
{
}

CHexBufTool::~CHexBufTool()
{
}

/*void CHexBufTool::LogCmd(BYTE *cmdBuf,int nLen,bool isGet)
{
   int hexLen = nLen*3+20;
   char *hexCmd= (char *)malloc(hexLen);
   memset(hexCmd,0,hexLen);
   if(isGet) strcpy(hexCmd,"GET: ");
   else strcpy(hexCmd,"PUT: ");
   Buf2Hex(cmdBuf,hexCmd+5,nLen,true);
   m_logger->debug(hexCmd);
   free(hexCmd);
}*/

//------------------16进制与BUF互转函数----------------------------------
void CHexBufTool::Buf2Hex(const BYTE *buf,char *hex,int nBufLen,bool hasBlank)
{
   int nPos = 0,i=0;
   while(i<nBufLen){
      sprintf(hex+nPos,"%02X",buf[i]&0xFF);
      i++; nPos +=2;
      if(hasBlank){
         hex[nPos]=' ';
         nPos++;
      }
   }
}

void CHexBufTool::Hex2Buf(const char *hex,char *buf,int nHexLen,bool hasBlank)
{
   BYTE nStep =2;
   if(hasBlank) nStep =3;
   BYTE nHigh=0, nLow=0;
   for(int i=0;i<nHexLen;i += nStep){
      nHigh = toupper(hex[i]);
      if(nHigh==' ') nHigh = 0;  //容错
      nLow = toupper(hex[i+1]);
      if(nLow==' ') nLow = 0;    //容错

      if(nHigh>0x39) nHigh -=0x37;
      else nHigh -=0x30;
      if(nHigh>15) nHigh = 0;  //容错

      if(nLow>0x39) nLow -= 0x37;
      else nLow -=0x30;
      if(nLow>15) nLow = 0;    //容错too

      buf[i/nStep] = (nHigh<<4) | nLow;
   }
}

char* CHexBufTool::GetStrDate(time_t t,bool hasDiv,bool hasTime)
{
   memset(dstStr,0,513);
   char sFmt[30]={0};
   if(hasTime && hasDiv) strcpy(sFmt,"%Y-%m-%d %H:%M:%S");
   else if(hasTime && !hasDiv) strcpy(sFmt,"%Y%m%d%H%M%S");
   else if(!hasTime && hasDiv) strcpy(sFmt,"%Y-%m-%d");
   else strcpy(sFmt,"%Y%m%d");
   struct tm *tmInfo=localtime(&t);
   strftime(dstStr,80,sFmt,tmInfo);
   return dstStr;
}

//-----------高字节在前数值转换函数---------------------------------
UINT CHexBufTool::Buf2Num(const BYTE *buf,BYTE bufLen)
{
	/*char hexBuf[10]={0},sFmt[10]={0};
    UINT nValue =0; BYTE i=0;
    if(bufLen>4) bufLen = 4;
    for(i=0;i<bufLen;i++) sprintf(hexBuf+i*2,"%02X",buf[i] &0xFF);
    snprintf(sFmt,10,"%%0%dX",bufLen*2);
    sscanf(hexBuf,sFmt,&nValue);
	return nValue;*/
	if(bufLen>4) bufLen = 4;  //2,3,4
    UINT n=0;
    for(BYTE i=0;i<bufLen;i++) n += (buf[i]<<((bufLen-i-1)*8));
	return n;
}

void CHexBufTool::Num2Buf(const UINT num,BYTE *buf,BYTE bufLen)
{
   /*char hexBuf[10]={0},sFmt[10]={0};
   if(bufLen>4) bufLen =4;
   snprintf(sFmt,10,"%%0%dX",bufLen*2);
   snprintf(hexBuf,10,sFmt,num);
   BYTE nTemp =0;
   for(BYTE i=0;i<bufLen*2;i+=2){
      sscanf(hexBuf+i,"%02X",&nTemp);
      buf[i/2] = nTemp;
   }*/
   if(bufLen>4) bufLen =4;  //2,3,4
   for(BYTE i=0;i<bufLen;i++) buf[i] = (num>>((bufLen-i-1)*8));
}
//------------------BCD相关函数--------------------------------------
UINT CHexBufTool::Bcd2Num(UINT bcd)
{
   UINT n=0; BYTE i=0;
   while(bcd>0){
	  n += (bcd%10)<<4*i;
	  i++; bcd= bcd/10;
   }
   return n;
}

//输入13569 输出3501
UINT CHexBufTool::Num2Bcd(const UINT n)
{
   char s[12]={0};
   sprintf(s,"%x",n);
   return strtoul(s,NULL,10);
}

//与Num2Bcd类似
UINT CHexBufTool::Buf2Bcd(const BYTE *buf,BYTE bufLen)
{
   if(bufLen>4) bufLen =4;
   UINT n = 0;
   for(BYTE i=0;i<bufLen;i++) n += (buf[i]<<(bufLen-i-1)*8);
   return Num2Bcd(n);
}

//与Bcd2Num类似
void CHexBufTool::Bcd2Buf(UINT bcd,BYTE *buf,BYTE bufLen)
{
   if(bufLen>4) bufLen =4;
   UINT n = Bcd2Num(bcd);
   Num2Buf(n,buf,bufLen);
}

UINT  CHexBufTool::Buf2Int(const BYTE *buf,BYTE bufLen)
{
	/*char hexBuf[10]={0};
    if(bufLen>4) bufLen = 4;
    for(BYTE i=0;i<bufLen;i++) sprintf(hexBuf+i*2,"%02X",buf[i] & 0xFF);
	return atoi(hexBuf);*/
	return Buf2Bcd(buf,bufLen);
}

void  CHexBufTool::Int2Buf(const UINT num,BYTE *buf,BYTE bufLen)
{
   /*char hexBuf[10]={0},sFmt[10]={0};
   if(bufLen>4) bufLen =4;
   snprintf(sFmt,10,"%%0%dd",bufLen*2);
   snprintf(hexBuf,10,sFmt,num);
   BYTE nTemp =0;
   for(BYTE i=0;i<bufLen*2;i+=2){
      sscanf(hexBuf+i,"%02X",&nTemp);
      buf[i/2] = nTemp;
   }*/
   Bcd2Buf(num,buf,bufLen);
}

//--------------16进制与普通字符串互转函数--------------------------------
void CHexBufTool::Hex2Str(const char *hex,char *str,int hexLen)
{
   if(strlen(hex)<(unsigned int)hexLen) hexLen = strlen(hex);
   int nTemp =0;
   for(int i=0;i<hexLen;i=i+2){
      sscanf(hex+i,"%02X",&nTemp);
      str[i/2] = nTemp;
   }
};

//字符串转化为16进制串，支持汉字(以\0结尾,注意hex长度,防止越界!!)
void CHexBufTool::Str2Hex(const char *str, char *hex,int hexLen)
{
   for(int i=0;i<hexLen/2;i++){
      sprintf(hex+i*2,"%02X",str[i]&0xFF);
   }
};

//数值转化为16进制串(不带0x)
void CHexBufTool::Num2Hex(const UINT n,char *hex,short hexLen)
{
   char f[20]={0};
   snprintf(f,20,"%%0%dX",hexLen);
   sprintf(hex,f,n);
};

//16进制串转化为数值
UINT CHexBufTool::Hex2Num(const char *hex,short hexLen)
{
   if(hexLen ==0) return 0;
   if(hexLen >8) hexLen =8;
   char sFmt[20]={0};
   snprintf(sFmt,20,"%%0%dX",hexLen);
   UINT nTemp=0;
   sscanf(hex,sFmt,&nTemp);
   return nTemp;
};

//---------------结构体与Buffer快速互转函数----------------------------------
//fmt:u.UINT b.BYTE i.int s.short t.USHORT 0..9和A..F:表示字符串及长度(大写)
//fmt:h.2字节BCD码 H.4字节BCD码 w.3字节无符号整型
int CHexBufTool::struct_to_buf(BYTE *buf,const char *fmt,...)
{
   va_list  argp;
   va_start(argp,fmt);
   USHORT nPos =0;
   BYTE b=0; char c=0; char *p=NULL;
   USHORT t=0; short s=0;
   UINT u=0; int i=0;
   bool bHex=false;

   for(USHORT j=0;j<strlen(fmt);j++){
      if(fmt[j]=='X'){ //16进制
          bHex = true;
          continue;
      }
	  switch(fmt[j]){
		case 'i':{
		   i = va_arg(argp,int);
		   Num2Buf(i,buf+nPos,4);
		   nPos +=4;
		   break;
		}
		case 'u':{
		   u = va_arg(argp,UINT);
		   Num2Buf(u,buf+nPos,4);
		   nPos +=4;
		   break;
		}
		case 'H':{
		   u = va_arg(argp,UINT);
		   Bcd2Buf(u,buf+nPos,4);
		   nPos +=4;
		   break;
		}
		case 'w':{
		   u = va_arg(argp,UINT);
		   Num2Buf(u,buf+nPos,3);
		   nPos +=3;
		   break;
		}
        case 'y':{
           u = va_arg(argp,UINT);
           Bcd2Buf(u,buf+nPos,3);
           nPos +=3;
           break;
        }
		case 's':{
           //s = va_arg(argp,short);
          s = (short)va_arg(argp,int);
		   Num2Buf(s,buf+nPos,2);
		   nPos +=2;
		   break;
		}
		case 't':{
           //t = va_arg(argp,USHORT);
          t = (USHORT)va_arg(argp,UINT);
		   Num2Buf(t,buf+nPos,2);
		   nPos +=2;
		   break;
		}
		case 'h':{
           //t = va_arg(argp,USHORT);
          t = (USHORT)va_arg(argp,UINT);
		   Bcd2Buf(t,buf+nPos,2);
		   nPos +=2;
		   break;
		}
		case 'b':{
           //b = (BYTE)va_arg(argp,BYTE);
           b = (BYTE)va_arg(argp,UINT);
		   buf[nPos] = b;
		   nPos++;
		   break;
		}
		case 'c':{
           //c = va_arg(argp,char);
          c = (char)va_arg(argp,int);
		   buf[nPos] = c;
		   nPos++;
		   break;
		}
		default:{
		   if(fmt[j]>'0' && fmt[j]<='9'){
			  p = va_arg(argp,char*);
              if(bHex)
                 Hex2Buf(p,(char*)(buf+nPos),(fmt[j]-'0')*2);
              else
                 memcpy(buf+nPos,p,fmt[j]-'0');
			  nPos += fmt[j]-'0';
		   }
           else if(fmt[j]>='A' && fmt[j]<='G'){
              p = va_arg(argp,char*);
              if(bHex)
                 Hex2Buf(p,(char*)(buf+nPos),(fmt[j]-'A'+10)*2);
              else
                 memcpy(buf+nPos,p,fmt[j]-'A'+10);
			  nPos += fmt[j]-'A'+10;
		   }
           bHex = false;
		   break;
		}
	  }
   }
   va_end(argp);
   return nPos;
}

int CHexBufTool::buf_to_struct(const BYTE *buf,const char *fmt,...)
{
   va_list  argp;
   va_start(argp,fmt);
   USHORT nPos =0;
   BYTE *b=0; char *c=0; char *p=NULL;
   USHORT *t=0; short *s=0;
   UINT *u=0; int *i=0;
   bool bHex=false;

   for(USHORT j=0;j<strlen(fmt);j++){
      if(fmt[j]=='X'){  //16进制字符串
          bHex=true;
          continue;
      }
	  switch(fmt[j]){
		case 'i':{
		   i = va_arg(argp,int*);
		   *i = Buf2Num(buf+nPos,4);
		   nPos +=4;
		   break;
		}
		case 'u':{
		   u = va_arg(argp,UINT*);
		   *u = Buf2Num(buf+nPos,4);
		   nPos +=4;
		   break;
		}
		case 'H':{
		   u = va_arg(argp,UINT*);
		   *u = Buf2Bcd(buf+nPos,4);
		   nPos +=4;
		   break;
		}
		case 'w':{
		   u = va_arg(argp,UINT*);
		   *u = Buf2Num(buf+nPos,3);
		   nPos +=3;
		   break;
		}
        case 'y':{
           u = va_arg(argp,UINT*);
           *u = Buf2Bcd(buf+nPos,3);
           nPos +=3;
           break;
        }
		case 's':{
		   s = va_arg(argp,short*);
		   *s = Buf2Num(buf+nPos,2);
		   nPos +=2;
		   break;
		}
		case 't':{
		   t = va_arg(argp,USHORT*);
		   *t = Buf2Num(buf+nPos,2);
		   nPos +=2;
		   break;
		}
		case 'h':{
		   t = va_arg(argp,USHORT*);
		   *t = Buf2Bcd(buf+nPos,2);
		   nPos +=2;
		   break;
		}
		case 'b':{
		   b = va_arg(argp,BYTE*);
		   *b = buf[nPos];
		   nPos++;
		   break;
		}
		case 'c':{
		   c = va_arg(argp,char*);
		   *c = buf[nPos];
		   nPos++;
		   break;
		}
		default:{
		   if(fmt[j]>'0' && fmt[j]<='9'){
			  p = va_arg(argp,char*);
              if(bHex)
                  Buf2Hex(buf+nPos,p,fmt[j]-'0');
              else
                 memcpy(p,buf+nPos,fmt[j]-'0');
			  nPos += fmt[j]-'0';
		   }
           else if(fmt[j]>='A' && fmt[j]<='G'){
			  p = va_arg(argp,char*);
              if(bHex)
                 Buf2Hex(buf+nPos,p,fmt[j]-'A'+10);
              else
                 memcpy(p,buf+nPos,fmt[j]-'A'+10);
			  nPos += fmt[j]-'A'+10;
		   }
           bHex = false;
		   break;
		}
	  }
   }
   va_end(argp);
   return nPos;
}

//获取hex字符串的xor值(sscanf要用int型)
BYTE CHexBufTool::HexXor(const char *hex,int hexLen)
{
   if(strlen(hex)<(unsigned int)hexLen) hexLen = strlen(hex);
   int nTmp=0,nRes=0;
   sscanf(hex,"%02X",&nRes);
   for(int i=2;i<hexLen;i=i+2){
      sscanf(hex+i,"%02X",&nTmp);
      nRes^=nTmp;
   }
   return nRes;
};

//在hex串后增加xor值
void CHexBufTool::AppendXor(char *hex)
{
  int len=strlen(hex);
  BYTE nValue=HexXor(hex,len);
  sprintf(hex+len,"%02X",nValue);
}


char* CHexBufTool::FillChar(const char *src,char c,int srcLen,int dstLen,BYTE filltype)
{

   memset(dstStr,0,513);
   int i=0;

   if(filltype ==0){    //右补齐
      for(i=0;i<srcLen;i++) dstStr[i]=src[i];
      for(i=srcLen;i<srcLen+dstLen-srcLen;i++) dstStr[i] = c;
   }
   else if(filltype ==1){ //左补齐
      for(i=0;i<dstLen-srcLen;i++) dstStr[i] = c;
      for(i=0;i<srcLen;i++) dstStr[i+dstLen-srcLen]= src[i];
   }
   else{   //居中补齐   c1234cc
      int n = (dstLen-srcLen)/2;
      for (i=0;i<n;i++) dstStr[i] = c;
      for (i=n+srcLen;i<dstLen;i++) dstStr[i] = c;
      for (i=0;i<srcLen;i++) dstStr[i+n] = src[i];
   }

   return dstStr;
}

char* CHexBufTool::FillCharRight(const char *src,char c,int srcLen,int dstLen)
{
   return FillChar(src,c,srcLen,dstLen,0);
}

char* CHexBufTool::FillCharLeft(const char *src,char c,int srcLen,int dstLen)
{
   return FillChar(src,c,srcLen,dstLen,1);
}

char* CHexBufTool::FillCharRL(const char *src,char c,int srcLen,int dstLen)
{
   return FillChar(src,c,srcLen,dstLen,2);
}

USHORT CHexBufTool::calc_crc16(int init_crc, BYTE *crc_data, int len)
{
    int crc = init_crc, cnt, k;
    for(cnt = 0; cnt < len; cnt++)
    {//loop data length
        crc ^= crc_data[cnt];
        for(k = 0; k < 8; k++)
        {//every bit data
            if(crc & 0x1)
            {
                crc = (crc >> 1) ^ CRC_POLYNOM;
            }
            else
            {
                crc = (crc >> 1);
            }
        }
    }
    crc = (~crc) & 0xFFFF;
    return (USHORT)crc;
}
