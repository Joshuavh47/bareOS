#include <barelib.h>
#include <syscall.h>
#include <thread.h>
#include <queue.h>

/*  'resched' places the current running thread into the ready state  *
 *  and  places it onto  the tail of the  ready queue.  Then it gets  *
 *  the head  of the ready  queue  and sets this  new thread  as the  *
 *  'current_thread'.  Finally,  'resched' uses 'ctxsw' to swap from  *
 *  the old thread to the new thread.                                 */
int32 resched(void) {
  //int found=0;
  //int old_thread=-1;
  
  uint32 old_thread=current_thread;
  current_thread=thread_dequeue(NTHREADS);
  if(current_thread==NTHREADS){
    current_thread=old_thread;
    return 0;
  }
  
  if(thread_table[old_thread].state==TH_RUNNING || thread_table[old_thread].state==TH_READY){
    thread_table[old_thread].state=TH_READY;
    thread_enqueue(NTHREADS,old_thread);
  }
  thread_table[current_thread].state=TH_RUNNING;
  ctxsw(&thread_table[current_thread].stackptr,&thread_table[old_thread].stackptr);
  return current_thread;
  
}
