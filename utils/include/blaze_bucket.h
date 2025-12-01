#pragma once
#include <stdint.h>
#include <blaze_tab.h>
#include <blaze_return.h>

typedef uint64_t BzMemory;
typedef uint64_t BzSize;

typedef struct BucketAllocation{
    uint64_t remaining; // REMAINING UNDEFINED MEMORY
    BzTab buckets; // BUCKETS
} BucketAllocation;

typedef struct Bucket {
    BzTab slots; // SLOT ADRESSES
} Bucket;

void blaze_bucket_allocator_create(BucketAllocation* bucketAllocation, BzSize size);
BzReturn blaze_bucket_allocate(BucketAllocation* bucketAllocation, BzSize size, BzMemory* map);
void blaze_bucket_unallocate(BucketAllocation* BucketAllocation, BzSize size, BzMemory* map);