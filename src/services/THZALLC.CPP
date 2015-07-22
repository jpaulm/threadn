

#include <malloc.h>
#include "setjmp.h"
#include "threads.h"


IPh HUGE * thzallc(long lth, process * p_ptr) {

  p_ptr->appl_ptr->alloc_lth = lth;

  if (0 == setjmp(p_ptr->alloc_state)) 
     longjmp(p_ptr->appl_ptr->alloc_state, 0);

  return(p_ptr->appl_ptr->alloc_ptr);
}



void thzfree(IPh HUGE * p, process * p_ptr) {

     p_ptr->appl_ptr->alloc_ptr = p;

  if (0 == setjmp(p_ptr->free_state))
     longjmp(p_ptr->appl_ptr->free_state, 0);

   return;
}


