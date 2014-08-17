#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windef.h>

#include "common_api.h"
#include "sqlite3.h"
#include "sms_database.h"

static const char*DATABASE_FILE_NAME="sms_database.db";

static FILE *fp=NULL;

//sms recv store table
static char*create_record_table_cmd="create table sms_recv_table(Index_num,Sender,Recipient,Content,Date,Time)";
static char*insert_record_table_cmd="insert into sms_recv_table values('%u','%s','%s','%s','%s','%s')";
static char*update_record_table_cmd="update sms_recv_table SET %s='%s' WHERE Index_num='%u'";
static char*select_record_by_index="select * from sms_recv_table WHERE Index_num='%u'";
static char*select_record_by_pattern="select * from sms_recv_table WHERE %s like '%%%s%%'";
static char*select_all_record_table_cmd="select * from sms_recv_table";
static char*select_record_count_table_cmd="select count(*) from sms_recv_table";
static char*delete_all_record_cmd="delete from sms_recv_table";
static char*delete_record_by_index="delete from sms_recv_table WHERE Index_num='%u'";
static char*null_string="null";

static struct {
	sqlite3*db;
	char cmd_buffer[4096];
}SMS_DATABASE_MAN;

static struct SMS_MANAGER{
	pSmsStruct head;
	unsigned int count;
}SMS_MANAGER;

static int count_callback_exec(void*count,int argc, char ** argv, char ** aszColName)
{
	if(count) {
		(*(int*)count)=atoi(argv[0]);
	}
	return 0;
}

static int find_all_sms_call_back(void*param,int argc,char**argv,char**aszColName)
{
	pSmsStruct sms=(pSmsStruct)malloc(sizeof(SmsStruct));
	sms->index=(unsigned int)atoi(argv[0]);
	CaDefaultAnsi2Unicode(argv[1],sms->sender);
	CaDefaultAnsi2Unicode(argv[2],sms->recipient);
	CaDefaultAnsi2Unicode(argv[3],sms->content);
	CaDefaultAnsi2Unicode(argv[4],sms->date);
	CaDefaultAnsi2Unicode(argv[5],sms->time);

	sms->next=SMS_MANAGER.head;
	SMS_MANAGER.head=sms;
	if(SMS_MANAGER.count<sms->index) {
		SMS_MANAGER.count=sms->index;
	}
	return 0;
}

static int find_sms_by_index_callback(void*param,int argc,char**argv,char**aszColName)
{
	pSmsStruct sms=(pSmsStruct)malloc(sizeof(SmsStruct));
	sms->index=(unsigned int)atoi(argv[0]);
	CaDefaultAnsi2Unicode(argv[1],sms->sender);
	CaDefaultAnsi2Unicode(argv[2],sms->recipient);
	CaDefaultAnsi2Unicode(argv[3],sms->content);
	CaDefaultAnsi2Unicode(argv[4],sms->date);
	CaDefaultAnsi2Unicode(argv[5],sms->time);

	SMS_MANAGER.head=sms;
	return 0;
}

LIB_DATABASE_EXPORT void init_sms_table_base()
{
	int ret=0;
	char*err_msg=NULL;
	char *db_file_name;

	db_file_name=CaGetTempDataBuffer();
	CaGetDatabaseFilePrefix(db_file_name);
	strcat(db_file_name,DATABASE_FILE_NAME);

	memset(&SMS_DATABASE_MAN,0,sizeof(SMS_DATABASE_MAN));
	memset(&SMS_MANAGER,0,sizeof(SMS_MANAGER));

	fp=fopen("sms_sqlite_log.txt","w+");
	
	ret=sqlite3_open(db_file_name,&SMS_DATABASE_MAN.db);
	if(ret!=SQLITE_OK) {
		fprintf(fp,"sqlite database file open exist!\n");
		return;
	}

	ret=sqlite3_exec(SMS_DATABASE_MAN.db,create_record_table_cmd,NULL,0,&err_msg);
	if(ret!=SQLITE_OK) {
		fprintf(fp,"Sql table create Error:%d\n",err_msg);
		sqlite3_free(err_msg);
	}
}

LIB_DATABASE_EXPORT void exit_sms_table_base()
{
	pSmsStruct p_next=NULL;

	if(SMS_DATABASE_MAN.db) {
		sqlite3_close(SMS_DATABASE_MAN.db);
		SMS_DATABASE_MAN.db=NULL;
	}

	if(fp)fclose(fp);

	if(SMS_MANAGER.head==NULL) {
		return;
	}
	while(SMS_MANAGER.head!=NULL) {
		p_next=SMS_MANAGER.head->next;
		free(SMS_MANAGER.head);
		SMS_MANAGER.head=p_next;
	}
}

LIB_DATABASE_EXPORT int WriteSmsToDatabase(unsigned int index,char *Sender,char *Recipient,char *Content,char *Date,char *Time)
{
	int ret=SQLITE_ERROR;
	char*err_msg=NULL;

	if(!index)
	{
		index=GetSmsCount();
		index++;
	}
	
	sprintf(SMS_DATABASE_MAN.cmd_buffer,insert_record_table_cmd,index,((Sender)?Sender:null_string),
		(Recipient)?Recipient:null_string,((Content)?Content:null_string),((Date)?Date:null_string),
		((Time)?Time:null_string));

	ret=sqlite3_exec(SMS_DATABASE_MAN.db,SMS_DATABASE_MAN.cmd_buffer,NULL,0,&err_msg);
	if(ret!=SQLITE_OK) {
		fprintf(fp,"WriteSmsToDatabase Error:%d\n",err_msg);
		sqlite3_free(err_msg);
	}

	return (ret==SQLITE_OK);
}

LIB_DATABASE_EXPORT int UpdateSmsByIndex(unsigned int index,char*key,char*value)
{
	int ret=SQLITE_ERROR;
	char*err_msg=NULL;
	if(!key) {
		sprintf(SMS_DATABASE_MAN.cmd_buffer,update_record_table_cmd,(unsigned short)value,index);
	}
	sprintf(SMS_DATABASE_MAN.cmd_buffer,update_record_table_cmd,key,value,index);
	ret=sqlite3_exec(SMS_DATABASE_MAN.db,SMS_DATABASE_MAN.cmd_buffer,NULL,0,&err_msg);
	if(ret!=SQLITE_OK) {
		sqlite3_free(err_msg);
	}

	return (ret==SQLITE_OK);
}

LIB_DATABASE_EXPORT int GetSmsCount()
{
	int count=0;
	int ret;
	char*err_msg=NULL;

	ret=sqlite3_exec(SMS_DATABASE_MAN.db,select_record_count_table_cmd,count_callback_exec,(void*)&count,&err_msg);
	if(ret!=SQLITE_OK) {
		sqlite3_free(err_msg);
		return 0;
	}
	return count;
}

LIB_DATABASE_EXPORT pSmsStruct *ReadAllSms()
{
	int ret=SQLITE_ERROR;
	char*err_msg=NULL;
	
	sprintf(SMS_DATABASE_MAN.cmd_buffer,select_all_record_table_cmd);
	ret=sqlite3_exec(SMS_DATABASE_MAN.db,SMS_DATABASE_MAN.cmd_buffer,find_all_sms_call_back,NULL,&err_msg);
	if(ret!=SQLITE_OK) {
		fprintf(fp,"ReadAllSms Error:%d\n",err_msg);
		sqlite3_free(err_msg);
	}

	return &SMS_MANAGER.head;
}

LIB_DATABASE_EXPORT pSmsStruct *ReadSmsByIndex(unsigned int index)
{
	int ret=SQLITE_ERROR;
	int result=FALSE;
	char*err_msg=NULL;
	
	sprintf(SMS_DATABASE_MAN.cmd_buffer,select_record_by_index,index);
	ret=sqlite3_exec(SMS_DATABASE_MAN.db,SMS_DATABASE_MAN.cmd_buffer,find_sms_by_index_callback,(void*)&result,&err_msg);
	if(ret!=SQLITE_OK) {
		fprintf(fp,"ReadSmsByIndex Error:%d\n",err_msg);
		sqlite3_free(err_msg);
	}

	return &SMS_MANAGER.head;
}

//LIB_DATABASE_EXPORT void ReadSmsByParam(char*key,char*value,void*param)
//{
//	int ret;
//	char*err_msg=NULL;
//	
//	sprintf(SMS_DATABASE_MAN.cmd_buffer,select_record_by_pattern,key,value);
//	ret=sqlite3_exec(SMS_DATABASE_MAN.db,SMS_DATABASE_MAN.cmd_buffer,find_sms_by_index_callback,param,&err_msg);
//	if(ret!=SQLITE_OK) {
//		printf("Error:%s\n",err_msg);
//		sqlite3_free(err_msg);
//	}
//}

LIB_DATABASE_EXPORT int DeleteSmsByIndex(unsigned int index)
{
	int ret=SQLITE_ERROR;
	char*err_msg=NULL;

	//删除之前先判断是否存在
	pSmsStruct *sms = NULL;
	sms=ReadSmsByIndex(index);
	if(*sms)
	{
		free(*sms);
		*sms=NULL;
	}else
	{
		return FALSE;
	}

	sprintf(SMS_DATABASE_MAN.cmd_buffer,delete_record_by_index,index);
	ret=sqlite3_exec(SMS_DATABASE_MAN.db,SMS_DATABASE_MAN.cmd_buffer,NULL,0,&err_msg);
	if(ret!=SQLITE_OK) {
		fprintf(fp,"DeleteSmsByIndex Error:%d\n",err_msg);
		sqlite3_free(err_msg);
	}

	return TRUE;
}
LIB_DATABASE_EXPORT int DeleteAllSms()
{
	int ret=SQLITE_ERROR;

	char*err_msg=NULL;
	ret=sqlite3_exec(SMS_DATABASE_MAN.db,delete_all_record_cmd,NULL,0,&err_msg);
	if(ret!=SQLITE_OK) {
		fprintf(fp,"DeleteAllSms Error:%d\n",err_msg);
		sqlite3_free(err_msg);
	}
	
	return TRUE;
}