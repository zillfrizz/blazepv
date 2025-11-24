#pragma once

#include <time.h>

void blaze_time_init(void);
time_t blaze_time_get(void);
time_t blaze_time_compare(time_t srcTime, time_t dstTime);
time_t blaze_time_compare_current(time_t srcTime);
char* blaze_time_string(time_t srcTime);
char* blaze_time_string_now(void);