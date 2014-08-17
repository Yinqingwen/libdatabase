#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <windows.h>

#include "common_api.h"

#define MAX_TEMP_BUFFER	2048

static unsigned char temp_buffer[MAX_TEMP_BUFFER];
static unsigned char temp_buffer2[MAX_TEMP_BUFFER];

void LIB_DATABASE_EXPORT *CaGetTempDataBuffer()
{
	return (void*)temp_buffer;
}

void LIB_DATABASE_EXPORT *CaGetTempDataBuffer2()
{
	return (void*)temp_buffer2;
}

void LIB_DATABASE_EXPORT CaGetDatabaseFilePrefix(char *out_put_database_file_prefix)
{
	unsigned short str_buffer[512];
	char *path_pos=NULL;
	SECURITY_ATTRIBUTES attribute;

	GetModuleFileName(NULL,(unsigned short*)str_buffer,512);
	CaUnicode2DefaultAnsi((unsigned short*)str_buffer,out_put_database_file_prefix);
	path_pos=strrchr(out_put_database_file_prefix,'\\');
	*path_pos=0;

	strcat(out_put_database_file_prefix,"\\database\\");

	attribute.nLength = sizeof(attribute);
	attribute.lpSecurityDescriptor = NULL;
	attribute.bInheritHandle = FALSE;
	CaDefaultAnsi2Unicode(out_put_database_file_prefix,str_buffer);
	CreateDirectory(str_buffer,&attribute);
}

void LIB_DATABASE_EXPORT CaDefaultAnsi2UnicodeLen(char*src,int len,unsigned short*tar)
{
	unsigned int word_cnt;
	word_cnt=MultiByteToWideChar(0,0,(unsigned char*)src,len,tar,len);
	tar[word_cnt]=0;
}

void LIB_DATABASE_EXPORT CaDefaultAnsi2Unicode(char*src,unsigned short*tar)
{
	unsigned int n;
	n=MultiByteToWideChar(0,0,(unsigned char*)src,(unsigned int)-1,0,0);
	MultiByteToWideChar(0,0,(unsigned char*)src,(unsigned int)-1,tar,n);
	tar[n]=0;
}

void LIB_DATABASE_EXPORT CaUnicode2DefaultAnsi(unsigned short* src,char*tar)
{
	unsigned int n;
	n=WideCharToMultiByte(0,0,src,(unsigned int)-1, 0, 0,0,0);
	WideCharToMultiByte(0,0,src,(unsigned int)-1,(unsigned char*)tar,n,0,0);
	tar[n]=0;
}

void LIB_DATABASE_EXPORT CaUnicode2DefaultAnsiLen(unsigned short* src,int len,char*tar)
{
	unsigned int n;
	n=WideCharToMultiByte(0,0,src,(unsigned int)-1,(unsigned char*)tar,len,0,0);
	tar[n]=0;
}

void LIB_DATABASE_EXPORT CaUnicode2Utf8Len(unsigned short* src,int len,char*tar)
{
	unsigned int n;
	n=WideCharToMultiByte(CP_UTF8,0,src,(unsigned int)-1,(unsigned char*)tar,len,0,0);
	tar[n]=0;
}

void LIB_DATABASE_EXPORT CaUnicode2Utf8(unsigned short* src,char*tar)
{
	unsigned int n;
	n=WideCharToMultiByte(CP_UTF8,0,src,(unsigned int)-1, 0, 0,0,0);
	WideCharToMultiByte(CP_UTF8,0,src,(unsigned int)-1,(unsigned char*)tar,n,0,0);
	tar[n]=0;
}

void LIB_DATABASE_EXPORT CaUtf8ToUnicodeLen(char*src,int len,unsigned short*tar)
{
	unsigned int word_cnt;
	word_cnt=MultiByteToWideChar(CP_UTF8,0,(unsigned char*)src,len,tar,len);
	tar[word_cnt]=0;
}

void LIB_DATABASE_EXPORT CaUtf8ToUnicode(char*src,unsigned short*tar)
{
	unsigned int n;
	n=MultiByteToWideChar(CP_UTF8,0,(unsigned char*)src,(unsigned int)-1,0,0);
	MultiByteToWideChar(CP_UTF8,0,(unsigned char*)src,(unsigned int)-1,tar,n);
	tar[n]=0;
}

