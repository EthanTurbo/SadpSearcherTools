#include "queue.h"
#include "sadphandle.h"
#include <stdio.h>  
#include <stdlib.h>   
#include <unistd.h>
#include <string.h> 
#include "common.h"  
 
 

QUEUESTATUS_T init_queue(COMMQUEUE_T* queue)  
{  
	int i = 0;     
	if( NULL == queue )
	{
		return QUEUE_FAIL;
	}
    for( i=0;i<QUEUE_DATA_MAX;i++)
	{
		memset( &(queue->stSadpData[i]), 0, sizeof(queue->stSadpData[i]) );
	}
    
    queue->iWp = 0;  
    queue->iRp = 0;  
    
    queue->iQueueCnt = 0; // conuter  
	
	return QUEUE_OK;
}  
 
 static int isFull(COMMQUEUE_T *queue)  
{  
    return (queue->iQueueCnt == QUEUE_DATA_MAX - 1) ? 1 : 0;  
}
 
 
 static int isEmpty(COMMQUEUE_T *queue)  
{  
    return (0==queue->iQueueCnt)? 1 : 0;  
} 
 
static QUEUESTATUS_T inQueue(const SADP_INFO_RECV_T *val, COMMQUEUE_T *queue)  
{  
    if (1==isFull(queue))  
    {  
        return QUEUE_FULL;  
    }  
    else  
    {  
        memcpy( &(queue->stSadpData[queue->iWp]), val, sizeof(val));  
          
        if (++(queue->iWp) == QUEUE_DATA_MAX)  
        {  
            queue->iWp = 0;  
        }  
        
        (queue->iQueueCnt)++;  
    }     
    return QUEUE_OK;  
}  
 
static QUEUESTATUS_T outQueue(COMMQUEUE_T *queue, SADP_INFO_RECV_T *val)  
{  
    if (1==isEmpty(queue))  
    { 
        return QUEUE_EMPTY;  
    }  
    else 
	{  
        int index = queue->iRp;  
        if (++(queue->iRp) == QUEUE_DATA_MAX)  
        {  
            queue->iRp = 0;  
        }  
        (queue->iQueueCnt)--;  
        memcpy(val ,&(queue->stSadpData[index]), sizeof(queue->stSadpData[index]));  
    }     
    return QUEUE_OK;  
}  
 
QUEUESTATUS_T readQueue( SADP_INFO_RECV_T *buf, COMMQUEUE_T * queue )   
{  

    SADP_INFO_RECV_T val;  

	QUEUESTATUS_T ret = outQueue(queue, &val);  
	if (ret != QUEUE_EMPTY)   
	{  
		memcpy(buf, &val, sizeof(val));  
	} 

    return ret;  
}  
 
QUEUESTATUS_T writeQueue( const SADP_INFO_RECV_T *buf, COMMQUEUE_T *queue )  
{  
	QUEUESTATUS_T ret;
	/* 返回QUEUE_FULL/QUEUE_OK */
	ret = inQueue(buf, queue);  
	
	return ret;
}