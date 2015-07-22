
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "thxiip.h"
#include "threads.h"

int thzcrep(process *pptr, void **ptr, long IPsize, char *type)
{
   long totsize;
   IPh HUGE *IPptr;
   IPh HUGE *optr;
   IP HUGE *tptr;
   IPh HUGE * thzallc(long lth, process * p);
   void thzfree(IPh HUGE *, process * p);


   if (pptr->trace) MSG1("%s Crep start\n", pptr -> procname);
   if (IPsize > 64000 || IPsize < 0) {
      MSG2("Invalid IP size in CREP: %ld - %s\n", IPsize, pptr -> procname);
      return(8);
      }
   totsize = IPsize + sizeof(IPh) + 1;
   IPptr = thzallc(totsize, pptr);
   
   IPptr -> IP_size = IPsize;
   IPptr -> owner = pptr;
   IPptr -> type = type;
   if ((optr = pptr -> first_owned_IP) != 0)
       optr -> prev_IP = IPptr;
   IPptr -> next_IP = optr;
   IPptr -> prev_IP = 0;
   IPptr -> on_stack = 0;
   pptr -> first_owned_IP = IPptr;
   tptr = (IP *) IPptr;
   *ptr = tptr -> datapart;
   tptr -> datapart[IPsize] = guard_value;

   if (pptr->trace) MSG1("%s Crep end\n",pptr -> procname);
   return(0);
}
