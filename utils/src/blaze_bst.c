/*#include <blaze_bst.h>
#include <stdlib.h>
#include <string.h>

void blaze_bst_create(BSThree* three){
    blaze_tab_create(&(*three).main, 1, sizeof(BSTNode));
}

void blaze_bst_delete(BSThree* three){
    blaze_tab_erase(&(*three).pool);
    free(three);
}

void* blaze_bst_search(BSThree* three, uint32_t valueSize, void* value){
    BSTNode* node = &(*three).pool;
    while(node){
        int r = memcmp((char*)(*node).data, value, valueSize);
        if(!r){return (*node).data;}
        else if (r > 0){node = (*node).left;}
        else{node = (*node).right;}
    }
}

void* blaze_bst_fit_contain_search(BSThree* three, uint32_t valueSize, void* value){
    BSTNode* node = &(*three).pool;
    BSTNode* candidate;;
    while(node){
        int r = memcmp((char*)(*node).data, value, valueSize);
        if(!r){return node;}   
        // CLEF RECHERCHEE PLUS PETITE     
        else if(r > 0){candidate = node; node = node->left;}
        // CLEF RECHERCHEE PLUS GROSSE
        else{node = node->right;}
    }
    return candidate;
}

void blaze_bst_push(BSThree* three, uint32_t valueSize, void* value, void* data){
    BSTNode* node = &(*three).pool;
    while(node){
        int r = memcmp((char*)(*node).data, value, valueSize);
        if (r > 0){
            if(!(*node).left){
                blaze_tab_push_2X(&(*three).pool, sizeof(BSTNode), data);
                (*node).left = (char*)&(*three).pool + (*three).pool.front * sizeof(BSTNode);
                return;
            }
            node = (*node).left;
        }
        else{   

            node = (*node).right;}
    }


}*/