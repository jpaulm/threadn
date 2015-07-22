#include <setjmp.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "thxiip.h"
#include "threads.h"
typedef enum {FALSE, TRUE} Boolean;

  int thzpop(process *pptr, void **ptr, long *size, char **typep)
  {
   process *sptr;
   IPh HUGE *IPptr;
   IPh HUGE *optr;
   IP HUGE *tptr;
   int value;


   if (pptr -> trace) MSG1("%s Pop start\n", pptr -> procname);

   if ((IPptr = pptr -> stack) == 0)
      return(2);
   pptr -> stack = IPptr -> next_IP;
   tptr = (IP *) IPptr;
   if (tptr -> datapart[IPptr -> IP_size] != guard_value)
      MSG1("Guard byte corrupted\n", pptr -> procname);
   if (IPptr -> owner != pptr)
     MSG1("IP header corrupted\n", pptr->procname);
   if (! IPptr -> on_stack)
     MSG1("IP not on stack\n", pptr->procname);

   *ptr = tptr -> datapart;
   *size = IPptr -> IP_size;
   *typep = IPptr -> type;
   if ((optr = pptr -> first_owned_IP) != 0)
       optr -> prev_IP = IPptr;
   IPptr -> next_IP = optr;
   IPptr -> prev_IP = 0;
   pptr -> first_owned_IP = IPptr;
   IPptr -> owner = pptr;
   IPptr -> on_stack = 0;
   if (pptr -> trace) MSG1("%s Pop end\n", pptr -> procname);
   return(0);
}