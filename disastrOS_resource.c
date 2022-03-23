#include <assert.h>
#include <stdio.h>
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "pool_allocator.h"
#include "linked_list.h"

#include "disastrOS_globals.h"

#define RESOURCE_SIZE sizeof(Resource)
#define RESOURCE_MEMSIZE (sizeof(Resource)+sizeof(int))
#define RESOURCE_BUFFER_SIZE MAX_NUM_RESOURCES*RESOURCE_MEMSIZE

#define MSGQUEUE_SIZE sizeof(MsgQueue)
#define MSGQUEUE_MEMSIZE (sizeof(MsgQueue)+sizeof(int))
#define MSGQUEUE_BUFFER_SIZE MAX_NUM_MSGQUEUES*MSGQUEUE_MEMSIZE

#define MSG_STRING_SIZE sizeof(char) * MAX_MESSAGE_SIZE
#define MSG_STRING_MEMSIZE (MSG_STRING_SIZE +sizeof(int))
#define MSG_STRING_BUFFER_SIZE MAX_NUM_STRINGS*MSG_STRING_MEMSIZE

#define MSGQUEUE_USER_SIZE sizeof(MsgQueueForProcess)
#define MSGQUEUE_USER_MEMSIZE (sizeof(MsgQueueForProcess)+sizeof(int))
#define MSGQUEUE_USER_BUFFER_SIZE MAX_NUM_PROCESSES*MSGQUEUE_USER_MEMSIZE

#define MESSAGE_SIZE sizeof(Message)
#define MESSAGE_MEMSIZE (sizeof(Message)+sizeof(int))
#define MAX_NUM_MESSAGES (MAX_NUM_MESSAGES_PER_MSGQUEUE*MAX_NUM_MSGQUEUES)
#define MESSAGE_BUFFER_SIZE MAX_NUM_MESSAGES*MESSAGE_MEMSIZE

static char _resources_buffer[RESOURCE_BUFFER_SIZE];
static PoolAllocator _resources_allocator;

static char _queues_buffer[MSGQUEUE_BUFFER_SIZE];
static PoolAllocator _queues_allocator;

static char _msg_strings_buffer[MSG_STRING_BUFFER_SIZE];
static PoolAllocator _msg_strings_allocator;

static char _queue_users_buffer[MSGQUEUE_BUFFER_SIZE];
static PoolAllocator _queue_users_allocator;

static char _message_buffer[MESSAGE_BUFFER_SIZE];
static PoolAllocator _message_allocator;


// General resource section

void Resource_init(){
    int result = PoolAllocator_init(& _resources_allocator,
				  RESOURCE_SIZE,
				  MAX_NUM_RESOURCES,
				  _resources_buffer,
				  RESOURCE_BUFFER_SIZE);
    assert(! result);
}

Resource* Resource_alloc(int id, int type){
  Resource* r = (Resource*) PoolAllocator_getBlock(&_resources_allocator);
  if (!r)
    return 0;
  r -> list.prev = r -> list.next = 0;
  r -> id = id;
  r -> type = type;
  List_init(&r -> descriptors_ptrs);
  return r;
}

int Resource_free(Resource* r) {
  assert(r -> descriptors_ptrs.first == 0);
  assert(r -> descriptors_ptrs.last == 0);
  return PoolAllocator_releaseBlock(&_resources_allocator, r);
}

Resource* ResourceList_byId(ResourceList* l, int id) {
  ListItem* aux = l -> first;
  while(aux){
    Resource* r = (Resource*)aux;
    if (r -> id == id)
      return r;
    aux = aux -> next;
  }
  return 0;
}

// Debug
void Resource_print(Resource* r) {
  printf("id: %d, type:%d, pids:", r->id, r->type);
  if (r->type == 2)
    Queue_print((Queue*) r -> value);
  else
    DescriptorPtrList_print(&r -> descriptors_ptrs);
}

void ResourceList_print(ListHead* l){
  ListItem* aux = l -> first;
  printf("{\n");
  while(aux){
    Resource* r = (Resource*)aux;
    printf("\t");
    Resource_print(r);
    if(aux -> next)
      printf(",");
    printf("\n");
    aux = aux -> next;
  }
  printf("}\n");
}

// MSGQUEUS IMPLEMENTATION
// use pool allocator function to initialize the msgQueue
void MsgQueue_init(){
  int result=PoolAllocator_init(& _queues_allocator,
				  MSGQUEUE_SIZE,
				  MAX_NUM_MSGQUEUES,
				  _queues_buffer,
				  MSGQUEUE_BUFFER_SIZE);
    assert(! result);
}

// after initialize, we may alloc memory for our message queue
MsgQueue* MsqQueue_alloc(){
  MsgQueue* q=(MsgQueue*) PoolAllocator_getBlock(&_queues_allocator);
  if (!q)
    return 0;
  List_init(&q -> messages);
  List_init(&q -> processReaders);
  List_init(&q -> processWriters);
  List_init(&q -> non_block);
  q -> maxMsgQueue = MAX_NUM_MESSAGES_PER_MSGQUEUE;
  q -> maxSizeMsgQueue = MAX_MESSAGE_SIZE;
  q -> nMsgQueueProcesses = 0;
  return q;
}

// add a process user to nMsgQueueProcesses 
int MsqQueue_add_pid(MsgQueue* q, int pid, int mode, ListItem** ds){

}

// to free memory, we need to empty ListItem in MsgQueue and then free the PoolAllocator too  
int MsqQueue_free(MsgQueue* queue){

  // perform a pop for every message 
  for (int i = 0; i < q->messages.size; i++){
    Message_free((Message*) List_pop(&q->messages));
  }
  // same thing with others lists
  while(queue -> processReaders.first != 0) MsgQueueForProcess_free((MsgQueueForProcess*) List_pop(&queue->processReaders));
  while(queue -> processWriters.first != 0) MsgQueueForProcess_free((MsgQueueForProcess*) List_pop(&queue->processWriters));
  while(queue -> non_block.first != 0) MsgQueueForProcess_free((MsgQueueForProcess*) List_pop(&queue->non_block));
  
  // at the end we can free PoolAllocator
  return PoolAllocator_releaseBlock(&_queues_allocator, q);
}
