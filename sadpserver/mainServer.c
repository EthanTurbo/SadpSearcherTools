#include <pthread.h>
#include <stdio.h>
#include <stdlib.h> /* 包含malloc.h */
#include <string.h>
#include "tcphandle.h"
#include "sqlite3handle.h"
#include "udphandle.h"
#include "common.h"
#include "queue.h"

const char cPath[60] = "/home/sqlite3_database/deviceinfo.db";

int main(  ) //int argc, void *argv[]
{
	int iRet;
	pthread_t id1,id2,id3;
	
	g_bUdpRunFlag = 1;
	g_bUdpStarSendtFlag = 1;	
	g_bSendConfigMsg = 0;
	
	g_stGlobalQueue = (COMMQUEUE_T *)malloc( sizeof(COMMQUEUE_T) );  //队列内存初始化
	db = NULL;
		
	// 输入处理
	cpDbPath = malloc(sizeof(char)*100);
	memcpy( cpDbPath, cPath, sizeof(cPath) );
	
	/* 循环队列初始化 */
	if( QUEUE_FAIL == init_queue( g_stGlobalQueue) )
	{
		SEND_ERROR("Queue init fail!\n");
		return FALSE;
	}
	
	// 初始化  创建数据库表
	/* 打开数据库 */
	if( sqlite3_open(cpDbPath,&db) )
	{
		/*  sqlite3_errmsg(db)用以获得数据库打开错误码的英文描述  */
		ABORT_ERROR("Can't open database: %s", sqlite3_errmsg(db));
		sqlite3_close(db);
		return FALSE;
	}

	/* 创建表 */
	if( !sqlite3_table_create(  ) )
	{
		SEND_ERROR("Table create fail!");
		sqlite3_close(db);
		return FALSE;
	}
	
#if 1	
	/* 开启udp组播线程 */
	iRet = pthread_create(&id1, NULL, udp_send_recv_thread, db);
	if(iRet)
	{
		SEND_ERROR("udp_send_recv_thread create failed!");
	}
	
	/* 开启sqlite3写线程 */
	iRet = pthread_create(&id2, NULL, sqlite3_data_save_thread, db);
	if(iRet)
	{
		SEND_ERROR("sqlite3_data_save_thread create failed!");
	}
#endif
	
	memset( g_cConfigShm,0,sizeof(g_cConfigShm) );
	/* 开启tcp服务器线程 */
	iRet = pthread_create(&id3, NULL, tcp_server_thread,db);
	if(iRet)
	{
		SEND_ERROR("tcp_server_thread create failed!");
	}
	
	/* 主线程阻塞，等待子线程结束 */
#if 0	
	pthread_join(id1, NULL);  
    pthread_join(id2, NULL);	
#endif

    pthread_join(id3, NULL);	
	
	sqlite3_close(db);
	free(g_stGlobalQueue);
	free(cpDbPath);
	return 0;
}