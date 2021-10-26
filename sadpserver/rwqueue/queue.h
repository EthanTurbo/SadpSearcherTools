#ifndef __QUEUE_H__
#define __QUEUE_H__

 
#include "common.h"  
 
 
COMMQUEUE_T *g_stGlobalQueue;

 
static int isFull(COMMQUEUE_T *queue);
static int isEmpty(COMMQUEUE_T *queue);  

QUEUESTATUS_T init_queue(COMMQUEUE_T* queue);  
 
static QUEUESTATUS_T inQueue(const SADP_INFO_RECV_T *val, COMMQUEUE_T* queue);  

static QUEUESTATUS_T outQueue(COMMQUEUE_T* queue, SADP_INFO_RECV_T *val);  
  
QUEUESTATUS_T readQueue(SADP_INFO_RECV_T *buf, COMMQUEUE_T *queue);  
 
QUEUESTATUS_T writeQueue(const SADP_INFO_RECV_T *buf, COMMQUEUE_T *queue);


#endif