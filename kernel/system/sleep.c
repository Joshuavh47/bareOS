#include <barelib.h>
#include <syscall.h>
#include <interrupts.h>
#include <queue.h>
#include <thread.h>

/*  Places the thread into a sleep state and inserts it into the  *
 *  sleep delta list.                                             */
int32 sleep(uint32 threadid, uint32 delay) {
  char mask;
  mask = disable_interrupts();
  if(delay <= 0){
    raise_syscall(RESCHED);
    return -1;
  }
  specific_dequeue(ready_list, threadid);
  thread_table[threadid].state = TH_SLEEP;
  sleep_enqueue(sleep_list, threadid, delay);
  
  raise_syscall(RESCHED);


  restore_interrupts(mask);
  return 0;
}

/*  If the thread is in the sleep state, remove the thread from the  *
 *  sleep queue and resumes it.                                      */
int32 unsleep(uint32 threadid) {
  char mask;
  mask = disable_interrupts();
  if(isInQueue(thread_queue, sleep_list, threadid) == 0){
    return -1;
  }
  sleep_specific_dequeue(sleep_list, threadid);
  thread_table[threadid].state = TH_READY;
  thread_enqueue(NTHREADS, threadid);
  raise_syscall(RESCHED);


  restore_interrupts(mask);
  return 0;
}
