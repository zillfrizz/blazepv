#include <blaze_bucket.h>
#include <blaze_tab.h>

#define BUCKET_COUNTERS_SIZE uint32_t
#define BUCKET_COUNTERS_OCTAL_SIZE 4

void blaze_bucket_allocation_create(BucketAllocation* bucketAllocation, uintptr_t handle, uint32_t size, uint32_t binaryScope, uint32_t bucketCount){
    (*bucketAllocation).handle = handle;
    (*bucketAllocation).size = size >> binaryScope;
    (*bucketAllocation).binaryScope = binaryScope;
    (*bucketAllocation).head = 0;
    (*bucketAllocation).buckets = malloc(sizeof(Bucket) * bucketCount);
    for(int i = 0; i < bucketCount; i++){
        blaze_tab_create(&(*bucketAllocation).buckets[i].ids, 2, sizeof(BUCKET_COUNTERS_SIZE));
    }
}

void blaze_bucket_allocate(BucketAllocation* bucketAllocation, uint32_t size, void* map){
    size >>= (*bucketAllocation).binaryScope;
    uint16_t id = 0;
    while(size > 1){id >>= 1;}
    if((*bucketAllocation).buckets[id].ids.front){blaze_tab_pop(&(*bucketAllocation).buckets[id].ids, BUCKET_COUNTERS_OCTAL_SIZE, map);}
    else if((*bucketAllocation).head != (*bucketAllocation).size){(*bucketAllocation).head++; }
}