#ifndef _blocking_queue_internal_h
#define _blocking_queue_internal_h

#include <nimbus.h>
#include <nim.h>

#include "simple_thread.h"
#include "blocking_queue.h"

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
