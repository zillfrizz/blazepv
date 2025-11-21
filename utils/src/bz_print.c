#include <bz_print.h>
#include <stdio.h>
#include <string.h>
#include <bz_time.h>

void print(char* data){
    printf("%s -> %s\n", time_string_now(), data);
}