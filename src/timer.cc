
#include "error.h"
#include "timer.h"
#include "utility.h"

extern bool human_readable;

void
timer_start(precision_timer_t *timer) 
{
  gettimeofday(&timer->start, NULL);
}

void
timer_end(precision_timer_t *timer)
{
  gettimeofday(&timer->end, NULL);
}

double
timer_elapsed_time(precision_timer_t timer)
{
  double elapsed;
  
  elapsed  = (timer.end.tv_sec  - timer.start.tv_sec)  * 1000.0; /* sec to ms */
  elapsed += (timer.end.tv_usec - timer.start.tv_usec) / 1000.0; /* us  to ms */
  
  return elapsed;
}

void
print_elapsed_time(double elapsed)
{
  if (human_readable) {
    progress("done [%lf]\n", elapsed);
  } else {
    message("%lf ", elapsed);
  }
}

void
print_elapsed_time(precision_timer_t timer)
{
  double elapsed;
  
  elapsed = timer_elapsed_time(timer);
  print_elapsed_time(elapsed);
}
