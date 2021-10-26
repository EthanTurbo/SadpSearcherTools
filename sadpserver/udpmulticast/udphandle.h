#ifndef __UDPHANDLE_H__
#define __UDPHANDLE_H__


#include "common.h"


/* UDP重新发送多播消息标志，用于通知sqlite3数据库删除库里存在的设备信息 */
BOOL g_bUdpStarSendtFlag;
BOOL g_bUdpRunFlag;

void * udp_send_recv_thread( void *arg);
static BOOL udp_cfg_nonblocking_set(int sockfd);


#endif