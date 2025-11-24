#pragma once
#include <stdint.h>
#include <blaze_tab.h>

typedef struct BucketAllocation{
    uintptr_t handle; 
    uint16_t binaryScope;
    uint32_t size;
    uint32_t head;
    Bucket* buckets;
} BucketAllocation;

typedef struct Bucket {
    BlazeTab ids;
} Bucket;

void blaze_bucket_allocation_create(BucketAllocation* bucketAllocation, uintptr_t handle, uint32_t size, uint32_t binaryScope, uint32_t bucketCount);
void blaze_bucket_allocate(BucketAllocation* bucketAllocation, uint32_t size, void* map);