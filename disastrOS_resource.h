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

typedef ListHead ResourceList;

// Resource section
void Resource_init();
Resource* Resource_alloc(int id, int type);
int Resource_free(Resource* resource);

// Debug
Resource* ResourceList_byId(ResourceList* l, int id);
void ResourceList_print(ListHead* l);
void Queue_print();
