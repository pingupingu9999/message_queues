#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"


void internal_openResource(){
  //1 get from the PCB the resource id of the resource to open
  int id = pcb -> syscall_args[0];
  int type = pcb -> syscall_args[1];
  int open_mode = pcb -> syscall_args[2];

  Resource* res = ResourceList_byId(&resources_list, id);
  // if type = CREATE, check if the resource is already existing (throwing an error if true), and then create it 
  // else, let's see if we have it in our list (throwing an error if false)
  // throw an error
  // printf ("CREATING id %d, type: %d, open mode %d\n", id, type, open_mode);
  if (open_mode&DSOS_CREATE){
    // this shouldn't happen, as DSOS_CREATE should create the new resource if it doesn't exist
    // and open it if it does

    // if (res) {
    //   pcb->syscall_retvalue=DSOS_ERESOURCECREATE;
    //   return;
    // }

    // if resource is opened with DSOS_CREATE and DSOS_EXCL, and it already exists, error is thrown
    // maybe create another error for this specific case to differentiate it from the case without DSOS_CREATE
    if (open_mode&DSOS_EXCL && res){
      pcb -> syscall_retvalue = DSOS_ERESOURCENOEXCL;
      return;
    }

    if (res == 0){
    res = Resource_alloc(id, type);
    if (!res) {
      pcb -> syscall_retvalue = DSOS_ERESOURCECREATE;
      return;
    }
    List_insert(&resources_list, resources_list.last, (ListItem*) res);
    }
  }

  // at this point we should have the resource, if not something was wrong
  if (! res || res -> type != type) {
     pcb->syscall_retvalue=DSOS_ERESOURCEOPEN;
     return;
  }

  if (open_mode&DSOS_EXCL && res -> descriptors_ptrs.size != 0){
    pcb -> syscall_retvalue = DSOS_ERESOURCENOEXCL;
    return;
  }
  
  

  
  //5 create the descriptor for the resource in this process, and add it to
  //  the process descriptor list. Assign to the resource a new fd
  Descriptor* des = Descriptor_alloc(pcb->last_fd, res, pcb);
  if (! des){
     pcb -> syscall_retvalue = DSOS_ERESOURCENOFD;
     return;
  }
  pcb -> last_fd++; // we increment the fd value for the next call
  DescriptorPtr* desptr = DescriptorPtr_alloc(des);
  List_insert(&pcb -> descriptors, pcb -> descriptors.last, (ListItem*) des);
  
  //6 add to the resource, in the descriptor ptr list, a pointer to the newly
  //  created descriptor
  des -> ptr = desptr;
  List_insert(&res -> descriptors_ptrs, res -> descriptors_ptrs.last, (ListItem*) desptr);

  // return the FD of the new descriptor to the process
  pcb -> syscall_retvalue = des -> fd;
}