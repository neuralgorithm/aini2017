#include <stdio.h>
#include <sys/time.h>

# define timersub(a, b, result)						      \
  do {									      \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;			      \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;			      \
    if ((result)->tv_usec < 0) {					      \
      --(result)->tv_sec;						      \
      (result)->tv_usec += 1000000;					      \
    }									      \
  } while (0)

static struct timeval start, stop;

void timer_start(void) {
  gettimeofday(&start, NULL);
}

double timer_elapsed(void) {
  struct timeval elapsed;
  gettimeofday(&stop, NULL);
  timersub(&stop, &start, &elapsed);
  return ((elapsed.tv_sec*1000.0 + elapsed.tv_usec/1000.0)/1000.0);
}
