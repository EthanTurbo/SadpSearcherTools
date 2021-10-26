#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "tcpclient.h"
#include "common.h"
#include "shell.h"


int main(int argc, char **argv)
{
	int iRet;
	pthread_t id1;
	
	/* 初始化 */
	g_bStartFlag = 0;
	g_bExitFlag = 0;
	g_iSendDataType = CMD_NOP;
	memset( g_cConfigShm,0,sizeof(g_cConfigShm) );
	
	/* 开启tcp客户端线程 */
	iRet = pthread_create(&id1, NULL, tcp_client_thread, NULL);
	if(iRet)
	{
		SEND_ERROR("tcp_client_thread create failed!");
	}	
		
	/* 输入处理线程 */
	shell_loop();

	
	/* 等待tcp客户端线程退出 */
	pthread_join(id1, NULL);  
	
	return 0;
}