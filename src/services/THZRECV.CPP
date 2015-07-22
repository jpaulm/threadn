#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include "thxiip.h"
#include "threads.h"
typedef enum {FALSE, TRUE} Boolean;
#define INPUT 0
#define OUTPUT 1

int thzcrep(process *pptr, void **ptr, long IPsize, char *type);


  int thzrecv(process *pptr, void **ptr, port_ent *peptr, int elem_no,
     long *size, char **typep)
{
   process *sptr;
   IPh HUGE *IPptr;
   IPh HUGE *optr;
   IP HUGE *tptr;
   int value;
   cp *cpp;
   cnxt *cnp;
   IIP *IIPptr;
   IPh HUGE *created_IIP_ptr;
   void *created_ptr;

   
   if (peptr -> ret_code == 2) {
      if (pptr -> trace) MSG1("%s Recv start\n", pptr -> procname);
      return(2);
      }
   cpp = (cp *)peptr -> cpptr;

   if (pptr -> trace) MSG3("%s Recv start %s[%d]\n", pptr -> procname,
       cpp -> port_name, elem_no);

   if (elem_no < 0 || elem_no >= cpp ->elem_count) {
      MSG2("%s %s RECV Element Number negative or too high\n",
         pptr -> procname, peptr -> port_name);
      return(3);
      }
   if (cpp ->direction != INPUT) {
      MSG2("%s %s RECV Wrong direction\n",
         pptr -> procname, peptr -> port_name);
      return(3);
      }
   cnp = cpp -> elem_list[elem_no].gen.connxn;
   if (cnp == 0) return(2);

   if (cpp -> elem_list[elem_no].closed) {
	  if (pptr -> trace) MSG1("%s Recv end no data\n", pptr -> procname);
	  return(1); }

   if (cpp -> elem_list[elem_no].is_IIP) {
       IIPptr = cpp -> elem_list[elem_no].gen.IIPptr;
       value = thzcrep(pptr, &created_ptr, IIPptr -> IIP_len, "OPTIONS");

       created_IIP_ptr = (IPh HUGE *) created_ptr - 1;
       memcpy (created_ptr, IIPptr -> datapart, IIPptr -> IIP_len);
       cpp -> elem_list[elem_no].closed = TRUE;
       *ptr = created_ptr;
       *size = created_IIP_ptr -> IP_size;
       *typep = created_IIP_ptr -> type;

       if (pptr -> trace) MSG1("%s Recv end\n", pptr -> procname);
       return(0);
   }

   pptr -> value = setjmp(pptr -> state);
   cnp -> fedproc_wtg_to_recv = FALSE;

   if (cnp -> first_IPptr != 0) goto X;

   if (cnp -> nonterm_upstream_cpelem_count == 0 ||
       cpp -> elem_list[elem_no].closed)
     {
       if (pptr -> trace) MSG1("%s Recv end no data\n", pptr -> procname);
       return(1);
     }

   cnp -> fedproc_wtg_to_recv = TRUE;
   pptr -> status = SUSPENDED_ON_RECV;
   if (pptr -> trace) MSG1("%s Recv susp\n", pptr -> procname);
   longjmp (pptr -> appl_ptr -> state, 2);

X: IPptr = cnp -> first_IPptr;
   if ((cnp -> first_IPptr = IPptr -> next_IP) == 0)
      cnp -> last_IPptr = 0;

   sptr = cnp -> procs_wtg_to_send;
   while (sptr != 0)
   {
      cnp -> procs_wtg_to_send = sptr -> next_proc;
      sptr -> next_proc = pptr -> appl_ptr -> first_ready_proc;
      pptr -> appl_ptr -> first_ready_proc = sptr;
      sptr -> status = READY_TO_RESUME;
      sptr = cnp -> procs_wtg_to_send;
   }
   tptr = (IP *) IPptr;
   if (tptr -> datapart[IPptr -> IP_size] != guard_value)
      MSG1("Guard byte corrupted\n", pptr->procname);
   if (IPptr -> owner != cnp)
     MSG1("IP header corrupted\n", pptr->procname);
   *ptr = tptr -> datapart;
   *size = IPptr -> IP_size;
   *typep = IPptr -> type;
   if ((optr = pptr -> first_owned_IP) != 0)
       optr -> prev_IP = IPptr;
   IPptr -> next_IP = optr;
   IPptr -> prev_IP = 0;
   pptr -> first_owned_IP = IPptr;
   IPptr -> owner = pptr;
   cnp -> IPcount--;
   if (pptr -> trace) MSG1("%s Recv end\n", pptr -> procname);
   return(0);
}