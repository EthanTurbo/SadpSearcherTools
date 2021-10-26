#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h> /* 线程头文件 */
#include <errno.h>	 /* errno */

#include "tcphandle.h"
#include "sqlite3handle.h"
#include "sadphandle.h"

#define LOCAL_IPADDR "192.168.0.201"
#define LOCAL_PORT  		8887	/* 本地端口号 */

#define TCP_CONN_MAX   		1024		/* 线程头文件 */

#define TCP_RECV_BUF_SIZE 	1024	/* 收缓存大小 */

#define DEV_MSG_MAX			50		/* 数据库实时存储的设备信息的最大数目 */
#define TCP_SEND_BUF_SIZE 	1024	 	/* 收缓存大小 */ 

static const char cTcpInfoHead[4] = {0x11,0x22,0x33,0x44};
static const char cTcpInfoEnd[4]  = {0x44,0x33,0x22,0x11};

static const char cTcpDataHead[4] = {0x10,0x11,0x12,0x13};
static const char cTcpDataEnd[4]  = {0x01,0x02,0x03,0x04};

static const char *cpXmlMsgHead = 
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<Probe>\n\
<Uuid>8d2091bc-1dd2-11b2-807b-8ce748cf9334</Uuid>\n\
<Types>update</Types>\n";

static const char *cpXmlMsgTail = "</Probe>\n";

static enum {CMD_INQUIRY = 0, CMD_CONFIGURE = 1};


void *tcp_server_thread(void *arg)
{
    //定义sockfd
    int iSockfd_S;

	struct sockaddr_in stServerSockAddr;
    
	    
	
	iSockfd_S = socket(AF_INET,SOCK_STREAM, 0);
    if( -1 == iSockfd_S )
	{
		SEND_ERROR("Tcp server socket create fail!");
		goto EXIT;
	}
	
    ///定义sockaddr_in
    
    stServerSockAddr.sin_family = AF_INET;
    stServerSockAddr.sin_port 	= htons(LOCAL_PORT);
    stServerSockAddr.sin_addr.s_addr = inet_addr(LOCAL_IPADDR);
    
    ///bind，成功返回0，出错返回-1
    if(-1 == bind(iSockfd_S,(struct sockaddr *)&stServerSockAddr,sizeof(stServerSockAddr)) )
    {
		close(iSockfd_S);
        SEND_ERROR("TCP server bind address fail!");
        goto EXIT;
    }
    
    ///listen，成功返回0，出错返回-1
    if( -1 == listen( iSockfd_S,TCP_CONN_MAX ) )
    {
		close(iSockfd_S);
        SEND_ERROR("TCP server listen fail!");
        goto EXIT;
    }
    
    ///客户端套接字
	if( !tcp_conn_handle( iSockfd_S ) )
	{
		goto EXIT;
	}
	
EXIT:
	SEND_DEBUG("TCP thread Exits!");
}

static BOOL tcp_conn_handle( int iSockfd )
{
	int iSockfd_C[TCP_CONN_MAX];
	int iClientLen = 0;		
	//char cBuffer[BUFFER_SIZE];
	
	unsigned char cIdIndex = 0;
	pthread_t id[TCP_CONN_MAX];
	struct sockaddr_in stClientSockAddr;

	memset(iSockfd_C,-1,TCP_CONN_MAX*sizeof(int));
    iClientLen = sizeof(stClientSockAddr);

    ///成功返回非负描述字，出错返回-1
	while(1)
	{
		if(cIdIndex >= TCP_CONN_MAX)
		{
			cIdIndex = 0;
		}
		SEND_DEBUG("等待客户端连接");
		iSockfd_C[cIdIndex] = accept( iSockfd, 
									  (struct sockaddr*)&stClientSockAddr, 
									  &iClientLen 
									);
		if( 0 > iSockfd_C[cIdIndex] )
		{
			SEND_ERROR("TCP server connect client fail!");
			continue;
		}
		SEND_DEBUG("客户端成功连接");
		
		if( pthread_create( &id[cIdIndex],
							NULL,
							tcp_request_handle,
							(void *)&(iSockfd_C[cIdIndex]) )
						  )  //创建线程
		{
			SEND_ERROR("thread creation failed");
			close(iSockfd_C[cIdIndex]);
            continue;
		}
		ABORT_DEBUG("Sucess create %d client thread.",id[cIdIndex]);
		cIdIndex += 1;				
	}	
	
	close(iSockfd);
	return FALSE;
}

static void *tcp_request_handle(void *arg)
{
	int iConnSockfd = *((int *)arg);
	char cRecvBuf[TCP_RECV_BUF_SIZE];
	char *cSendBuf = (char *)malloc(DEV_MSG_MAX*TCP_SEND_BUF_SIZE*sizeof(char));
	int iRecvLen= -1;
	int iMsgLen = 0;
	int i = 0;	
	
	/* 建立TCP短连接，客户端一次收发数据完毕后，主动关闭套接字 */
	while(1)
	{
		memset( cRecvBuf,0,sizeof(cRecvBuf) );
		
		/* 一次性收完客户端发送的数据 */
        iRecvLen = recv(iConnSockfd, cRecvBuf, sizeof(cRecvBuf),0);

		if(iRecvLen > 0)
		{
			if(  (0 == strncmp( cRecvBuf,cTcpInfoHead,sizeof(cTcpInfoHead) )) &&
				 (0 == strncmp( cRecvBuf+iRecvLen-sizeof(cTcpInfoEnd), cTcpInfoEnd, sizeof(cTcpInfoEnd) )) )				 
			{
				iMsgLen = 0;
				
				memset(cSendBuf,0,sizeof(cSendBuf));			
				
				/* 客户端数据包接收完毕 */
				/* 消息处理 ， 查表 */
				tcp_recv_info_handle(cSendBuf, cRecvBuf+sizeof(cTcpInfoHead), &iMsgLen);
				/* 给客户端返回消息 */
				if( !tcp_send_handle (cSendBuf, iConnSockfd, iMsgLen ) )
				{
					break;
				};	
				
			}
		}
		else if(iRecvLen == 0) 
		{ 
			 /* 这里表示对端的socket已正常关闭. */
			 SEND_DEBUG("TCP Client closed socket.");
			 break;
		} 
		else
		{
			if( errno == EAGAIN||errno == EINTR )
			{
				continue; /* 继续接收数据 */
			}
			break;		/* 跳出接收循环 */
		}		
				
	}
	free(cSendBuf);
	close(iConnSockfd);
	SEND_DEBUG("TCP Server close client socket.");
}



static BOOL tcp_recv_info_handle( char *szSendData, char *szRecvData, int *iMsgLen)
{
	int  cCmdType = -1;
	int i;
	
	if(szRecvData == NULL )
	{
		SEND_ERROR("Input szRecvData is NULL!");
		return FALSE;
	}
	
	if( szSendData == NULL )
	{
		SEND_ERROR("Input szSendData is NULL!");
		return FALSE;
	}

	cCmdType = *szRecvData; /* 第一个字节为命令类型，0：查询，1：配置*/	
	switch(cCmdType)
	{
		case CMD_INQUIRY:
			*iMsgLen = tcp_inquiry_info_handle( szSendData );
			break;
			
		case CMD_CONFIGURE:
			*iMsgLen = tcp_config_info_handle( szSendData, szRecvData+1);
			break;
			
		default:
			SEND_WARN("Invalid command type!");
			return FALSE;
	}
	
	SEND_DEBUG("Print Send Data:");
	for(i=0;i<*szSendData+1;i++)
		printf("[i]%d ",*(szSendData+i));
	printf("\n");
	
	return TRUE;	
	
}

/**********************************************************

客户端发送的数据格式：（ 查询/配置 ）
	|  4Byte |   1Byte   |   2Byte   | 数据长度 | 4Byte  |
	| 数据头 | 命令类型 | 数据长度 | 数据信息 | 数据尾 |
	数据长度= len0+len1*255

服务器端发送的数据格式：
	|  4Byte |   1Byte   | 数据长度 | 4Byte  |
	| 数据头 | 信息条数 | 数据信息 | 数据尾 |
	
服务器端发送的数据信息对应的格式如下：
	| 4Byte |  2Byte |   Len   | 4Byte|...| 4Byte |  1Byte |   Len   | 4Byte|
	|数据头|数据长度|数据内容|数据尾|...|数据头|数据长度|数据内容|数据尾|

**********************************************************/
static int tcp_inquiry_info_handle( char *szSendData )
{
	int iMsgLen = 0;
	int iCntLen = 0;
	int i = 0;
	char szDevMsg[DEV_MSG_MAX][sizeof(SADP_INFO_RECV_T)];
	int iIndex = 0;
	
	//查表 判断表是否为空
	iMsgLen = sqlite3_sel_table_data( szDevMsg );
	if( 48 < iMsgLen )
	{
		/* 最多支持查的设备数 */
		iMsgLen = DEV_MSG_MAX-2;
	}
	//表不为空则查表中的数据
	if( 0 >= iMsgLen )
	{	
		SEND_DEBUG("Table is null,handle data_len=0.");
		/* 表内无数据，返回信息长度为0 */
		/* 信息头 */
		memcpy(szSendData+1,cTcpInfoHead,sizeof(cTcpInfoHead));
		iIndex += sizeof(cTcpInfoHead);
		/* 数据长度 */
		*(szSendData+iIndex+1) = 0;
		iIndex += 1;
		/* 信息尾 */
		memcpy( szSendData+iIndex+1,cTcpInfoEnd,sizeof(cTcpInfoEnd));
		iIndex += sizeof(cTcpInfoEnd);
		/* 信息长度 */
		*szSendData = iIndex;
		iMsgLen = 1;
	}
	else
	{
		/* 表内有数据，返回各条设备数据 */
		memcpy( szSendData+1,cTcpInfoHead,sizeof(cTcpInfoHead) );
		iIndex = sizeof(cTcpInfoHead);	
		*(szSendData+iIndex+1) = iMsgLen;
		iIndex += 1;
		*szSendData = iIndex;
		
		for( i=1;i<=iMsgLen;i++ )
		{
			iIndex = 0;
			iCntLen = i*TCP_SEND_BUF_SIZE;
			memcpy( szSendData+iCntLen+1, cTcpDataHead, sizeof(cTcpDataHead) );
			iIndex += sizeof(cTcpDataHead);
			if( 255 <  strlen(szDevMsg[i]) )
			{
				*(szSendData+iCntLen+iIndex+1) = strlen(szDevMsg[i]) % 255;
				iIndex += 1;
				*(szSendData+iCntLen+iIndex+1) =  strlen(szDevMsg[i]) / 255;
			}
			else
			{
				*(szSendData+iCntLen+iIndex+1) = 255;
				iIndex += 1;
				*(szSendData+iCntLen+iIndex+1) =  0;				
			}
			iIndex += 1;
			memcpy( szSendData+iCntLen+iIndex+1, &(szDevMsg[i][0]), strlen(szDevMsg[i]) );
			iIndex += strlen(szDevMsg[i]);			
			memcpy( szSendData+iCntLen+iIndex+1, cTcpDataEnd, sizeof(cTcpDataEnd) );
			iIndex += sizeof(cTcpDataEnd);
			*(szSendData+iCntLen) = iIndex;
		}
		
		iIndex = 0;
		iCntLen += iCntLen+TCP_SEND_BUF_SIZE;
		memcpy( szSendData+iCntLen+1, cTcpInfoEnd, sizeof(cTcpInfoEnd) );
		iIndex = sizeof(cTcpInfoEnd);
		*(szSendData+iCntLen) = iIndex;
		iMsgLen += 2;		
	}
	SEND_DEBUG("6");
	return iMsgLen;
	
}

/* 每条xml节点尾部都需要换行符 */
static int tcp_config_info_handle( char *szSendData, char *szRecvData )
{
	int iMsgLen = 0;
	int iIndex = 0;
	int i = 0;
	int iRcvLen = *szRecvData + (*(szRecvData+1))*255;
	
	
	/* 写入待配网络参数信息 */
	memcpy( g_cConfigShm, szRecvData+2, iRcvLen );
	iIndex += iRcvLen;
	g_cConfigShm[iIndex] = '\0';
	
	/* 通知 UDP组播 发送 */
	g_bSendConfigMsg = 1;
	
	iMsgLen = 1;

	
	return iMsgLen;
}

static BOOL tcp_send_handle ( char *szSendData, int iConnSockfd, const int iMsgLen )
{
	int i = 0,j=0;
	int ret = -1;
	int iCntLen = 0;
	SEND_DEBUG("7");
	for(i=0;i<iMsgLen;i++)
	{
		iCntLen = i*TCP_SEND_BUF_SIZE;
		int ret = send(iConnSockfd, szSendData+iCntLen+1, *(szSendData+iCntLen), 0);
		if(ret>0 && ret < TCP_SEND_BUF_SIZE)
		{
			ABORT_DEBUG("TCP Server send %d byte message to client!",ret);
			continue;
		}
		else if(ret == 0)
		{
			/* 客户端已断开连接 */
			ABORT_ERROR("TCP_CLIENT %d closed!",iConnSockfd);
			return FALSE;
		}
		else 
		{
		   if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
		   {
			   continue;
		   }
		   else
		   {
			   ABORT_ERROR("TCP server send message to clent %d failed!",iConnSockfd);
			   return FALSE;
		   }
		} 		
	}
	
	
	return TRUE;
	
}
