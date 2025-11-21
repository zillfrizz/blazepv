#pragma once

#include <time.h>

void time_init(void);
time_t time_get(void);
time_t time_compare(time_t srcTime, time_t dstTime);
time_t time_compare_current(time_t srcTime);
char* time_string(time_t srcTime);
char* time_string_now(void);