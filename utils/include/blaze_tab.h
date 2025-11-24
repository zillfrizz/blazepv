    #pragma once
    #include <stdint.h>
    #include <stdalign.h>

    typedef struct BlazeTab{
        size_t tabSize;
        size_t front;
        void* data;
    } BlazeTab;

    typedef struct BlazeTabCoherent{
        size_t tabSize;
        size_t front;
        void* data;
        uint8_t _padding[64 - ((sizeof(size_t) * 2 + sizeof(void*)) % 64)];
    } __attribute__((aligned(64))) BlazeTabCoherent; 

    void blaze_tab_create(BlazeTab* tab, size_t initSize, const size_t elemSize);
    void blaze_tab_coherent_create(BlazeTab* tab, size_t initSize, const size_t elemSize);
    void blaze_tab_free(BlazeTab* t);
    void blaze_tab_erase(BlazeTab* t);
    //POP
    void blaze_tab_pop(BlazeTab* t, const size_t elemSize, void* popped);
    //PUSH
    void blaze_tab_push(BlazeTab*t, const size_t elemSize, const void* data);
    void blaze_tab_push_2X(BlazeTab*t, const size_t elemSize, const void* data);
    //GROWTH
    void blaze_tab_growth_2X(BlazeTab* t, const size_t elemSize);
    


