#ifndef _PLX_QUEUE_H_
#define _PLX_QUEUE_H_


/******************************************************************************
 *
 * File Name:
 *
 *      PlxQueue.h
 *
 * Description:
 *
 *      Header for the PLX Queue helper functions
 *
 ******************************************************************************/


#include "PlxTypes.h"


#ifdef __cplusplus
extern "C" {
#endif




/*******************************************
*          Definitions
*******************************************/
typedef struct _PLX_QUEUE
{
    volatile U16  head;
    volatile U16  tail;
    volatile U16  size;
    volatile U8  *pBuffer;
} PLX_QUEUE;




/*******************************************
*        Public Functions
*******************************************/
void
PlxQueueInitialize(
    PLX_QUEUE *pQueue,
    U16        size,
    U8        *pBuffer
    );

void
PlxQueueAdd(
    PLX_QUEUE *pQueue,
    U8         data
    );

U8
PlxQueueRemove(
    PLX_QUEUE *pQueue
    );

void
PlxQueueFlush(
    PLX_QUEUE *pQueue
    );

void
PlxQueueAppendAndFlush(
    PLX_QUEUE *pQueue,
    PLX_QUEUE *pCopyQueue
    );

BOOLEAN
PlxQueueIsEmpty(
    PLX_QUEUE *pQueue
    );

BOOLEAN
PlxQueueIsFull(
    PLX_QUEUE *pQueue
    );



#ifdef __cplusplus
}
#endif

#endif
