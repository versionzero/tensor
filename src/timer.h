
#ifndef _TIME_H_
#define _TIME_H_

#include <sys/time.h>

typedef struct {
  timeval start, end;
} timer_t;

void timer_start(timer_t *timer);
void timer_end(timer_t *timer);
double timer_elapsed_time(timer_t *timer);
void print_elapsed_time(timer_t timer);
void print_elapsed_time(double elapsed);

#endif /* _TIME_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
