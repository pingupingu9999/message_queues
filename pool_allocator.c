#include "pool_allocator.h"

static const int NullIdx=-1;
static const int DetachedIdx=-2;

static const char* PoolAllocator_strerrors[]=
  {"Success",
   "NotEnoughMemory",
   "UnalignedFree",
   "OutOfRange",
   "DoubleFree",
   0
  };

const char* PoolAllocator_strerror(PoolAllocatorResult result) {
  return PoolAllocator_strerrors[-result];
}

PoolAllocatorResult PoolAllocator_init(PoolAllocator* p_alloc,
		       int item_size,
		       int num_items,
		       char* memory_block,
		       int memory_size) {

  //  let's see if we have enough memory to store it
  int requested_size= num_items*(item_size+sizeof(int));
  if (memory_size<requested_size){
    return NotEnoughMemory;
  }

  // setup fields for pool allocator
  p_alloc -> item_size=item_size;
  p_alloc -> size=num_items;
  p_alloc -> buffer_size=item_size*num_items;
  p_alloc -> size_max = num_items;
  
  p_alloc -> buffer = memory_block; // the upper part of the buffer is used as memory
  p_alloc -> free_list = (int*)(memory_block+item_size*num_items); // the lower part is for linked_list

  // populate the free list 
  for (int i = 0; i < p_alloc -> size-1; ++i){
    p_alloc -> free_list[i] = i+1;
  }
  // last element must be "NULL" 
  p_alloc -> free_list[p_alloc -> size-1] = NullIdx;
  p_alloc -> first_idx = 0;
  return Success;
}

void* PoolAllocator_getBlock(PoolAllocator* p_alloc) {
  if (p_alloc -> first_idx == -1){
    return 0;
  }

  // we need to remove the first bucket from the list
  int detached_idx = p_alloc -> first_idx;

  // advance the head
  p_alloc -> first_idx = p_alloc -> free_list[p_alloc -> first_idx];
  --(p_alloc -> size);
  
  p_alloc -> free_list[detached_idx] = DetachedIdx;
  
  //now we retrieve the pointer in the item buffer
  char* block_address = p_alloc -> buffer + (detached_idx * p_alloc -> item_size);
  return block_address;
}

PoolAllocatorResult PoolAllocator_releaseBlock(PoolAllocator* p_alloc, void* block_){
  //we need to find the index from the address
  char* block = (char*) block_;
  int offset = block - p_alloc->buffer;

  //sanity check, we need to be aligned to the block boundaries
  if (offset % p_alloc -> item_size)
    return UnalignedFree;

  int idx = offset / p_alloc -> item_size;

  //sanity check, are we inside the buffer?
  if (idx < 0 || idx >= p_alloc -> size_max)
    return OutOfRange;

  //is the block detached?
  if (p_alloc -> free_list[idx] != DetachedIdx)
    return DoubleFree;

  // insert in the head
  p_alloc -> free_list[idx] = p_alloc -> first_idx;
  p_alloc -> first_idx = idx;
  ++(p_alloc -> size);
  return Success;
}