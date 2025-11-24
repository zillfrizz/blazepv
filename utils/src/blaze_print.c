#include <blaze_print.h>
#include <stdio.h>
#include <string.h>
#include <blaze_time.h>

void blaze_print(char* data){
    printf("%s -> %s\n", time_string_now(), data);
}