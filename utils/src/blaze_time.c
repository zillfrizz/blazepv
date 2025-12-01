#include <blaze_time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void blaze_time_init(void){

}

time_t blaze_time_get(void){
    return time(0);
}

time_t blaze_time_compare(time_t srcTime, time_t dstTime){
    return dstTime - srcTime;
}

time_t blaze_time_compare_current(time_t srcTime){
    return time(0) - srcTime;
}

char* blaze_time_string(time_t srcTime) {
    const struct tm ltime = *localtime(&srcTime);

    int buffer_size = 30; 
    char *output = malloc(buffer_size);
    if (!output) {
        perror("time string malloc failed\n");
        return 0;
    }

    sprintf(output, "%d-%d-%d %d:%d:%d",
            ltime.tm_year + 1900,
            ltime.tm_mon + 1,
            ltime.tm_mday,
            ltime.tm_hour,
            ltime.tm_min,
            ltime.tm_sec);

    return output;
}

char* time_string_now(){
    return time_string(time_get());
}
