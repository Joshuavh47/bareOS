#include <queue.h>
#include <thread.h>
#include <semaphore.h>

/*  Queues in bareOS are all contained in the 'thread_queue' array.  Each queue has a "root"
 *  that contains the index of the first and last elements in that respective queue.  These
 *  roots are  found at the end  of the 'thread_queue'  array.  Following the 'qnext' index 
 *  of each element, starting at the "root" should always eventually lead back to the "root".
 *  The same should be true in reverse using 'qprev'.                                          */

queue_t thread_queue[NTHREADS+3];   /*  Array of queue elements, one per thread plus one for the read_queue root  */
uint32 ready_list = NTHREADS + 0;   /*  Index of the read_list root  */

queue_t sleep_queue[NTHREADS+1];
uint32 sleep_list = NTHREADS + 1;

uint32 sem_list = NTHREADS + 2;

/*  'thread_enqueue' takes an index into the thread_queue  associated with a queue "root"  *
 *  and a threadid of a thread to add to the queue.  The thread will be added to the tail  *
 *  of the queue,  ensuring that the  previous tail of the queue is correctly threaded to  *
 *  maintain the queue.                                                                    */
void thread_enqueue(uint32 queue, uint32 threadid) {
  int passed_root = 0;
  queue_t temp_queue = thread_queue[queue];
  
  int index=temp_queue.qprev;
  
  while(thread_queue[temp_queue.qprev].qnext!=queue&&passed_root>0){
    if(temp_queue.qnext!=queue&&thread_table[threadid].priority<thread_queue[temp_queue.qnext].key){
      index=thread_queue[temp_queue.qprev].qnext;
    }
    if(thread_queue[temp_queue.qprev].qnext==queue){
      passed_root++;
    }
    if(temp_queue.qnext==threadid){
      return;
    }
    temp_queue=thread_queue[temp_queue.qnext];
  }
  thread_queue[threadid].qnext=thread_queue[index].qnext;
  thread_queue[threadid].qprev=index;
  thread_queue[thread_queue[index].qnext].qprev=threadid;
  thread_queue[index].qnext=threadid;
  
  
  
  
  
  
  
  


}


/*  'thread_dequeue' takes a queue index associated with a queue "root" and removes the  *
 *  thread at the head of the queue and returns the index of that thread, ensuring that  *
 *  the queue  maintains its structure and the head correctly points to the next thread  *
 *  (if any).                                                                            */
uint32 thread_dequeue(uint32 queue) {
  if(thread_queue[queue].qnext!=queue){
    uint32 ret_thread = thread_queue[queue].qnext;
    //queue_t temp_queue = thread_queue[thread_queue[queue].qnext];
    thread_queue[queue].qnext = thread_queue[thread_queue[queue].qnext].qnext;
    thread_queue[thread_queue[queue].qnext].qprev = queue;
    thread_queue[ret_thread].qnext=0;
    thread_queue[ret_thread].qprev=0;
    return ret_thread;
  }
  return queue;
}

uint32 specific_dequeue(uint32 queue, uint32 threadid){
  queue_t temp_queue = thread_queue[queue];
  int passed_root = 0;
  while(thread_queue[temp_queue.qprev].qnext!=threadid){
    if(thread_queue[temp_queue.qprev].qnext == queue){
      passed_root++;
    }
    if(thread_queue[temp_queue.qprev].qnext == threadid){
      break;
    }
    temp_queue = thread_queue[temp_queue.qnext];
    if(thread_queue[temp_queue.qprev].qnext == queue && passed_root > 0){
      return NTHREADS;
    }
    
  }
  uint32 ret_queue = thread_queue[temp_queue.qprev].qnext;
  thread_queue[temp_queue.qprev].qnext = temp_queue.qnext;
  thread_queue[temp_queue.qnext].qprev = temp_queue.qprev;
  return ret_queue;
}

int isInQueue(queue_t arr[], uint32 queue, uint32 threadid) {
  queue_t temp_queue = arr[queue];
  int passed_root = 0;
  while(arr[temp_queue.qprev].qnext!=threadid){
    if(arr[temp_queue.qprev].qnext == queue && passed_root > 0){
      return 0;
    }
    if(arr[temp_queue.qprev].qnext == queue){
      passed_root++;
    }
    if(arr[temp_queue.qprev].qnext == threadid){
      break;
    }
    temp_queue = arr[temp_queue.qnext];
    
  }
  return 1;
}

void sleep_enqueue(uint32 queue, uint32 threadid, uint32 delay) {
  int passed_root = 0;
  queue_t temp_queue = thread_queue[queue];
  
  uint32 index=queue;
  int total = 0;
  int prev = 0;
  for(int i=0;i<NTHREADS;i++){
    total+=temp_queue.key;
    if(thread_queue[temp_queue.qprev].qnext == queue && passed_root > 0){
      return;
    }
    if(delay>total&&(delay<total+thread_queue[temp_queue.qnext].key||temp_queue.qnext == queue)){
      index=thread_queue[temp_queue.qprev].qnext;
      break;
    }
    
    if(thread_queue[temp_queue.qprev].qnext==queue){
      passed_root++;
    }
    if(temp_queue.qnext==threadid){
      return;
    }
    
    temp_queue=thread_queue[temp_queue.qnext];
  }
  

  thread_queue[threadid].qnext=thread_queue[index].qnext;
  thread_queue[threadid].qprev=index;
  thread_queue[thread_queue[threadid].qnext].qprev=threadid;
  thread_queue[index].qnext=threadid;
  temp_queue = thread_queue[queue];
  while(thread_queue[temp_queue.qprev].qnext != threadid){
    prev+=temp_queue.key;
    temp_queue = thread_queue[temp_queue.qnext];
  }

  thread_queue[threadid].key = delay-prev;
  
  if(thread_queue[threadid].qnext != queue){
    thread_queue[thread_queue[threadid].qnext].key = thread_queue[thread_queue[threadid].qnext].key - (delay-prev);
  }
  

}

uint32 sleep_dequeue(uint32 queue) {
  if(thread_queue[queue].qnext!=queue){
    uint32 ret_thread = thread_queue[queue].qnext;
    //queue_t temp_queue = thread_queue[thread_queue[queue].qnext];
    thread_queue[queue].qnext = thread_queue[thread_queue[queue].qnext].qnext;
    thread_queue[thread_queue[queue].qnext].qprev = queue;
    
    return ret_thread;
  }
  return queue;
}

uint32 sleep_specific_dequeue(uint32 queue, uint32 threadid){
  queue_t temp_queue = thread_queue[queue];
  int passed_root = 0;
  while(temp_queue.qnext!=threadid){
    if(thread_queue[temp_queue.qprev].qnext == queue){
      passed_root++;
    }
    if(thread_queue[temp_queue.qprev].qnext == threadid){
      break;
    }
    temp_queue = thread_queue[temp_queue.qnext];
    if(thread_queue[temp_queue.qprev].qnext == queue && passed_root > 0){
      return NTHREADS;
    }
    
  }
  uint32 ret_queue = thread_queue[temp_queue.qprev].qnext;
  uint32 ret = thread_queue[ret_queue].qnext;
  temp_queue = thread_queue[ret_queue];
  thread_queue[ret_queue].qnext = thread_queue[ret].qnext;
  thread_queue[thread_queue[ret].qnext].qprev = ret_queue;
  thread_queue[thread_queue[ret].qnext].key += thread_queue[ret].key;
  return ret;
}

uint32 sem_is_in_queue(uint32 sid, uint32 threadid){
  uint32 temp = sem_table[sid].qnext;
  if(temp == threadid){
    return 1;
  }
  while(thread_queue[temp].qnext != sem_list){
    if(temp == threadid){
      return 1;
    }
    temp = thread_queue[temp].qnext;
  }
  if(temp == threadid){
    return 1;
  }
  else{
    return 0;
  }
}

void sem_enqueue(uint32 sid, uint32 threadid){
  if(sem_table[sid].qnext == sem_list){
    sem_table[sid].qnext = threadid;
    sem_table[sid].qprev = threadid;
    thread_queue[threadid].qnext = sem_list;
    thread_queue[threadid].qprev = sem_list;
    return;
  }
  uint32 temp = sem_table[sid].qnext;
  while(thread_queue[temp].qnext != sem_list){
    temp = thread_queue[temp].qnext;
  }
  thread_queue[temp].qnext = threadid;
  thread_queue[threadid].qprev = temp;
  sem_table[sid].qprev = threadid;
}

uint32 sem_dequeue(uint32 sid){
  if(sem_table[sid].qnext == sem_list){
    return sem_list;
  }
  uint32 temp = sem_table[sid].qnext;
  if(thread_queue[temp].qnext == sem_list){
    uint32 ret_sem = sem_table[sid].qnext;
    sem_table[sid].qnext = sem_list;
    sem_table[sid].qprev = sem_list;
    return ret_sem;
  }
  uint32 ret_sem = sem_table[sid].qnext;
  sem_table[sid].qnext = thread_queue[ret_sem].qnext;
  thread_queue[sem_table[sid].qnext].qprev = sem_list;
  thread_queue[ret_sem].qnext = sem_list;
  thread_queue[ret_sem].qprev = sem_list;
  return ret_sem;
}