#ifndef __TCPHANDLE_H__
#define __TCPHANDLE_H__

#include "common.h"

BOOL g_bSendConfigMsg;
char g_cConfigShm[1024];


void *tcp_server_thread(void *arg);
static BOOL tcp_conn_handle( int iSockfd );
static void *tcp_request_handle(void *arg);
static BOOL tcp_recv_info_handle( char *szSendData, char *szRecvData, int *iMsgLen);
static int tcp_inquiry_info_handle( char *szSendData );
static int tcp_config_info_handle( char *szSendData, char *szRecvData);
static BOOL tcp_send_handle ( char *szSendData, int iConnSockfd, const int iMsgLen );

#endif