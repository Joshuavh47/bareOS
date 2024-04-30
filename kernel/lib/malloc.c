#include <barelib.h>
#include <malloc.h>
#include <thread.h>

extern uint32* mem_start;
extern uint32* mem_end;
static alloc_t* freelist;

/*  Sets the 'freelist' to 'mem_start' and creates  *
 *  a free allocation at that location for the      *
 *  entire heap.                                    */
//--------- This function is complete --------------//
void heap_init(void) {
  freelist = (alloc_t*)mem_start;
  freelist->size = get_stack(NTHREADS) - mem_start - sizeof(alloc_t);
  freelist->state = M_FREE;
  freelist->next = NULL;
}

/*  Locates a free block large enough to contain a new    *
 *  allocation of size 'size'.  Once located, remove the  *
 *  block from the freelist and ensure the freelist       *
 *  contains the remaining free space on the heap.        *
 *  Returns a pointer to the newly created allocation     */
void* malloc(uint64 size) {
  int found = 0;
  alloc_t* last = freelist;
  while(last->next != NULL){
    if(last->state == M_FREE && last->size >= size){
      found = 1;
      break;
    }
    last = last->next;
  }
  if(found!=1){
    if(last->state != M_FREE || last->size < size){
      return 0;
    }
  }
  
  
  if(last->size-size <= sizeof(alloc_t)){
    if(last!=freelist){
      alloc_t* before = freelist;
      while(before->next!=last){
        before = before->next;
      }
      before->next = last;
    }
    freelist = last->next;
    last->state = M_USED;
    last->next = NULL;
    
    
    return (void*)last + sizeof(alloc_t);
  }
  
  alloc_t* new_free = (void*)last + sizeof(alloc_t) + size;
  new_free->size = last->size - size - sizeof(alloc_t);
  new_free->state = M_FREE;
  new_free->next = last->next;


  if(last!=freelist){

    alloc_t* before = freelist;
    while(before->next!=last){
      before = before->next;
    }
    before->next = new_free;
    
  }

  last->size = size;
  last->state = M_USED;
  last->next = NULL;
  
  if(freelist == last){
    freelist = new_free;
  }
  

  return (void*)last+sizeof(alloc_t);

  
}

/*  Free the allocation at location 'addr'.  If the newly *
 *  freed allocation is adjacent to another free          *
 *  allocation, coallesce the adjacent free blocks into   *
 *  one larger free block.                                */
void free(void* addr) {
  if(((alloc_t*)(addr-sizeof(alloc_t)))->state == M_FREE){
    return;
  }
  alloc_t* free_block = addr - sizeof(alloc_t);
  
  free_block->state = M_FREE;
  alloc_t* ptr = freelist;
  if(free_block>freelist){
    while((alloc_t*)((void*)ptr + sizeof(alloc_t) + ptr->size) != addr - sizeof(alloc_t)){
      ptr = (void*)ptr + sizeof(alloc_t) + ptr->size;
    }
    if(ptr == freelist){
      free_block->next = NULL;
      freelist->size = freelist->size + sizeof(alloc_t) + free_block->size;
      if(freelist->next == (void*)free_block + sizeof(alloc_t) + free_block->size){
        freelist->size = freelist->size + sizeof(alloc_t) +freelist->next->size;
        freelist->next = freelist->next->next;
      }
      return;
    }
    int last = 0;
    if(ptr->next == NULL){
      last = 1;
    }
    if(ptr->state != M_FREE){
      free_block->next = ptr->next;
      ptr->next = addr - sizeof(alloc_t);
    }
    
    if(ptr->state == M_FREE && free_block->state == M_FREE){
      ptr->size = ptr->size + sizeof(alloc_t) + free_block->size;
      ptr->next = free_block->next;
      free_block->next = NULL;
    }
    if(last == 0){
      if(ptr->state == M_FREE && ptr->next->state == M_FREE){
        ptr->size = ptr->size + sizeof(alloc_t) + ptr->next->size;
        alloc_t* temp = ptr->next;
        ptr->next = temp->next;
        temp->next = NULL;
      }
    }
    alloc_t* next_ptr = freelist;
    while(next_ptr->next != NULL && next_ptr->next<ptr){
      next_ptr = next_ptr->next;
    }
    if(last != 0){
      ptr = (void*)ptr + sizeof(alloc_t) + ptr->size;
    }
    ptr->next = next_ptr->next;
    next_ptr->next = ptr;
  }
  else{
    if(free_block->state == M_FREE && ((alloc_t*)(addr + free_block->size))->state == M_FREE){
      free_block->next = ((alloc_t*)(addr + free_block->size));
      free_block->size = free_block->size + sizeof(alloc_t) + ((alloc_t*)(addr + free_block->size))->size;
      ((alloc_t*)(addr + free_block->size))->next = NULL;
      freelist = addr - sizeof(alloc_t);
    }
    
  }
  
  

  
}
