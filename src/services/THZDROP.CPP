#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "thxiip.h"
#include "threads.h"

int thzdrop(process *pptr, void **ptr)
{
   IPh HUGE *IPptr;
   IPh HUGE *optr;
   IPh HUGE *qptr;
   IP HUGE *tptr;
   IPh HUGE * thzallc(long lth, process * p);
   void thzfree(IPh HUGE *, process * p);

   if (pptr->trace) MSG1("%s Drop start\n", pptr -> procname);
   IPptr = (IPh HUGE *) *ptr - 1;       /* back up size of header */
   tptr = (IP HUGE *) IPptr;
   if (tptr -> datapart[IPptr -> IP_size] != guard_value)
      MSG1("Guard byte corrupted - %s\n", pptr->procname);
   if (IPptr -> owner != pptr)
     MSG1("IP header corrupted - %s\n", pptr->procname);
   if (IPptr -> on_stack)
     MSG1("IP on stack - %s\n", pptr->procname);
   optr = IPptr -> prev_IP;
   qptr = IPptr -> next_IP;
   if (optr != 0)
       optr -> next_IP = qptr;
     else
       pptr -> first_owned_IP = qptr;
   if (qptr != 0)
     qptr -> prev_IP = optr;
   thzfree(IPptr, pptr);

   if (pptr->trace) MSG1("%s Drop end\n",pptr -> procname);
   *ptr = 0;
   return(0);
}
