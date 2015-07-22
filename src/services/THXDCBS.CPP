
   #include <stdio.h>
   #include <setjmp.h>
   #include "thxiip.h"
   #include "threads.h"

   void dump(void *cbptr, char cbtype);

   void thxdcbs(appl *appl_ptr)
   {
   cp *cpp, *old_cpp;
   process *this_proc, *old_proc;
   int i;

   this_proc = appl_ptr -> first_child_proc;
   while (this_proc != 0) {

     cpp = this_proc -> in_cps;
     while (cpp != 0) {
       for (i = 0; i < cpp -> elem_count; i++) {
       if (cpp -> elem_list[i].gen.connxn != 0)
           dump(cpp -> elem_list[i].gen.connxn,'X');
          }
       old_cpp = cpp;
       cpp = cpp -> next_cp;
       dump(old_cpp,'C');
       }
     cpp = this_proc -> out_cps;
     while (cpp != 0) {
       old_cpp = cpp;
       cpp = cpp -> next_cp;
       dump(old_cpp,'C');
       }

     old_proc = this_proc;
     this_proc = this_proc -> next_sibling_proc;
     dump(old_proc,'P');
     }

   this_proc = appl_ptr -> first_child_comp;
   while (this_proc != 0) {
     cpp = this_proc -> in_cps;
     while (cpp != 0) {
       for (i = 0; i < cpp -> elem_count; i++) {
       if (cpp -> elem_list[i].gen.connxn != 0)
           dump(cpp -> elem_list[i].gen.connxn,'X');
          }
       old_cpp = cpp;
       cpp = cpp -> next_cp;
       dump(old_cpp,'C');
       }
     cpp = this_proc -> out_cps;
     while (cpp != 0) {
       old_cpp = cpp;
       cpp = cpp -> next_cp;
       dump(old_cpp,'C');
       }

     old_proc = this_proc;
     this_proc = this_proc -> next_sibling_proc;
     dump(old_proc,'P');
     }

   dump(appl_ptr,'A');
   }

   void dump(void *cbptr, char cbtype)
   {
    process *pptr;
    cp *cpptr;

    switch (cbtype) {
	 case 'A':
           printf ("Appl\n");
	   break;
	 case 'P':
	   pptr = (process *) cbptr;
           printf ("Process %s\n",pptr -> procname);
	   break;
	 case 'C':
	   cpptr = (cp *) cbptr;
           printf ("CP %s\n", cpptr -> port_name);
	   break;
	 case 'X':
           printf ("Connection\n");
	   break;


         }
   }