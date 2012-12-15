#ifndef MD5_H
#define MD5_H

#include <stdio.h>
#include <QString>

typedef unsigned int UINT4;
typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef DWORD ULONG;
typedef unsigned char BYTE;
typedef BYTE UCHAR;

#define MD5_INIT_STATE_0 0x67452301
#define MD5_INIT_STATE_1 0xefcdab89
#define MD5_INIT_STATE_2 0x98badcfe
#define MD5_INIT_STATE_3 0x10325476

void MD5Init(void);
void MD5Update(unsigned char *bug, unsigned int len);
void MD5Final(char* cReturnStr);
void Transform(UINT4 *buf, UINT4 *in);
void GetMD5(char* pBuf, UINT nLength,char* cReturnStr);
QString MD5(QString in);

#endif
