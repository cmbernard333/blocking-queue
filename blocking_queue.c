#include <stdlib.h>
#include "blocking_queue_internal.h"

#define QUEUE_POISON1 ((void*)0xCAFEBAB5)

void BlockingQueue_signal_consumers(int signal_mode);

int BlockingQueue_init(BlockingQueue **_bqueue)
{
	int rc = BQUEUE_SUCCESS;
	BlockingQueue *bqueue = NULL;
	bqueue = calloc(1,sizeof(BlockingQueue));
	if(!bqueue)
	{
		return BQUEUE_NOMEM;
	}

	/* initialize condition and lock */
	rc = ThreadConditionVar_init(&bqueue->queue_empty);
	if(rc)
	{
		return BQUEUE_NOMEM;
	}
	/* lock for write */
	rc = ThreadLock_init(&bqueue->head_lock);
	if(rc)
	{
		return BQUEUE_NOMEM;
	}

	/* lock for read */
	rc = ThreadLock_init(&bqueue->tail_lock);
	if(rc)
	{
		return BQUEUE_NOMEM;
	}

	/* setup queue */
	bqueue->dummy_head.next = NULL;
	bqueue->head = &bqueue->dummy_head;
	bqueue->tail = &bqueue->dummy_head;
	bqueue->size = 0;
	/* special value to denote infinite capacity */
	bqueue->capacity = -1;

	*_bqueue = bqueue;

	return rc;
}

int BlockingQueue_destroy(BlockingQueue *bqueue)
{
	if(!bqueue)
	{
		return BQUEUE_INVALID_ARG;
	}
	/* cleanup the condition */
	ThreadConditionVar_cleanup(bqueue->queue_empty);
	/* cleanup the lock(s) */
	ThreadLock_cleanup(bqueue->head_lock);
	ThreadLock_cleanup(bqueue->tail_lock);
	
	free(bqueue);
	return BQUEUE_SUCCESS;
}

int BlockingQueue_put(BlockingQueue *bqueue, QueueNode *node)
{
	/* condition wait infinite */
	if(!bqueue|!node)
	{
		return BQUEUE_INVALID_ARG;
	}
	node->next = NULL;
	ThreadLock_lock(bqueue->tail_lock);
	/* check the capacity size */
	if(bqueue->capacity > 0 && bqueue->size +1 > bqueue->capacity)
	{
		return BQUEUE_NOMEM;
	}
	bqueue->tail->next = node;
	bqueue->tail = node;
	bqueue->size+= 1;
	ThreadConditionVar_signal_all(bqueue->queue_empty);
	ThreadLock_unlock(bqueue->tail_lock);
	return BQUEUE_SUCCESS;

}

int BlockingQueue_offer(BlockingQueue *bqueue, QueueNode *node, unsigned long ms_wait)
{
	/* condition wait with a timeout */
	if(!bqueue|!node|!ms_wait<0)
	{
		return BQUEUE_INVALID_ARG;
	}
	node->next = NULL;
	ThreadLock_lock(bqueue->tail_lock);
	bqueue->tail->next = node;
	bqueue->tail = node;
	bqueue->size+=1;
	ThreadConditionVar_signal_all(bqueue->queue_empty);
	ThreadLock_unlock(bqueue->tail_lock);
	return BQUEUE_SUCCESS;
}

QueueNode *BlockingQueue_take(BlockingQueue *bqueue)
{
	/* condition wait infinite */
	if(!bqueue)
	{
		return BQUEUE_INVALID_ARG;
	}

	QueueNode *head, *next;

	while(1) {

		ThreadLock_lock(bqueue->head_lock);

		/* wait until elements are available */
		while(BlockingQueue_isEmpty(bqueue)) {
			ThreadConditionVar_wait_lock(bqueue->queue_empty,bqueue->head_lock);
		}

		head = bqueue->head;
		next = head->next;

		if(next==NULL)
		{
			/* Only dummy enqueued - queue is empty */
			ThreadLock_unlock(bqueue->head_lock);
			return NULL;
		}

		bqueue->head = next;
		bqueue->size-=1;
		ThreadLock_unlock(bqueue->head_lock);

		if(head == &bqueue->dummy_head) {
			BlockingQueue_put(bqueue, head);
			continue;
		}

		head->next = QUEUE_POISON1;
		return head;
	}
}

QueueNode *BlockingQueue_poll(BlockingQueue *bqueue, unsigned long ms_wait)
{
	/* condition wait with a time out */
	if(!bqueue)
	{
		return BQUEUE_INVALID_ARG;
	}
	return NULL;
}

int BlockingQueue_isEmpty(BlockingQueue *bqueue)
{
	return (bqueue->head->next == NULL);
}

int BlockingQueue_set_capacity(BlockingQueue *bqueue, size_t capacity)
{
	int rc = BQUEUE_SUCCESS;
	ThreadLock_lock(bqueue->tail_lock);
	if(capacity<0)
	{
		/* infinite capacity */
		capacity = -1;
	}
	else if(capacity == 0 )
	{
		/* no elements ? */
		rc = BQUEUE_INVALID_ARG;
	}
	else if(bqueue->size > capacity)
	{
		/* don't shrink the capacity if the pool already exceeds it */
		rc = BQUEUE_INVALID_ARG;
	}
	if(rc == BQUEUE_SUCCESS)
	{
		bqueue->capacity = capacity;
	}
	ThreadLock_unlock(bqueue->tail_lock);
	return rc;
}
