
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "thread.h"
#include "types.h"

#define MAX_QUEUE_SIZE 100

typedef struct queue_tag {
  uint            data[MAX_QUEUE_SIZE];
  uint            first, last;
  pthread_mutex_t lock;
} queue_t;

queue_t* queue_malloc();
void queue_free(queue_t *queue);
void queue_push(queue_t *queue, uint x);
int queue_pop(queue_t *queue);

#endif /* _HASH_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
