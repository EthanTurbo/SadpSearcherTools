#include "udphandle.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>	/* inet_aton函数 */
#include <sys/time.h>	/* 设置超时函数 */
#include <unistd.h>		/* sleep usleep函数 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>		/* errno */

#include "sqlite3handle.h"
#include "common.h"
#include "tcphandle.h"
#include "sadphandle.h"


#define RECV_BUF_LEN		4096  			   	/* 收数据buffer长度 */
#define LOCAL_IPADDR		"192.168.0.201"  	/* 本地IP地址 */

#define UDP_MULTICAST_ADDR	"239.255.255.250"	/* 多播组IP */
#define UDP_MULTICAST_PORT	37020				/* 多播组端口 */


const char *cpProbe = 
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<Probe>\
<Uuid>8d2091bc-1dd2-11b2-807b-8ce748cf9334</Uuid>\
<Types>inquiry</Types>\
</Probe>";




void * udp_send_recv_thread( void *arg)
{
	unsigned int i = 0;
	int iRecvLen = 0;
	int iSendLen = 0;
	int iLocalAddrLen = 0;
	int iSocket = -1;							/* 套接字文件描述符 */
	//char iMsgID =  0;							/* UDP广播接收到的IPC编号 */		
	char szMsgBuf[RECV_BUF_LEN];			 
	struct ip_mreq stMreq;						/* 多播地址结构体 */ 
	struct sockaddr_in stRecvAddr,stSendAddr;	/* 本地地址 */
	struct timeval stTimeout;					/* 设置超时结构体 */
	
	/* udp线程正常运行 */
	g_bUdpRunFlag = 1;
	
	/* 创建套接字 */
	if( -1 == (iSocket = socket(AF_INET, SOCK_DGRAM, 0)) )
	{
		//SEND_ERROR("Socket create failed!");
		SEND_ERROR("UDP Socket create failed!");
		goto END;
	}
	
	if( !udp_cfg_nonblocking_set( iSocket ))
	{
		//SEND_WARN("Set socket nonblocking failed!");
		SEND_ERROR("UDP Set socket nonblocking failed!");
	}
	
	/* 初始化地址信息 */
	//memset_s(&stRecvAddr,sizeof(stRecvAddr),0,sizeof(stRecvAddr));
	memset(&stRecvAddr, 0, sizeof(stRecvAddr));
	stRecvAddr.sin_family = AF_INET;	
	stRecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);	/* 绑定IP，INADDR_ANY表示任意地址 */
	stRecvAddr.sin_port = htons(UDP_MULTICAST_PORT);					/* 绑定端口号 */
	
	/* 绑定自己的端口和IP信息到套接字上 */
	if( 0 > bind(iSocket, (struct sockaddr*)&stRecvAddr, sizeof(stRecvAddr)) )
	{
		//SEND_ERROR("Socket bind address failed!");
		SEND_ERROR("UDP Socket bind address failed!");
		close( iSocket );
		goto END;		
	}

	
	//memset_s(&stMreq,sizeof(stMreq),0,sizeof(stMreq));
	memset(&stMreq, 0, sizeof(stMreq));
	/* 初始化多播组的地址信息 */
	stMreq.imr_multiaddr.s_addr = inet_addr(UDP_MULTICAST_ADDR); 	/* 多播地址 */
	stMreq.imr_interface.s_addr = inet_addr(LOCAL_IPADDR);	/* 将本机加入多播组 */ 
	//inet_aton(LOCAL_IPADDR, &stMreq.imr_interface.s_addr );

	/* 设置组地址 */
    inet_pton(AF_INET,"239.255.255.250",&stSendAddr.sin_addr);
    iLocalAddrLen = sizeof(stSendAddr);
	
	/* 加入多播组 */
	if( setsockopt(iSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP,&stMreq, sizeof(stMreq)) )	
	{
		SEND_ERROR("UDP Add membership failed!");
		close( iSocket );
		goto END;
	}

    /* 设置超时时间 */
    stTimeout.tv_sec  = 5;                 /* 设置5s超时 */
    stTimeout.tv_usec = 0;
    if ( 0>setsockopt(iSocket, SOL_SOCKET, SO_RCVTIMEO, &stTimeout, sizeof(stTimeout)) )
    {
        //SEND_ERROR("Set timeout failed!");
        SEND_ERROR("UDP Set timeout failed!");
		/* 退出组播 */
		setsockopt( iSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, &stMreq, sizeof(stMreq) );		
		
		close( iSocket );
		goto END;		
    }
	
	while(1)
	{
		g_bUdpStarSendtFlag  = 1;
		while(g_bUdpStarSendtFlag)
		{
			/* 等待数据库删表 */
			;
		}
		
		if(g_bSendConfigMsg)
		{
			iSendLen = sendto(iSocket, g_cConfigShm, sizeof(g_cConfigShm), 0,
									(struct sockaddr*)&stRecvAddr, sizeof(stRecvAddr));	
			// 增加反馈处理
			g_bSendConfigMsg = 0;						
			
		}
		
		
		iSendLen = sendto(iSocket, cpProbe, strlen(cpProbe), 0,
						(struct sockaddr*)&stRecvAddr, sizeof(stRecvAddr));
		if( -1 == iSendLen )
		{
			SEND_ERROR("UDP Probe message send failed!");
			continue;
		}
		else
		{
			SEND_DEBUG("Search message send success!");
			ABORT_DEBUG("Send message length:%d",iSendLen);
			iSendLen = 0;
			memset(szMsgBuf, 0, sizeof(szMsgBuf));
		}
		
		/* 接收数据 */
		while(1)		
		{		
			iRecvLen = 0;
			iRecvLen = recvfrom(iSocket, szMsgBuf, sizeof(szMsgBuf), \
					0,(struct sockaddr*)&stSendAddr, &iLocalAddrLen);
			if( -1 == iRecvLen )     
			{    
				SEND_ERROR("Message receive failed!");   
				break;
			} 
			else if( errno == EAGAIN )
			{
				SEND_ERROR("Message receive timeout!");   
				break;				
			}
			else
			{
				//iMsgID++;
				ABORT_DEBUG("Recv message from server:%s", szMsgBuf);
				
				
				/* 将读取的数据存进循环队列中 */
				if( !sadp_recv_info_handle( szMsgBuf ) )
				{
					// 协议处理失败
					return FALSE;
				}
				
				//memset_s(szMsgBuf, sizeof(szMsgBuf), 0, sizeof(szMsgBuf));  				
				memset(szMsgBuf, 0, sizeof(szMsgBuf));  				
			}
		}
		sleep(10);
  	}
	
	/* 退出组播 */
	setsockopt( iSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, &stMreq, sizeof(stMreq) );
	
	close( iSocket );
	
END:
	/* udp线程退出，告知sqlite3写线程退出 */
	g_bUdpRunFlag = 0;
	SEND_DEBUG("UDP Multicast thread Exits!");
}

static BOOL udp_cfg_nonblocking_set(int sockfd) 
{
	int flag = 0;
    if ( 0>( flag=fcntl(sockfd, F_GETFL, 0)) ) 
	{
        SEND_ERROR("fcntl F_GETFL fail!");
        return FALSE;
    }
    if (fcntl(sockfd, F_SETFL, flag | O_NONBLOCK) < 0) 
	{
        SEND_ERROR("fcntl F_SETFL fail!");
		return FALSE;
    }
	return TRUE;
}
