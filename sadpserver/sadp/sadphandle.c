
#include "queue.h"
#include <string.h>
#include <unistd.h>
#include "sadphandle.h"
#include "queue.h"
#include <malloc.h>

#define SADP_COMMAND_MAX 	14		/* SADP信息中，所要登记的信息个数 */
#define SADP_COMMAND_LEN 	18		/* SADP信息中，所要登记信息的类别名长度 */
#define SADP_INFO_LEN		48		/* SADP信息中，所要登记信息的最大长度 */

static const char szCommand[SADP_COMMAND_MAX][SADP_COMMAND_LEN] =
{
	"DeviceType",
	"DeviceDescription",
	"DeviceSN",
	"CommandPort",
	"HttpPort",
	"MAC",
	"IPv4Address",
	"IPv4SubnetMask",
	"IPv4Gateway",
	"IPv6Address",
	"IPv6Gateway",
	"DHCP",
	"SoftwareVersion",
	"DHCPAbility"
};

BOOL sadp_recv_info_handle(const char *szRecvInfo)
{
	unsigned int i = 0;
	unsigned int iPreIndex = 0; 
	unsigned int iNextIndex = 0; 
	unsigned int iCopyLen[SADP_COMMAND_MAX] ; /* 记录每一个信息体的长度 */
	
	SADP_INFO_RECV_T *stSadpInfo = malloc(sizeof(SADP_INFO_RECV_T));
	
	char szInfoTmp[SADP_COMMAND_MAX][SADP_INFO_LEN];
	
	
	if( 0 == strspn(szRecvInfo, &(szCommand[i][0])) )
	{
		SEND_DEBUG("recieved invalid message!");
		return TRUE;
	}
	
	memset( stSadpInfo, 0, sizeof(stSadpInfo) );	
	memset( szInfoTmp, 0, sizeof(szInfoTmp) );
	memset( iCopyLen, 0, sizeof(iCopyLen) );
		
	
	if(g_stGlobalQueue == NULL)
	{
		SEND_ERROR("g_stGlobalQueue is NULL!");   
		return FALSE;
	}	
	
	ABORT_DEBUG("szRecvInfo = %s",szRecvInfo);
	/* 协议处理 */
	for( i=0; i<SADP_COMMAND_MAX; i++)
	{
		/* 在收新信息中，进行协议头匹配，找出对应信息的主体 */
		iPreIndex  = 1+strspn(szRecvInfo, &(szCommand[i][0]));
		SEND_DEBUG("TCW TEST2_1!");
		iNextIndex = strspn(szRecvInfo+iPreIndex, &(szCommand[i][0]));
		SEND_DEBUG("TCW TEST2_2!");
		iCopyLen[i]= iNextIndex-strlen(&(szCommand[i][0]))-2;
		SEND_DEBUG("TCW TEST2_3!");		
		ABORT_DEBUG("iPreIndex = %d,iCopyLen[i] = %d!",iPreIndex,iCopyLen[i]);		
		memcpy( &(szInfoTmp[i][0]), szRecvInfo+iPreIndex, iCopyLen[i] );
		SEND_DEBUG("TCW TEST2_4!");
		szInfoTmp[i][iCopyLen[i]] = '\0';
		SEND_DEBUG("TCW TEST2_5!");
		iCopyLen[i] += 1;
		szRecvInfo = szRecvInfo+iNextIndex;
	}
	SEND_DEBUG("TCW TEST2!");
	/* 协议搬移 */
	memcpy( stSadpInfo->ucDevType, 		  &(szInfoTmp[0][0]), iCopyLen[0] );
	memcpy( stSadpInfo->ucDevDescription, &(szInfoTmp[1][0]), iCopyLen[1] );
	memcpy( stSadpInfo->ucDevSN, 		  &(szInfoTmp[2][0]), iCopyLen[2] );
	memcpy( stSadpInfo->ucCommandPort, 	  &(szInfoTmp[3][0]), iCopyLen[3] );
	memcpy( stSadpInfo->ucHttpPort,       &(szInfoTmp[4][0]), iCopyLen[4] );
	memcpy( stSadpInfo->ucMacAddr, 		  &(szInfoTmp[5][0]), iCopyLen[5] );
	memcpy( stSadpInfo->ucIPv4Address,    &(szInfoTmp[6][0]), iCopyLen[6] );
	memcpy( stSadpInfo->ucIPv4SubnetMask, &(szInfoTmp[7][0]), iCopyLen[7] );
	memcpy( stSadpInfo->ucIPv4Gateway, 	  &(szInfoTmp[8][0]), iCopyLen[8] );
	memcpy( stSadpInfo->ucIPv6Address,    &(szInfoTmp[9][0]), iCopyLen[9] );
	memcpy( stSadpInfo->ucIPv6Gateway,    &(szInfoTmp[10][0]),iCopyLen[10]);
	memcpy( stSadpInfo->ucDHCP,    		  &(szInfoTmp[11][0]),iCopyLen[11]);
	memcpy( stSadpInfo->ucSoftwareVersion,&(szInfoTmp[12][0]),iCopyLen[12]);
	memcpy( stSadpInfo->ucDHCPAbility,    &(szInfoTmp[13][0]),iCopyLen[12]);
	
	ABORT_DEBUG("TCW TEST:\n %s",stSadpInfo);
	
	while( QUEUE_FULL == writeQueue( stSadpInfo, g_stGlobalQueue ) )
	{
		/* 队列满则等待 */
		SEND_WARN("Queue is full!");
		sleep(1);
	}
	
	return TRUE;
}