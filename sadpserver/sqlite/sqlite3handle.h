#ifndef __SQLITE3HANDLE_H__
#define __SQLITE3HANDLE_H__

#include <sqlite3.h>
#include "common.h" 

sqlite3 *db;

void *sqlite3_data_save_thread( void *arg );
BOOL sqlite3_table_create(  );
int sqlite3_callback_func( void* pHandle,int iRet, char** szSrc, char** szDst );
BOOL sqlite3_db_tableExists( const char *szTableName );
static BOOL sqlite3_insert_data_handle( char *szInsertData,const SADP_INFO_RECV_T *stSadpInfo );
static BOOL sqlite3_add_table_data(  const SADP_INFO_RECV_T * stSadpInfo );
static BOOL sqlite3_del_table_data(  );
int sqlite3_sel_table_data( char **szSelectData );

#endif