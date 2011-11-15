
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "types.h"

typedef struct _node_t {
  void    *data;
  _node_t *next;
} node_t;

typedef struct _queue_t {
  node_t *head, *tail;
} queue_t;

queue_t* queue_malloc();
void queue_free(queue_t *queue);
void queue_push(queue_t *queue, void *data);
void* queue_pop(queue_t *queue);

#endif /* _HASH_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
