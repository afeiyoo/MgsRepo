//---------------------------------------------------------------------------
#ifndef CHexBufToolH
#define CHexBufToolH
//---------------------------------------------------------------------------
#include "time.h"

typedef unsigned char BYTE;
typedef unsigned int  UINT;
typedef unsigned short USHORT;
#define CRC_POLYNOM     0x8408

class CHexBufTool{
private:
   char dstStr[513];
public:
   CHexBufTool();
   ~CHexBufTool();
   //void  LogCmd(BYTE *cmdBuf,int nLen,bool isGet);
   char*  GetStrDate(time_t t,bool hasDiv,bool hasTime);
   BYTE HexXor(const char *hex,int hexLen);  //获取hex字符串的xor值
   void AppendXor(char *hex);   //在hex串后增加xor值
   char* FillChar(const char *src,char c,int srcLen,int dstLen,BYTE filltype);
   char* FillCharRight(const char *src,char c,int srcLen,int dstLen);   //右对齐(最大512,下同)
   char* FillCharLeft(const char *src,char c,int srcLen,int dstLen);    //左对齐
   char* FillCharRL(const char *src,char c,int srcLen,int dstLen);      //左右对齐
public:
   //16进制字符串与Buffer互转
   void  Buf2Hex(const BYTE *buf,char *hex,int nBufLen,bool hasBlank=false);
   void  Hex2Buf(const char *hex,char *buf,int nHexLen,bool hasBlank=false);
public:
   //高位在前低位在后数值与Buffer互转
   UINT  Buf2Num(const BYTE *buf,BYTE bufLen);
   void  Num2Buf(const UINT num,BYTE *buf,BYTE bufLen);
public:
   //BCD码与BUF互转函数.例如:对于bcd=3501的bcd码数值,写入的是0x3501
   UINT  Bcd2Num(UINT bcd);
   UINT  Num2Bcd(const UINT n);
   UINT  Buf2Bcd(const BYTE *buf,BYTE bufLen);
   void  Bcd2Buf(UINT bcd,BYTE *buf,BYTE bufLen);
   UINT  Buf2Int(const BYTE *buf,BYTE bufLen);
   void  Int2Buf(const UINT num,BYTE *buf,BYTE bufLen);
public:
   //16进制串字符串与普通字符串互转，支持汉字
   void  Str2Hex(const char *str, char *hex,int hexLen);
   void  Hex2Str(const char *hex,char *str,int hexLen);
   //16进制串字符串与数值互转
   void  Num2Hex(const UINT n,char *hex,short hexLen);
   UINT  Hex2Num(const char *hex,short hexLen);
public:
   //结构体与Buffer快速互转函数
   //fmt:u.UINT b.BYTE i.int s.short t.USHORT 0..9和A..F:表示字符串及长度(大写)
   //fmt:h.2字节BCD码 H.4字节BCD码 w.3字节无符号整型; X.16进制字符串,后跟buffer长度，如X8
   int struct_to_buf(BYTE *buf,const char *fmt,...);
   int buf_to_struct(const BYTE *buf,const char *fmt,...);
   USHORT calc_crc16(int init_crc, BYTE *crc_data, int len);
};

#endif
