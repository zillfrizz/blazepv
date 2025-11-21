    #pragma once
    #include <stdint.h>
    #include <stdalign.h>

    typedef struct Tab{
        size_t tabSize;
        size_t front;
        void* data;
    } Tab;

    typedef struct TabCoherent{
        size_t tabSize;
        size_t front;
        void* data;
        uint8_t _padding[64 - ((sizeof(size_t) * 2 + sizeof(void*)) % 64)];
    } __attribute__((aligned(64))) TabCoherent; 

    Tab* tab_init(size_t initSize, const size_t elemSize);
    Tab* tab_init_coherent(size_t initSize, const size_t elemSize);
    void tab_free(Tab* t);
    void tab_erase(Tab* t);
    //POP
    void tab_pop(Tab* t, const size_t elemSize, void* popped);
    //PUSH
    void tab_push(Tab*t, const size_t elemSize, const void* data);
    void tab_push_2X(Tab*t, const size_t elemSize, const void* data);
    //GROWTH
    void tab_growth_2X(Tab* t, const size_t elemSize);
    


