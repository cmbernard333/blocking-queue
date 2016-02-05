#ifndef _bus_blocking_queue_h_
#define _bus_blocking_queue_h_

/* public API */

typedef struct queue_node_t QueueNode;
typedef struct blocking_queue_t BlockingQueue;

struct queue_node_t
{
	QueueNode *next;
};

/* 
	1. calculates the absolute position of a member within a struct
	2. subtracts the offset of the member from the given member pointer to calculate the address of the struct itself 
	3. casts that pointer to the requested type
*/
#define queue_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

int BlockingQueue_init(BlockingQueue **_bqueue);
int BlockingQueue_destroy(BlockingQueue *bqueue);

/* add - blocks */
int BlockingQueue_put(BlockingQueue *bqueue, QueueNode *node);
/* add - blocks until the lock can be acquired or the space becomes available. it will time out */
int BlockingQueue_offer(BlockingQueue *bqueue, QueueNode *node, unsigned long ms_wait);

/* retrieve - blocks */
QueueNode *BlockingQueue_take(BlockingQueue *bqueue);
/* retrieve - blocks until the lock can be acquired or an element becomes available. it will timeout */
QueueNode *BlockingQueue_poll(BlockingQueue *bqueue, unsigned long ms_wait);

int BlockingQueue_isEmpty(BlockingQueue *bqueue);

/* 
	capacity governs the number of elements you want this queue to hold.
	setting the capacity to less than the current number of elements will fail.
*/
int BlockingQueue_set_capacity(BlockingQueue *bqueue, size_t capacity);

#endif
