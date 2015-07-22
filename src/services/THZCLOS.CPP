#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "thxiip.h"
#include "threads.h"
typedef  enum {FALSE, TRUE} Boolean;
#define INPUT 0
#define OUTPUT 1


   int thzclos(process *pptr, port_ent *peptr, int elem_no)
{
   process *sptr;
   cp *cpp;
   cnxt *cnp;
   IPh HUGE *IPptr;
   IPh HUGE *nextIP;
   int term_now;
   IPh HUGE * thzallc(long lth, process * p);
   void thzfree(IPh HUGE *, process * p);

   cpp = (cp *)peptr -> cpptr;
   if (peptr -> ret_code == 2)
      return(2);
   cnp = cpp -> elem_list[elem_no].gen.connxn;
   if (cnp == 0) return(2);
   if (pptr->trace) MSG1("%s Close start\n",pptr -> procname);

   if (cpp -> elem_list[elem_no].closed) return(1);

   if (cpp -> direction == INPUT &&
         cnp -> nonterm_upstream_cpelem_count == 0)
       return(1);
   cpp -> elem_list[elem_no].closed = TRUE;

   if (cpp -> direction == INPUT) {
       cnp -> nonterm_upstream_cpelem_count = 0;
        sptr = cnp -> procs_wtg_to_send;
        while (sptr != 0)   {
           cnp -> procs_wtg_to_send = sptr -> next_proc;
           sptr -> next_proc = pptr -> appl_ptr -> first_ready_proc;
           pptr -> appl_ptr -> first_ready_proc = sptr;
           sptr -> status = READY_TO_RESUME;
           sptr = cnp -> procs_wtg_to_send;
	   IPptr = cnp -> first_IPptr;
	   while (IPptr != 0) {
	      nextIP = IPptr -> next_IP;
	      thzfree(IPptr, pptr);
	      IPptr = nextIP;
	      if (pptr -> trace)
		  printf("%s IP discarded\n", pptr -> procname);
	      }
	   cnp -> first_IPptr = 0;
	   cnp -> last_IPptr = 0;
	   cnp -> IPcount = 0;
           }
        }
   else {
          cnp -> nonterm_upstream_cpelem_count--;
          sptr = cnp -> fed_proc;
          term_now = TRUE;
          if (sptr -> begin_cp != 0 &&
		sptr -> begin_cp -> elem_list[0].gen.connxn == cnp) {
		cnp -> nonterm_upstream_cpelem_count = 0;
		term_now = FALSE;
		}

	  if (cnp -> nonterm_upstream_cpelem_count == 0) {

	      switch (sptr -> status) {

	      case NOT_INITIATED:
		 sptr -> status = READY_TO_BE_INITIATED;
		 if (term_now) sptr -> terminating = TRUE;
		 sptr -> next_proc = pptr -> appl_ptr -> first_ready_proc;
		 pptr -> appl_ptr -> first_ready_proc = sptr;
		 break;

	      case SUSPENDED_ON_RECV:
                 if (cnp -> fedproc_wtg_to_recv) {
                    sptr -> status = READY_TO_RESUME;
		    sptr -> next_proc = pptr -> appl_ptr -> first_ready_proc;
		    pptr -> appl_ptr -> first_ready_proc = sptr;
		    }
		 break;

	      case DORMANT:
                 sptr -> status = READY_TO_RESUME;
		 sptr -> next_proc = pptr -> appl_ptr -> first_ready_proc;
		 pptr -> appl_ptr -> first_ready_proc = sptr;

	      }
	    }
	  }


   if (pptr->trace) MSG1("%s Close end\n", pptr -> procname);
   return(0);
}

