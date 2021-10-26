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


typedef enum {FALSE = 0, TRUE = 1 }BOOL;

char g_cConfigShm[1024];


#endif
