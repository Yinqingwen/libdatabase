#ifndef	__CONTACTS_DATABASE_H__
#define	__CONTACTS_DATABASE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define LIB_DATABASE_EXPORT	__declspec(dllexport)

#define MAX_CHARS 100
#define MAX_CONTENT 256

typedef struct SmsStruct{
	unsigned int   index;
	unsigned short sender[MAX_CHARS];
	unsigned short recipient[MAX_CHARS];
	unsigned short content[MAX_CONTENT];
	unsigned short date[MAX_CHARS];
	unsigned short time[MAX_CHARS];
	struct SmsStruct *next;
}SmsStruct,*pSmsStruct;

LIB_DATABASE_EXPORT void init_sms_table_base();
LIB_DATABASE_EXPORT void exit_sms_table_base();
LIB_DATABASE_EXPORT int GetSmsCount();
LIB_DATABASE_EXPORT pSmsStruct *ReadAllSms();
LIB_DATABASE_EXPORT pSmsStruct *ReadSmsByIndex(unsigned int index);
//LIB_DATABASE_EXPORT void ReadSmsByParam(char*key,char*value);
LIB_DATABASE_EXPORT int WriteSmsToDatabase(unsigned int index,char *Sender,char *Recipient,char *Content,char *Date,char *Time);
LIB_DATABASE_EXPORT int UpdateSmsByIndex(unsigned int index,char*key,char*value);
LIB_DATABASE_EXPORT int DeleteSmsByIndex(unsigned int index);
LIB_DATABASE_EXPORT int DeleteAllSms();

#ifdef __cplusplus
}
#endif

#endif