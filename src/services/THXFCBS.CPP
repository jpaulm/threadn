
   #include <malloc.h>
   #include <stdio.h>
   #include <stdlib.h>
   #include <setjmp.h>
   #include "thxiip.h"
   #include "threads.h"

   void thxfcbs(appl *appl_ptr)
   {
   cp *cpp, *old_cpp;
   process *this_proc, *old_proc;
   int i, deadsw;

   deadsw = 0;

   this_proc = appl_ptr -> first_child_proc;
   while (this_proc != 0) {
   if (this_proc -> status != TERMINATED) {
       printf("Deadlock detected\n");
       deadsw = 1;
       break;
       }
     this_proc = this_proc -> next_sibling_proc;
     }
   this_proc = appl_ptr -> first_child_proc;
   while (this_proc != 0) {
     if (deadsw) {
        switch (this_proc -> status) {
	 case NOT_INITIATED:
           printf (" Process %s Not Initiated\n", this_proc -> procname);
	   break;
         case ACTIVE:
           printf (" Process %s Active\n", this_proc -> procname);
	   break;
         case DORMANT:
           printf (" Process %s Inactive\n", this_proc -> procname);
	   break;
         case SUSPENDED_ON_SEND:
           printf (" Process %s Suspended on Send\n", this_proc -> procname);
	   break;
         case SUSPENDED_ON_RECV:
           printf (" Process %s Suspended on Receive\n", this_proc -> procname);
	   break;
         case READY_TO_BE_INITIATED:
           printf (" Process %s Ready to be Initiated\n", this_proc -> procname);
	   break;
         case READY_TO_RESUME:
           printf (" Process %s Ready to Resume\n", this_proc -> procname);
	   break;
         case TERMINATED:
           printf (" Process %s Terminated\n", this_proc -> procname);
	   break;

         }
     }
     cpp = this_proc -> in_cps;
     while (cpp != 0) {
       for (i = 0; i < cpp -> elem_count; i++) {
       if (cpp -> elem_list[i].gen.connxn != 0)
           free(cpp -> elem_list[i].gen.connxn);
          }
       old_cpp = cpp;
       cpp = cpp -> next_cp;
       free(old_cpp);
       }
     cpp = this_proc -> out_cps;
     while (cpp != 0) {
       old_cpp = cpp;
       cpp = cpp -> next_cp;
       free(old_cpp);
       }

     old_proc = this_proc;
     this_proc = this_proc -> next_sibling_proc;
     free(old_proc);
     }

   this_proc = appl_ptr -> first_child_comp;
   while (this_proc != 0) {
     cpp = this_proc -> in_cps;
     while (cpp != 0) {
       for (i = 0; i < cpp -> elem_count; i++) {
       if (cpp -> elem_list[i].gen.connxn != 0)
           free(cpp -> elem_list[i].gen.connxn);
          }
       old_cpp = cpp;
       cpp = cpp -> next_cp;
       free(old_cpp);
       }
     cpp = this_proc -> out_cps;
     while (cpp != 0) {
       old_cpp = cpp;
       cpp = cpp -> next_cp;
       free(old_cpp);
       }

     old_proc = this_proc;
     this_proc = this_proc -> next_sibling_proc;
     free(old_proc);
     }

   if (!deadsw) {
     free(appl_ptr);
     printf("Done\n");
     }
   }