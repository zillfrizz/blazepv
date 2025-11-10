#include "tab.h"
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
    
    //INIT
    Tab* tab_init(size_t initSize, const size_t elemSize){
        Tab* t = malloc(sizeof(Tab));
        assert(t);
        t->tabSize = initSize;
        t->front = 0;
        t->data = malloc(elemSize * t->tabSize);
        assert(t->data);
        return t;
    }

    Tab* tab_init_coherent(size_t initSize, const size_t elemSize){
        Tab* t = malloc(sizeof(TabCoherent));
        assert(t);
        t->tabSize = initSize;
        t->front = 0;
        t->data = malloc(elemSize * t->tabSize);
        assert(t->data);
        return t;
    }

    //FREE
    void tab_free(Tab* t){
        assert(t);
        free(t->data);
        t->data = NULL;
        t->tabSize = 0;
        t->front = 0;
        free(t);
    }

    //ERASE
    void tab_erase(Tab* t){
        assert(t);
        t->front = 0;
    }

    //POP
    void tab_pop(Tab* t, const size_t elemSize, void* data){
        assert(t);
        assert(t->front > 0);
        memcpy(data, t->data + --(t->front), elemSize);
    }

    //PUSH
    void tab_push(Tab* t, const size_t elemSize, const void* data){
        assert(t);
        assert(t->front < t->tabSize);
        memcpy(t->data + (t->front)++, data, elemSize);
    }

    void tab_push_2X(Tab*t, const size_t elemSize, const void* data){
        assert(t);
        if(t->tabSize == t->front)
            tab_growth_2X(t, elemSize);
        memcpy(t->data + (t->front)++, data, elemSize);
    }

    //GROWTH
    void tab_growth_2X(Tab* t, const size_t elemSize){
        assert(t);
        t->tabSize *= 2;
        t->data = realloc(t->data, elemSize * t->tabSize);
        assert(t->data);
    }