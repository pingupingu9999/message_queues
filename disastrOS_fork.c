#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"

void internal_fork() {
  static PCB* new_pcb;
  new_pcb = PCB_alloc();
  if (!new_pcb) {
    pcb_running -> syscall_retvalue = DSOS_ESPAWN;
    return;
  } 

  new_pcb -> status = Ready;

  // sets the parent of the newly created process to the pcb_running process
  new_pcb -> parent = pcb_running;
  
  // adds a pointer to the new process to the children list of pcb_running
  PCBPtr* new_pcb_ptr = PCBPtr_alloc(new_pcb);
  assert(new_pcb_ptr);
  List_insert(&pcb_running -> children, pcb_running -> children.last, (ListItem*) new_pcb_ptr);

  //adds the new process to the ready queue
  List_insert(&ready_list, ready_list.last, (ListItem*) new_pcb);

  //sets the retvalue for the caller to the new pid
  pcb_running -> syscall_retvalue = new_pcb -> pid;
}