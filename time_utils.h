#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <sys/time.h>

long    get_absolute_ms(void);
long    get_relative_ms(long start_time);
void    ms_to_timespec(long timestamp_ms, struct timespec *ts);

#endif