#include <blaze_bucket.h>
#include <blaze_tab.h>

uint64_t blaze_bucket_get_msb(uint64_t i){
    int j = 0;
    while(i > 1){
        i >>= 1;
        j++;
    }
    return j;
}

uint64_t blaze_bucket_get_bucket_id(uint64_t i){
    uint64_t id = blaze_bucket_get_msb(i);
    if(!(i & (i - 1))){ id -= 1;}
    return id;
}

void blaze_bucket_allocator_create(BucketAllocation* bucketAllocation, BzSize size){
    (*bucketAllocation).remaining = size;
    blaze_tab_create(&(*bucketAllocation).buckets, 64, sizeof(Bucket));
    for(int i = 0; i < (*bucketAllocation).buckets.tabSize; i++){
        blaze_tab_create(&((Bucket*)(*bucketAllocation).buckets.data)[i].slots, 2, 64);
    }
}

void blaze_bucket_allocation_cleanup(BucketAllocation* bucketAllocation, uintptr_t handle){
    for(int i = 0; i < (*bucketAllocation).buckets.tabSize; i++){
        blaze_tab_free(&((Bucket*)(*bucketAllocation).buckets.data)[i].slots);
    }
    blaze_tab_free(&(*bucketAllocation).buckets);
}

BzReturn blaze_bucket_allocate(BucketAllocation* bucketAllocation, BzSize size, BzMemory* map){
    uint64_t id = blaze_bucket_get_bucket_id(size);
    if(((Bucket*)(*bucketAllocation).buckets.data)[id].slots.front){blaze_tab_pop(&((Bucket*)(*bucketAllocation).buckets.data)[id].slots, 64, map);}
    else if((*bucketAllocation).remaining >= size){*map = (*bucketAllocation).remaining; (*bucketAllocation).remaining += (0b10 << id);}
    else{return BLAZE_RETURN_ERROR;}
    return BLAZE_RETURN_SUCCESS;
}

void blaze_bucket_unallocate(BucketAllocation* bucketAllocation, BzSize size, BzMemory* map){
    uint64_t id = blaze_bucket_get_bucket_id(size);
    blaze_tab_push_2X(&((Bucket*)(*bucketAllocation).buckets.data)[id].slots, 64, map);
    *map = 0;
}