    #pragma once
    #include <stdint.h>
    #include <stdalign.h>

    typedef struct BzTab{
        size_t tabSize;
        size_t front;
        void* data;
    } BzTab;

    typedef struct BzTabCoherent{
        size_t tabSize;
        size_t front;
        void* data;
        uint8_t _padding[64 - ((sizeof(size_t) * 2 + sizeof(void*)) % 64)];
    } __attribute__((aligned(64))) BzTabCoherent; 

    void blaze_tab_create(BzTab* tab, size_t initSize, const size_t elemSize);
    void blaze_tab_coherent_create(BzTab* tab, size_t initSize, const size_t elemSize);
    void blaze_tab_free(BzTab* t);
    void blaze_tab_erase(BzTab* t);
    //POP
    void blaze_tab_pop(BzTab* t, const size_t elemSize, void* popped);
    //PUSH
    void blaze_tab_push(BzTab*t, const size_t elemSize, const void* data);
    void blaze_tab_push_2X(BzTab*t, const size_t elemSize, const void* data);
    //GROWTH
    void blaze_tab_growth_2X(BzTab* t, const size_t elemSize);
    


