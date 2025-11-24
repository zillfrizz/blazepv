#pragma once
#include <stdint.h>
#include <blaze_tab.h>

typedef struct BSThree{
    BSTNode* main;
} BSThree;

typedef struct BSTNode{
    void* value;
    BSTNode* left;
    BSTNode* right;
} BSTNode;

void blaze_bst_create(BSThree* three);
void blaze_bst_delete(BSThree* three);
void* blaze_bst_search(BSThree* three, uint32_t valueSize, void* value);
void blaze_bst_push(BSThree* three, uint32_t valueSize, void* value, void* data);