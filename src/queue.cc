
#include "queue.h"
#include "error.h"
#include "memory.h"
#include "utility.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

queue_t*
queue_malloc(void)
{
  queue_t *q;
  
  q                 = MALLOC(queue_t);
  q->head = q->tail = MALLOC(node_t);
  
  return q;
}

void
queue_push(queue_t *q, void *data)
{
  node_t *node, *tail, *next;
  
  node       = MALLOC(node_t);
  node->data = data;
  node->next = NULL;
  
  while (true) {
    
    tail = q->tail;
    next = tail->next;
    
    if (tail != q->tail) {
      continue;
    }
    
    if (NULL != next) {
      __sync_bool_compare_and_swap(&q->tail, tail, next);
      continue;
    }
    
    if (__sync_bool_compare_and_swap(&tail->next, NULL, node)) {
      break;
    }
    
  }
  
  __sync_bool_compare_and_swap(&q->tail, tail, node);
}

void*
queue_pop(queue_t *q)
{
  void   *data;
  node_t *head, *tail, *next;
  
  while (true) {
    
    head = q->head;
    tail = q->tail;
    next = head->next;
    
    if (head != q->head) {
      continue;
    }
    
    if (NULL == next) {
      return NULL; // Empty
    }
    
    if (head == tail) {
      __sync_bool_compare_and_swap(&q->tail, tail, next);
      continue;
    }
    
    data = next->data;
    
    if (__sync_bool_compare_and_swap(&q->head, head, next)) {
      break;
    }
    
  }
  
  safe_free(head);
  
  return data;
}


