
#ifndef _TIMER_H_
#define _TIMER_H_

#include <sys/time.h>

typedef struct {
  timeval start, end;
} precision_timer_t;

void timer_start(precision_timer_t *timer);
void timer_end(precision_timer_t *timer);
double timer_elapsed_time(precision_timer_t *timer);
void print_elapsed_time(precision_timer_t timer);
void print_elapsed_time(double elapsed);

#endif /* _TIMER_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
