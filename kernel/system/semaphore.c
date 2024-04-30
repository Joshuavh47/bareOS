#include <interrupts.h>
#include <queue.h>
#include <semaphore.h>

#include <syscall.h>

semaphore_t sem_table[NSEM];

/*  Finds an unusued semaphore in the sem_table and returns it after  *
 *  resetting its values and marking it as used.                      */
int32 sem_create(int32 count) {
  char mask = disable_interrupts();
  if(sem_table[current_thread].state==S_FREE){
    sem_table[current_thread].state = S_USED;
    sem_table[current_thread].count = count;
    sem_table[current_thread].qnext = sem_list;
    sem_table[current_thread].qprev = sem_list;
    return current_thread;
  }
  for(int i=NTHREADS;i<NSEM;i++){
    if(sem_table[i].state == S_FREE){
      sem_table[i].state = S_USED;
      sem_table[i].count = count;
      sem_table[i].qnext = sem_list;
      sem_table[i].qprev = sem_list;
      return i;
    }
  }


  restore_interrupts(mask);
  return -1;
}

/*  Marks a semaphore as free  */
int32 sem_free(uint32 sid) {
  if(sem_table[sid].state == S_FREE){
    return -1;
  }
  else if(sem_table[sid].state == S_USED){
    sem_table[sid].state = S_FREE;
  }
  return 0;
}

/*  Decrements the given semaphore if it is in use.  If the semaphore  *
 *  is less than 0, marks the thread as waiting and switches to        *
 *  another thread.                                                    */
int32 sem_wait(uint32 sid) {
  char mask;
  mask = disable_interrupts();
  if(sid>=NSEM || sem_table[sid].state == S_FREE){
    return -1;
  }
  sem_table[sid].count-=1;
  if(sem_table[sid].count<0){
    if(isInQueue(thread_queue, ready_list, current_thread)){
      specific_dequeue(ready_list, current_thread);
    }
    thread_table[current_thread].state = TH_WAITING;
    if(!sem_is_in_queue(sid, current_thread)){
      sem_enqueue(sid, current_thread);
    }
    raise_syscall(RESCHED);
  }
  

  restore_interrupts(mask);
  return 0;
}

/*  Increments the given semaphore if it is in use.  Resume the next  *
 *  waiting thread (if any).                                          */
int32 sem_post(uint32 sid) {
  char mask;
  mask = disable_interrupts();
  if(sid>=NSEM || sem_table[sid].state == S_FREE){
    return -1;
  }
  sem_table[sid].count++;
  if(sem_table[sid].count<=0){
    uint32 ret_thread = sem_dequeue(sid);
    if(ret_thread != sem_list){
      thread_table[ret_thread].state = TH_READY;
      thread_enqueue(ready_list, ret_thread);
    }
    raise_syscall(RESCHED);
  }

  restore_interrupts(mask);
  return 0;
}
