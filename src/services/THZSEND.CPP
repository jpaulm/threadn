
#include <setjmp.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "thxiip.h"
#include "threads.h"
typedef enum {FALSE, TRUE} Boolean;
#define INPUT 0
#define OUTPUT 1

int thzrecv(process *pptr, void **ptr, port_ent *peptr, int elem_no,
       long *size, char **typep);
int thzdrop(process *pptr, void **ptr);

   int thzsend(process *pptr, void **ptr, port_ent *peptr, int elem_no)
{
   process *sptr;
   IPh HUGE *IPptr;
   IPh HUGE *prevIP;
   IPh HUGE *optr;
   IPh HUGE *qptr;
   IP HUGE *tptr;
   long size;
   char *type;
   cp *cpp;
   cnxt *cnp;
   int value;


   if (peptr -> ret_code == 2) {
      if (pptr -> trace) MSG1("%s Send start\n", pptr -> procname);
      value = 2;
      goto retn;
      }

   cpp = (cp *)peptr -> cpptr;

   if (pptr -> trace) MSG3("%s Send start %s[%d]\n", pptr -> procname,
       cpp -> port_name, elem_no);

   IPptr = (IPh HUGE *) *ptr - 1;       /* back up size of header */
   tptr = (IP *)IPptr;
   if (tptr -> datapart[IPptr -> IP_size] != guard_value)
      MSG1("Guard byte corrupted\n", pptr->procname);
   if (IPptr -> on_stack)
     MSG1("IP on stack\n", pptr->procname);

   if (cpp -> direction != OUTPUT) {
      MSG2("%s %s SEND Wrong direction\n",
           pptr -> procname, peptr -> port_name);
      value = 3;
      goto retn;
      }
   if (elem_no < 0 || elem_no >= cpp ->elem_count) {
      MSG2("%s %s SEND Element Number negative or too high\n",
	 pptr -> procname, peptr -> port_name);
      value = 3;
      goto retn;
      }
   if ((cnp = cpp -> elem_list[elem_no].gen.connxn) == 0) return(2);
   if (cpp -> elem_list[elem_no].closed) return(1);
   if (cnp -> nonterm_upstream_cpelem_count == 0) {
       value = 1;
       goto retn;
       }
   pptr -> value = setjmp(pptr -> state);
   if (cnp -> IPcount < cnp -> max_IPcount) goto X;
   pptr -> next_proc = cnp -> procs_wtg_to_send;
   cnp -> procs_wtg_to_send = pptr;
   pptr -> status = SUSPENDED_ON_SEND;
   if (pptr -> trace) MSG1 ("%s Send susp\n", pptr -> procname);
   longjmp (pptr -> appl_ptr -> state, 2);

X: if (cpp -> elem_list[elem_no].closed) {
       value = 1;
       goto retn;
       }
   if (cnp -> nonterm_upstream_cpelem_count == 0) {
       value = 1;
       goto retn;
       }

   if (IPptr -> owner != pptr)
     MSG1("IP header corrupted\n", pptr->procname);
   IPptr -> owner = cnp;
   optr = IPptr -> prev_IP;
   qptr = IPptr -> next_IP;
   if (optr != 0)
       optr -> next_IP = qptr;
     else
       pptr -> first_owned_IP = qptr;
   if (qptr != 0)
     qptr -> prev_IP = optr;
   prevIP = cnp -> last_IPptr;
   if (prevIP != 0)
       prevIP -> next_IP = IPptr;
     else
       cnp -> first_IPptr = IPptr;

   cnp -> last_IPptr = IPptr;
   IPptr -> next_IP = 0;

   if ((++cnp -> IPcount) == 1)   /* if it was empty, enable fedproc */
   {
      sptr = cnp -> fed_proc;

      switch (sptr -> status) {
	 case NOT_INITIATED:
	   if (sptr -> begin_cp == 0) {
	     sptr -> status = READY_TO_BE_INITIATED;
             sptr -> next_proc = pptr -> appl_ptr -> first_ready_proc;
             pptr -> appl_ptr -> first_ready_proc = sptr;
	     }
	   else
             if (sptr -> begin_cp -> elem_list[0].gen.connxn == cnp) {
	       sptr -> status = READY_TO_BE_INITIATED;
               sptr -> next_proc = pptr -> appl_ptr -> first_ready_proc;
               pptr -> appl_ptr -> first_ready_proc = sptr;
	       pptr -> int_pe.cpptr = pptr -> begin_cp;
	       pptr -> value = thzrecv(pptr, &pptr -> int_ptr,
		 &pptr -> int_pe, 0, &size, &type);
	       pptr -> value = thzdrop(pptr, &pptr -> int_ptr);
	      }
	   break;

	 case SUSPENDED_ON_RECV:
	   if (cnp -> fedproc_wtg_to_recv) {
               sptr -> status = READY_TO_RESUME;
               sptr -> next_proc = pptr -> appl_ptr -> first_ready_proc;
               pptr -> appl_ptr -> first_ready_proc = sptr;
           }
           break;

	 case DORMANT:
           if (sptr -> begin_cp == 0) {
             sptr -> status = READY_TO_RESUME;
             sptr -> next_proc = pptr -> appl_ptr -> first_ready_proc;
             pptr -> appl_ptr -> first_ready_proc = sptr;
	     }
	   else
             if (sptr -> begin_cp -> elem_list[0].gen.connxn == cnp) {
	     sptr -> status = READY_TO_RESUME;
             sptr -> next_proc = pptr -> appl_ptr -> first_ready_proc;
             pptr -> appl_ptr -> first_ready_proc = sptr;
	     pptr -> int_pe.cpptr = pptr -> begin_cp;
	     pptr -> value = thzrecv(pptr, &pptr -> int_ptr,
		 &pptr -> int_pe, 0, &size, &type);
	     pptr -> value = thzdrop(pptr, &pptr -> int_ptr);
	     }
	   break;

         }

   }

   if (pptr -> trace) MSG1("%s Send end\n", pptr -> procname);
   *ptr = 0;
   value = 0;
retn:
   return(value);
}
