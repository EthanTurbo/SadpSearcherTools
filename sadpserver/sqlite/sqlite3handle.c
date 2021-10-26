#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

#include "sqlite3handle.h"
#include "queue.h"
#include "sadphandle.h"
#include "common.h"
#include "udphandle.h"

static const char *szCreateOpt = " CREATE TABLE IF NOT EXISTS HikDeviceInfo(\
								 DeviceDescription TEXT,\
								 DeviceSN TEXT,\
								 SoftwareVersion TEXT,\
								 IPv4Address TEXT,\
								 IPv6Address TEXT,\
								 IPv4SubnetMask TEXT,\
								 IPv4Gateway TEXT,\
								 IPv6Gateway TEXT,\
								 CommandPort TEXT,\
								 HttpPort TEXT,\
								 MacAddress TEXT,\
								 DHCPAbility TEXT,\
								 DHCP TEXT\
								 );" ;

//	 ID INTEDER PRIMARY KEY,
BOOL sqlite3_table_create(  )
{

	const char *szTableName = "HikDeviceInfo";	// Sqlite3表名
	char *szErrMsg = NULL;
	
	if(db == NULL)
	{
		SEND_ERROR("db is NULL!");
		return FALSE;
	}	

	// 不存在表，则创建表 
	if ( SQLITE_OK == sqlite3_exec(db, szCreateOpt, 0, 0, &szErrMsg) )
	{
		SEND_DEBUG("create \"HikDeviceInfo\" table success!");
	}
	else
	{
		SEND_ERROR("create \"HikDeviceInfo\" table fail!");
		return FALSE;
	}
	
/* 	if( !sqlite3_db_tableExists( szTableName ) )
	{
		// 不存在表，则创建表 
		if ( SQLITE_OK == sqlite3_exec(db, szCreateOpt, 0, 0, &szErrMsg) )
		{
			SEND_DEBUG("create \"HikDeviceInfo\" table success!");
		}
		else
		{
			SEND_ERROR("create \"HikDeviceInfo\" table fail!");
			return FALSE;
		}
	}
	else
	{
		SEND_DEBUG("HikDeviceInfo table exists");
	} */

	return TRUE;
}

void *sqlite3_data_save_thread( void *arg )
{
	SADP_INFO_RECV_T stSadpInfo;
	char *szErrMsg = NULL;

	if(db == NULL)
	{
		SEND_ERROR("db is NULL!");
		goto OUT;
	}
	
	while(g_bUdpRunFlag) /* 1：Udp线程正常运行，0：udp线程退出，实行线程控制 */
	{
		if( 1 == g_bUdpStarSendtFlag)
		{
			/* 为了满足表内内容实时更新 */
			sqlite3_del_table_data(  ); /* 删除表内内容 */
			g_bUdpStarSendtFlag = 0;		
		}
		
		if( QUEUE_OK == readQueue( &stSadpInfo, g_stGlobalQueue ))
		{
			/* 队列内有数据，则读出数据进行数据处理 */
			if( !sqlite3_add_table_data( &stSadpInfo ) )
			{
				SEND_ERROR("Sqlite3 add data fail!");
				continue;
			}			
		}
		else 
		{
			/* 队列空则等待 */
			SEND_WARN("Queue is Empty!");
			sleep(1);			
		}
	
	}

	sqlite3_close(db);
OUT:	
	SEND_DEBUG("Sqlite3 write data thread Exits!");

}


BOOL sqlite3_db_tableExists( const char *szTableName)
{
    int iRet;
	int iTableNums = 0;
    char *szErrMsg = NULL;
    char cSql[1024];

	if(db == NULL)
	{
		SEND_ERROR("Input db is NULL!");
		return FALSE;
	}

	if(szTableName == NULL)
	{
		SEND_ERROR("Input szTableName is NULL!");
		return FALSE;
	}

    sprintf(cSql, "select count(*) from deviceinfo where type='table' and name='%s';", szTableName);
	
	ABORT_DEBUG("cSql : %s",cSql);
	
    //iRet = sqlite3_prepare(db, cSql, -1, &pvm, &szTail);
    iRet = sqlite3_exec(db, cSql, &sqlite3_callback_func, &iTableNums, &szErrMsg);
    if (iRet==SQLITE_OK)
    {
		SEND_DEBUG("tcw get");
		if(iTableNums >0)
		{
			return TRUE;
		}
    }

    return FALSE;
}

int sqlite3_callback_func( void* pHandle,int iRet, char** szSrc, char** szDst )
{
	//... 
	int iTableExist = 0;
	int* pRes = NULL;
	if ( 1 == iRet )
	{
		iTableExist = atoi(*(szSrc));  //此处返回值为查询到同名表的个数，没有则为0，否则大于0
		ABORT_DEBUG("NORMAL1! %d",iTableExist);		
		if (pHandle != NULL)
		{
			SEND_DEBUG("NORMAL2!");
			pRes = (int*)pHandle;
			*pRes = iTableExist;
		}
		// szDst 指向的内容为"count(*)"
	}
	SEND_DEBUG("ERROR EXIT!");
	return 0; //返回值一定要写，否则下次调用 sqlite3_exec(...) 时会返回 SQLITE_ABORT
}

static BOOL sqlite3_insert_data_handle( char *szInsertData,
										const SADP_INFO_RECV_T *stSadpInfo
										)
{
	const char *szInsertHead = "INSERT INTO 'HikDeviceInfo' VALUES(";
	unsigned int iCntLen = 0;

	if(szInsertData == NULL)
	{
		SEND_ERROR("Input szInsertData is NULL!");
		return FALSE;
	}

	if(stSadpInfo == NULL)
	{
		SEND_ERROR("Input stSadpInfo is NULL!");
		return FALSE;
	}

	memcpy(szInsertData, szInsertHead, strlen(szInsertHead));
	iCntLen += strlen(szInsertHead);

	memcpy(szInsertData+iCntLen, stSadpInfo->ucDevDescription, strlen(stSadpInfo->ucDevDescription));
	iCntLen += strlen(stSadpInfo->ucDevDescription);
	*(szInsertData+iCntLen) = ',';
	iCntLen += 1;

	memcpy(szInsertData+iCntLen, stSadpInfo->ucDevSN, strlen(stSadpInfo->ucDevSN));
	iCntLen += strlen(stSadpInfo->ucDevSN);
	*(szInsertData+iCntLen) = ',';
	iCntLen += 1;

	memcpy(szInsertData+iCntLen, stSadpInfo->ucSoftwareVersion, strlen(stSadpInfo->ucSoftwareVersion));
	iCntLen +=  strlen(stSadpInfo->ucSoftwareVersion);
	*(szInsertData+iCntLen) = ',';
	iCntLen += 1;

	memcpy(szInsertData+iCntLen, stSadpInfo->ucIPv4Address, strlen(stSadpInfo->ucIPv4Address));
	iCntLen +=  strlen(stSadpInfo->ucIPv4Address);
	*(szInsertData+iCntLen) = ',';
	iCntLen += 1;

	memcpy(szInsertData+iCntLen, stSadpInfo->ucIPv6Address, strlen(stSadpInfo->ucIPv6Address));
	iCntLen +=  strlen(stSadpInfo->ucIPv6Address);
	*(szInsertData+iCntLen) = ',';
	iCntLen += 1;

	memcpy(szInsertData+iCntLen, stSadpInfo->ucIPv4SubnetMask, strlen(stSadpInfo->ucIPv4SubnetMask));
	iCntLen +=  strlen(stSadpInfo->ucIPv4SubnetMask);
	*(szInsertData+iCntLen) = ',';
	iCntLen += 1;

	memcpy(szInsertData+iCntLen, stSadpInfo->ucIPv4Gateway, strlen(stSadpInfo->ucIPv4Gateway));
	iCntLen +=  strlen(stSadpInfo->ucIPv4Gateway);
	*(szInsertData+iCntLen) = ',';
	iCntLen += 1;

	memcpy(szInsertData+iCntLen, stSadpInfo->ucIPv6Gateway, strlen(stSadpInfo->ucIPv6Gateway));
	iCntLen +=  strlen(stSadpInfo->ucIPv6Gateway);
	*(szInsertData+iCntLen) = ',';
	iCntLen += 1;

	memcpy(szInsertData+iCntLen, stSadpInfo->ucDHCPAbility, strlen(stSadpInfo->ucDHCPAbility));
	iCntLen +=  strlen(stSadpInfo->ucDHCPAbility);
	*(szInsertData+iCntLen) = ',';
	iCntLen += 1;

	memcpy(szInsertData+iCntLen, stSadpInfo->ucDHCP, strlen(stSadpInfo->ucDHCP));
	iCntLen +=  strlen(stSadpInfo->ucDHCP);
	*(szInsertData+iCntLen) = ')';
	iCntLen += 1;
	*(szInsertData+iCntLen) = ';';
	iCntLen += 1;
	*(szInsertData+iCntLen) = '\0';

	return TRUE;
}

/* 数据库写入数据 */
BOOL sqlite3_add_table_data( const SADP_INFO_RECV_T * stSadpInfo )
{
	char szInsertOption[512];
	char *szErrMsg = NULL;
	memset(szInsertOption,0,sizeof(szInsertOption));
	if( !sqlite3_insert_data_handle( szInsertOption,stSadpInfo ) )
	{
		SEND_ERROR("Insert info handle fail!");
		return FALSE;
	}

	/* 插入数据 */
	sqlite3_exec(db,szInsertOption,NULL,NULL,&szErrMsg);

	return TRUE;
}

/* 数据库删除所有数据 */
BOOL sqlite3_del_table_data(  )
{
	char *szErrMsg = NULL;
	const char szOption[50] = "delete from HikDeviceInfo;";
	sqlite3_exec(db,szOption,NULL,NULL,&szErrMsg);
}

/* 数据库查找数据 */
int sqlite3_sel_table_data( char **szSelectData )
{
	char *szOption="select *from HikDeviceInfo";
	char **szResult = NULL;
	int iRow = 0;
	int iColumn = 0;
	int iLen = 0;
	int i = 0,j = 0;
	char *szErrMsg = NULL;
	// 查询数据
	sqlite3_get_table( db, szOption, &szResult, &iRow, &iColumn, &szErrMsg );

	/* 按行为单位，进行数据搬移，每行存一条完整的设备信息 */
	for( i=1; i<iRow+1; i++ )
	{
		for( j=0; j<iColumn; j++ )
		{
			memcpy(&(szSelectData[i][iLen]),szResult[(i-1)*iColumn+j],sizeof(szResult[(i-1)*iColumn+j]));
			iLen += sizeof(szResult[(i-1)*iColumn+j]);
			szSelectData[i][iLen] = ',';
			iLen += 1;
		}
		szSelectData[i][iLen] = ';';
		szSelectData[i][iLen+1] = '\0';
		iLen = 0;
	}

	sqlite3_free_table(szResult);
	
	return iRow-1; /* 返回数据的条数 */
}
