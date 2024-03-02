/*
 *  This file contains functions for initializing and handling interrupts
 *  from the hardware timer.
 */

#include <barelib.h>
#include <interrupts.h>
#include <queue.h>
#include <sleep.h>

#define TRAP_TIMER_ENABLE 0x80

volatile uint32* clint_timer_addr  = (uint32*)0x2004000;
const uint32 timer_interval = 100000;
int64 resched(void);
int dec = 0;
/*
 * This function is called as part of the bootstrapping sequence
 * to enable the timer. (see bootstrap.s)
 */
void clk_init(void) {
  *clint_timer_addr = timer_interval;
  set_interrupt(TRAP_TIMER_ENABLE);
}

/* 
 * This function is triggered every 'timer_interval' microseconds 
 * automatically.  (see '__traps' in bootstrap.s)
 */
interrupt handle_clk(void) {
  *clint_timer_addr += timer_interval;

  char mask = disable_interrupts();
  ++dec;
  
  dec=0;
  uint32 threadid;
  
  
  if(thread_queue[sleep_list].qnext!=sleep_list&&thread_queue[thread_queue[sleep_list].qnext].key > 0){
    thread_queue[thread_queue[sleep_list].qnext].key -= 1;
  }
  for(int i=0;i<NTHREADS;i++){
    if(thread_queue[sleep_list].qnext != sleep_list&&thread_queue[thread_queue[sleep_list].qnext].key == 0){
      threadid = sleep_dequeue(sleep_list);
      thread_table[threadid].state = TH_READY;
      thread_enqueue(NTHREADS, threadid);
    }

  }

  
  restore_interrupts(mask);

  if (boot_complete && is_interrupting()) {
    char mask = disable_interrupts();
    
    
    
    
    resched();
    restore_interrupts(mask);
  }
}
