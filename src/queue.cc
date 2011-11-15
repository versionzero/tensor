
#include "queue.h"
#include "error.h"
#include "memory.h"
#include "utility.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

queue_t*
queue_malloc()
{
  queue_t *queue;
  
  superfluous("queue_malloc(max_size=%d)\n", max_size);
  
  queue        = MALLOC(queue_t);
  queue->first = 0;
  queue->last  = MAX_QUEUE_SIZE-1;
  queuecount   = 0;
  
  thread_mutex_init(&queue->lock);
  
  return queue;
}

void
queue_free(queue_t *queue)
{
  superfluous("queue_free(queue=0x%x)\n", queue);
  
  thread_mutex_destroy(&queue->lock);
  safe_free(queue);
}
void
queue_push(queue_t *queue, queue_node_t *node, uint x)
{
  debug("queue_update(queue=0x%x, node=0x%x, data=0x%x)\n", queue, node, data);
  
  thread_mutex_lock(&queue->lock);
  
  queue->last              = (queue->last+1) % MAX_QUEUE_SIZE; 
  queue->data[queue->last] = x;
  queue->count++;
  
  thread_mutex_unlock(&queue->lock);
}

void
queue_pop(queue_t *queue)
{
  int current, x;

  thread_mutex_lock(&queue->lock);
  
  x            = queue->data[queue->first];
  queue->first = (queue->first+1) % QUEUE_SIZE;
  queue->count--;
  
  return x;
}


