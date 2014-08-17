#ifndef __OS_API_H__
#define __OS_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#define LIB_DATABASE_EXPORT	__declspec(dllexport)

void LIB_DATABASE_EXPORT CaDefaultAnsi2UnicodeLen(char*src,int len,unsigned short*tar);
void LIB_DATABASE_EXPORT CaDefaultAnsi2Unicode(char* src,unsigned short*tar);
void LIB_DATABASE_EXPORT CaUnicode2DefaultAnsi(unsigned short*src,char*tar);
void LIB_DATABASE_EXPORT CaUnicode2DefaultAnsiLen(unsigned short* src,int len,char*tar);
void LIB_DATABASE_EXPORT CaUnicode2Utf8Len(unsigned short* src,int len,char*tar);
void LIB_DATABASE_EXPORT CaUnicode2Utf8(unsigned short* src,char*tar);
void LIB_DATABASE_EXPORT CaUtf8ToUnicodeLen(char*src,int len,unsigned short*tar);
void LIB_DATABASE_EXPORT CaUtf8ToUnicode(char*src,unsigned short*tar);
void LIB_DATABASE_EXPORT CaGetDatabaseFilePrefix(char *out_put_database_file_prefix);
void LIB_DATABASE_EXPORT *CaGetTempDataBuffer();
void LIB_DATABASE_EXPORT *CaGetTempDataBuffer2();

#ifdef __cplusplus
}
#endif

#endif