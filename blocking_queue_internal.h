#ifndef _blocking_queue_internal_h
#define _blocking_queue_internal_h

#include "blocking_queue.h"
#include "simplethread/simple_thread.h"

/* internal API */

struct blocking_queue_t
{
	QueueNode *head;
	QueueNode *tail;
	/* this element is ALWAYS on the queue to avoid null checks */
	QueueNode dummy_head;
	ThreadConditionVar *queue_empty;
	ThreadLock *head_lock;
	ThreadLock *tail_lock;
	int size;
	int capacity;
};

#endif
