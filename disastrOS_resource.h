#pragma once
#include "linked_list.h"
#include "disastrOS_pcb.h"


// structs
typedef struct {
  ListItem list;
  int id;
  int type;
  ListHead descriptors_ptrs;
  void* value;
} Resource;

// struct used to refer to a process that is using a message queue
typedef struct {
  ListItem list;
  int pid;
  // can be either ready or running 
  ProcessStatus status;
} MsgQueueForProcess;

// def to refer to message char*
typedef char* MsgString;

typedef struct {
  ListItem list;
  MsgString msg;
  int len;
} Message;

// def to distinguish name across the structures
typedef ListHead MsgList;
typedef ListHead MsgQueueList;

typedef struct {
  // list of message currently in the queue
  MsgList messages;
  MsgQueueList processReaders;
  MsgQueueList processWriters;
  MsgQueueList non_block;
  
  // the largest number of messages queue can afford
  int maxMsgQueue;
  // the largest size number a message queue can afford 
  int maxSizeMsgQueue;
  // quantity of processes using the queue
  int nMsgQueueProcesses;

} MsgQueue;

typedef ListHead ResourceList;

// Resource section
void Resource_init();
Resource* Resource_alloc(int id, int type);
int Resource_free(Resource* resource);

// MsgQueue main function for SLAB
void MsgQueue_init();
MsgQueue* MsqQueue_alloc();
int MsqQueue_add_pid(MsgQueue* q, int pid, int mode, ListItem** ds);
int MsqQueue_free(MsgQueue* queue);

// MsgQueueForProcess main function for SLAB
void MsgQueueForProcess_init();
MsgQueueForProcess* MsgQueueForProcess_alloc(int pid);
int MsgQueueForProcess_free(MsgQueueForProcess* msgQuFProcess);

// Message main function for SLAB
void Message_init();
Message* Message_alloc(char* message, int message_len);
void MsgString_init();
MsgString MsgString_alloc();
int Message_free(Message* msg);

// Debug
Resource* ResourceList_byId(ResourceList* l, int id);
void ResourceList_print(ListHead* l);
void Queue_print();
