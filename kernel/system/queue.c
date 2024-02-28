#include <queue.h>
#include <thread.h>

/*  Queues in bareOS are all contained in the 'thread_queue' array.  Each queue has a "root"
 *  that contains the index of the first and last elements in that respective queue.  These
 *  roots are  found at the end  of the 'thread_queue'  array.  Following the 'qnext' index 
 *  of each element, starting at the "root" should always eventually lead back to the "root".
 *  The same should be true in reverse using 'qprev'.                                          */

queue_t thread_queue[NTHREADS+1];   /*  Array of queue elements, one per thread plus one for the read_queue root  */
uint32 ready_list = NTHREADS + 0;   /*  Index of the read_list root  */


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
  uint32 temp_queue = thread_queue[queue];
  int passed_root = 0;
  while(temp_queue.qnext!=threadid){
    if(thread_queue[temp_queue.qprev].qnext == queue){
      passed_root++;
    }
    
    temp_queue = thread_queue[temp_queue.qnext];
    if(thread_queue[temp_queue.qprev].qnext == queue && passed_root > 0){
      return -1;
    }
  }
  uint32 ret_queue = temp_queue.qnext;
  temp_queue = thread_queue[temp_queue.qnext];
  thread_queue[temp_queue.qprev].qnext = temp_queue.qnext;
  thread_queue[temp_queue.qnext].qprev = temp_queue.qprev;
  return ret_queue;
}

int isInQueue(queue_t[] arr, uint32 queue, uint32 threadid) {
  uint32 temp_queue = arr[queue];
  int passed_root = 0;
  while(temp_queue.qnext!=threadid){
    if(arr[temp_queue.qprev].qnext == queue){
      passed_root++;
    }
    
    temp_queue = arr[temp_queue.qnext];
    if(arr[temp_queue.qprev].qnext == queue && passed_root > 0){
      return 0;
    }
  }
  return 1;
}

void sleep_enqueue(uint32 queue, uint32 threadid, uint32 delay) {
  int passed_root = 0;
  queue_t temp_queue = sleep_queue[queue];
  
  uint32 index=temp_queue.qprev;
  int total = 0;
  while(sleep_queue[temp_queue.qprev].qnext!=queue&&passed_root>0){
    total+=sleep_queue[temp_queue.qnext].key;
    if(temp_queue.qnext!=queue&&delay<total){
      index=sleep_queue[temp_queue.qprev].qnext;
    }
    if(sleep_queue[temp_queue.qprev].qnext==queue){
      passed_root++;
    }
    if(temp_queue.qnext==threadid){
      return;
    }
    
    temp_queue=sleep_queue[temp_queue.qnext];
  }

  sleep_queue[threadid].qnext=sleep_queue[index].qnext;
  sleep_queue[threadid].qprev=index;
  sleep_queue[thread_queue[index].qnext].qprev=threadid;
  sleep_queue[index].qnext=threadid;
  temp_queue.key = 

}

uint32 sleep_dequeue(uint32 queue) {
  if(sleep_queue[queue].qnext!=queue){
    uint32 ret_thread = sleep_queue[queue].qnext;
    //queue_t temp_queue = thread_queue[thread_queue[queue].qnext];
    sleep_queue[queue].qnext = sleep_queue[sleep_queue[queue].qnext].qnext;
    sleep_queue[sleep_queue[queue].qnext].qprev = queue;
    sleep_queue[ret_thread].qnext=0;
    sleep_queue[ret_thread].qprev=0;
    return ret_thread;
  }
  return queue;
}