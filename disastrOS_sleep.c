#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"

void internal_sleep(){
  //! determines how many cicles a process has to sleep, by reading the argument from
  //! the syscall
  if (pcb_running -> timer) {
    printf("process has already a timer!!!\n");
    pcb_running -> syscall_retvalue = DSOS_ESLEEP;
    return;
  }
  int cycles_to_sleep = pcb_running -> syscall_args[0];
  int wake_time = disastrOS_time + cycles_to_sleep;
  
  TimerItem* new_timer = TimerList_add(&timer_list, wake_time, pcb_running);
  if (! new_timer) {
    printf("no new timer!!!\n");
    pcb_running -> syscall_retvalue = DSOS_ESLEEP;
    return;
  } 
  pcb_running -> status = Waiting;
  List_insert(&waiting_list, waiting_list.last, (ListItem*) pcb_running);
  if (ready_list.first)
    pcb_running = (PCB*) List_detach(&ready_list, ready_list.first);
  else {
    pcb_running = 0;
    printf ("they are all sleeping\n");
    disastrOS_printStatus();
  }
}
