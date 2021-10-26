#ifndef __COMMON_H__
#define __COMMON_H__


#define SADP_TOOL_ERROR 
#define SADP_TOOL_WARN 
#define SADP_TOOL_DEBUG 

/*************************************调试打印信息处理*********************************/
/* 错误信息打印 */
#ifdef SADP_TOOL_ERROR
	#define ABORT_ERROR(fmt,...) printf("[ERROR]%s:%s:%d " fmt "\n", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
	#define SEND_ERROR(fmt,...)  printf("[ERROR]%s:%s:%d " fmt "\n", __FILE__, __FUNCTION__, __LINE__ )
#else
	#define ABORT_ERROR(fmt,...) 
	#define SEND_ERROR(fmt,...)  
#endif

/* 警告信息打印 */
#ifdef SADP_TOOL_WARN
	#define ABORT_WARN(fmt,...) printf("[WARN]%s:%s:%d " fmt "\n", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
	#define SEND_WARN(fmt,...)  printf("[WARN]%s:%s:%d " fmt "\n", __FILE__, __FUNCTION__, __LINE__ )
#else
	#define ABORT_WARN(fmt,...) 
	#define SEND_WARN(fmt,...)  
#endif

/* 调试信息打印 */
#ifdef SADP_TOOL_DEBUG
	#define ABORT_DEBUG(fmt,...) printf("[DBUG]%s:%s:%d " fmt "\n", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
	#define SEND_DEBUG(fmt,...)  printf("[DBUG]%s:%s:%d " fmt "\n", __FILE__, __FUNCTION__, __LINE__ )
#else
	#define ABORT_DEBUG(fmt,...) 
	#define SEND_DEBUG(fmt,...)  
#endif
/****************************************************************************/


//#define SAFE_CLOSE(fd)   do { if(-1 != fd){close(fd);(fd)=-1;} } while(0)



/****** 为了解决头文件相互包含导致的错误，将SADP_INFO_RECV_T移至本处 ******/
typedef struct SadpInfoRecv
{
	char ucDevType[4];			/* 设备类型 */
	char ucDevDescription[24];  /* “设备类型描述” 长度限制：23个字节 */
	char ucDevSN[48];			/* “设备序列号” 长度限制：47个字节 */
	char ucSoftwareVersion[48]; /* “设备序列号” 长度限制：47个字节 */
	char ucCommandPort[4];		/* 私有协议命令端口 */
	char ucHttpPort[4];			/* http端口 */
	char ucMacAddr[20];			/* “MAC地址” 长度限制：19个字节 */
	char ucIPv4Address[16];		/* “IPv4地址” 长度限制：15个字节 */
	char ucIPv6Address[48];		/* “IPv6地址” 长度限制：45个字节 */
	char ucIPv4Gateway[16];		/* “IPv4网关”长度限制：15个字节 */
	char ucIPv6Gateway[48];		/* “IPv6网关”长度限制：45个字节 */
	char ucIPv4SubnetMask[16];	/* “IPv4子网掩码”长度限制：15个字节 */
	char ucDHCPAbility[8];		/* “是否支持DHCP，true:支持，false:不支持” */	
	char ucDHCP[8];				/* “DHCP使能开关，true:开启，false:关闭” */
} SADP_INFO_RECV_T; /* 312 Byte */
/****************************************************************************/


/************************* 用于解决重定义问题 *******************************/
#define QUEUE_DATA_MAX 10  /* 循环队列中能够存多少块数据 */
 
typedef struct Queue 
{  
    SADP_INFO_RECV_T stSadpData[QUEUE_DATA_MAX];  
    int iWp;  
    int iRp;  
    int iQueueCnt; 
}COMMQUEUE_T;  
  
typedef enum
{  
    QUEUE_FULL = 0,  
    QUEUE_EMPTY,  
    QUEUE_OK,  
    QUEUE_FAIL  
} QUEUESTATUS_T;
/****************************************************************************/

typedef enum {TRUE=1,FALSE=0} BOOL;

char *cpDbPath ;





#endif