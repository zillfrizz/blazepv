#include <blaze_tab.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
    
    //INIT
    void blaze_tab_create(BlazeTab* tab, size_t initSize, const size_t elemSize){
        tab = malloc(sizeof(BlazeTab));
        assert(tab);
        tab->tabSize = initSize;
        tab->front = 0;
        tab->data = malloc(elemSize * tab->tabSize);
        assert(tab->data);
    }

    void blaze_tab_coherent_create(BlazeTab* tab, size_t initSize, const size_t elemSize){
        tab = malloc(sizeof(BlazeTabCoherent));
        assert(tab);
        tab->tabSize = initSize;
        tab->front = 0;
        tab->data = malloc(elemSize * tab->tabSize);
        assert(tab->data);
    }

    //FREE
    void blaze_tab_free(BlazeTab* t){
        assert(t);
        free(t->data);
        t->data = NULL;
        t->tabSize = 0;
        t->front = 0;
        free(t);
    }

    //ERASE
    void blaze_tab_erase(BlazeTab* t){
        assert(t);
        t->front = 0;
    }

    //POP
    void blaze_tab_pop(BlazeTab* t, const size_t elemSize, void* data){
        assert(t);
        assert(t->front > 0);
        memcpy(data, t->data + --(t->front), elemSize);
    }

    //PUSH
    void blaze_tab_push(BlazeTab* t, const size_t elemSize, const void* data){
        assert(t);
        assert(t->front < t->tabSize);
        memcpy(t->data + (t->front)++, data, elemSize);
    }

    void blaze_tab_push_2X(BlazeTab*t, const size_t elemSize, const void* data){
        assert(t);
        if(t->tabSize == t->front)
            tab_growth_2X(t, elemSize);
        memcpy(t->data + (t->front)++, data, elemSize);
    }

    //GROWTH
    void blaze_tab_growth_2X(BlazeTab* t, const size_t elemSize){
        assert(t);
        t->tabSize *= 2;
        t->data = realloc(t->data, elemSize * t->tabSize);
        assert(t->data);
    }