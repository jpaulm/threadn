   #if __OS2__ == 1

     #define INCL_DOS
     #include <os2.h>
   #else
     #include <dos.h>
   #if defined _Windows
       #include <windows.h>
     #endif
   #endif

   /* #define DEBUG */ 

   #include <stdarg.h>
   #include <stdio.h>
   #include <io.h>
   #include <ctype.h>
   #include <setjmp.h>
   #include <stdlib.h>
   #include <malloc.h>
   #include <string.h>
   #include <time.h>
   #include "thxiip.h"
   #include "threads.h"
   #include "thxscan.h"

   #define FALSE 0
   #define TRUE 1
   #define INPUT 0
   #define OUTPUT 1
   #define THRSIZE 6000

   extern int thz(int code, process *proc_ptr, ...);
   process * find_proc(proc_ent *proc_tab, char name[32]);
   void close_input_cps(process *pptr);
   int check_input_cps(process *pptr);
   void close_output_cps(process *pptr);
   void thxfcbs(appl *appl_ptr);
   void thxdcbs(appl *appl_ptr);
   void disp_IP(IPh HUGE * this_IP);
   /* first param of buildnet is subnet address; last is whole net address */
   int buildnet(label_ent * label_ptr, process *mother,
       appl *appl_ptr, label_ent *label_tab);
   int findmod(char * p);
   int thxscan(FILE *fp, cnxt_ent *cnxt_tab, proc_ent *proc_tab,
                   label_ent *label_tab, char file_name[10],
                   int *label_count, int *proc_count, int *cnxt_count );
   /* comp_ent * find_comp(struct comp_table *CTptr, char name[32]); */
   int thzsend(process *pptr, void **ptr, port_ent *peptr, int elem_no);
   int thzcrep(process *pptr, void **ptr, long IPsize, char *type);
   int thzdrop(process *pptr, void **ptr);
   IPh HUGE  * thzallc(long lth, process * ptr);
   void thzfree(IPh HUGE * p, process * ptr);
   void thzputs(process * p, char * buffer);

   #if __OS2__ == 1 
   extern "C" void thzsstk(void HUGE * ptr);
   #endif


   process *curr_proc_ptr;     /* must be in data segment */
   unsigned int segm,offs, stk_len;

   struct modent {
      char name[8+1];
      int (FAR PASCAL *addr)(_anchor anch);
      HINSTANCE hinst;
      }
      modents[100];
   int modentnas = 0;

   
   #ifdef _Windows
     extern unsigned _stklen = 643210U;
   #endif

   void FAR threads(label_ent * label_tab, int dynam, FILE * fp,
      int timereq)    {
                 
   int value, i, attsw;
   
   appl *appl_ptr;
   process *this_proc;
   cnxt_ent *cnxt_tab;
   proc_ent *proc_tab;

   /* label_ent *label_tab; */
   int label_count = 0;
   int proc_count = 0;
   int cnxt_count = 0;
   char file_name[10];
  
   cp *cpp;
   time_t st_time, end_time;
   time_t *tp = 0;
   time_t secs;

   #if __OS2__ == 1
   static char * stk_ptr;
   #endif

   #ifdef DEBUG
   static char pause[132];
   #endif
  
   st_time = time(tp);
   
   if (!dynam) {
      cnxt_tab = label_tab -> cnxt_ptr;
      proc_tab = label_tab -> proc_ptr;
      }
   else {
      

      cnxt_tab = (cnxt_ent *)calloc(200,sizeof(cnxt_ent));
      proc_tab = (proc_ent *)calloc(100,sizeof(proc_ent));
      label_tab = (label_ent *)calloc(100,sizeof(label_ent));

      strcpy(file_name, "\0");

      if (thxscan(fp, cnxt_tab, proc_tab, label_tab, file_name,
           &label_count, &proc_count, &cnxt_count) != 0)
         goto scan_err;
      
     }
   appl_ptr =(appl*)  malloc(sizeof(appl));
   strcpy(appl_ptr -> name,"APPL");
   appl_ptr -> first_ready_proc = 0;
   appl_ptr -> first_child_proc = 0;
   appl_ptr -> first_child_comp = 0;

   appl_ptr -> dynam = dynam;
   appl_ptr -> ds = _DS;

   buildnet(label_tab, 0, appl_ptr, label_tab);

   /* Look for processes with no input ports */

   
   this_proc = appl_ptr -> first_child_proc;
   while (this_proc != 0)
     {
       attsw = TRUE;
       cpp = this_proc -> in_cps;
       while (cpp != 0)
	 {
	   for (i = 0; i < cpp -> elem_count; i++) {
	      if (cpp -> elem_list[i].gen.connxn != 0 &&
		! cpp -> elem_list[i].is_IIP)
		 attsw = FALSE;
	      }
	   cpp = cpp -> next_cp;
	 }
       if (attsw)
	   {
	      this_proc -> status = READY_TO_BE_INITIATED;
	      this_proc -> next_proc = appl_ptr -> first_ready_proc;
	      appl_ptr -> first_ready_proc = this_proc;
	   }
       
       this_proc = this_proc -> next_sibling_proc;
     }

   if (dynam) {
      free(cnxt_tab);
      free(proc_tab);
      free(label_tab);
      }

   /* Start execution  */

   if (1 == setjmp(appl_ptr -> alloc_state)) {
      appl_ptr->alloc_ptr = (IPh HUGE *) malloc(appl_ptr->alloc_lth);
      longjmp(curr_proc_ptr->alloc_state, 0);
      }

   if (1 == setjmp(appl_ptr -> free_state)) {
      free(appl_ptr->alloc_ptr); 
      longjmp(curr_proc_ptr->free_state, 0);
      }

   /*
   if (1 == setjmp(appl_ptr -> puts_state)) {
      puts(appl_ptr -> puts_ptr);
      longjmp(curr_proc_ptr->puts_state, 0);
      }

   if (1 == setjmp(appl_ptr -> putc_state)) {
      putchar(appl_ptr -> putc_char);
      longjmp(curr_proc_ptr->putc_state, 0);
      }
   */


   /* This is the place that components jump to to switch processes */

   value = setjmp(appl_ptr -> state);

   curr_proc_ptr = appl_ptr -> first_ready_proc;

   if (curr_proc_ptr == 0) goto X;          // no more ready processes - finished or deadlock

   appl_ptr -> first_ready_proc = curr_proc_ptr -> next_proc;

   if (curr_proc_ptr -> status == READY_TO_RESUME) goto Y;
   if (curr_proc_ptr -> status == TERMINATED) goto Z;

   /* any other condition - initiate a process  */

    if (curr_proc_ptr -> trace)
	 printf("%s Initiated\n",curr_proc_ptr -> procname);
    curr_proc_ptr -> status = ACTIVE;

    
    if ((curr_proc_ptr -> wptr = (char*) farmalloc(THRSIZE)) == NULL) {
	 printf("Not enough storage for component stack\n");
	 return;
         }
                    
    segm = FP_SEG(curr_proc_ptr -> wptr);
    offs = FP_OFF(curr_proc_ptr -> wptr);

    #ifdef DEBUG
       printf("alloc wptr - seg, off %04X:%04X\n", segm, offs);
    #endif

    memset(curr_proc_ptr->wptr, 0xff, THRSIZE);

    #ifdef DEBUG
    gets(pause);
    #endif

    #if __OS2__ == 1
     stk_ptr = (char *) curr_proc_ptr -> wptr;
     stk_ptr = stk_ptr + THRSIZE - 40;
     thzsstk(stk_ptr);
    #else
    
    #if defined _Windows

    #else   
     segm = segm * 16;
     segm = segm + offs + 16;
     segm = segm / 16 + 1;
     offs = 0;

 
    #endif  
    offs = offs + THRSIZE - 40;
    _AX = offs;
    _DX = segm;
           
    asm {

     cli 
     mov ss,dx
     mov sp,ax
     sti

     } 

    #endif

    #ifdef DEBUG
    gets(pause);
    #endif
   
    if (! curr_proc_ptr -> terminating || curr_proc_ptr -> must_run)

       for (;;)  {      // do forever - down to 'A' - for a particular process
	 if (curr_proc_ptr -> trace) 
  	    printf("%s Activated\n", curr_proc_ptr -> procname);
            
         if (curr_proc_ptr -> faddr == 0)  {
            printf("No routine for %s\n", curr_proc_ptr -> procname);
            return;
	    }

	 /* Invoke component code  - processes can only switch within this call,
	    or at end of loop when process goes dormant (no data currently available) */

	 curr_proc_ptr -> value = 
	   curr_proc_ptr -> faddr (curr_proc_ptr -> proc_anchor);

	 /* component code returns to driver */

         if (curr_proc_ptr -> value > 0)
            printf ("Process %s returned with value %d\n",
               curr_proc_ptr -> procname, curr_proc_ptr -> value);  

         if (curr_proc_ptr -> first_owned_IP != 0)
           while (curr_proc_ptr -> first_owned_IP != 0) {
	    disp_IP (curr_proc_ptr -> first_owned_IP);
            curr_proc_ptr -> first_owned_IP = curr_proc_ptr ->
	       first_owned_IP -> next_IP;
	    }

         if (curr_proc_ptr -> trace)
              printf("%s Deactivated with retcode %d\n",
	      curr_proc_ptr -> procname, curr_proc_ptr -> value);
         if (curr_proc_ptr -> value > 4) break;

         if ((curr_proc_ptr -> in_cps_status =
	     check_input_cps(curr_proc_ptr)) == 2)
	        break;
         if (curr_proc_ptr -> end_cp != 0) {
	      curr_proc_ptr -> value = thzcrep(curr_proc_ptr,
		  &curr_proc_ptr -> int_ptr, 0, "\0");
	      curr_proc_ptr -> int_pe.cpptr = curr_proc_ptr -> end_cp;
	      curr_proc_ptr -> value = thzsend(curr_proc_ptr,
		&curr_proc_ptr -> int_ptr, &curr_proc_ptr -> int_pe, 0);
	      if (curr_proc_ptr -> value > 0)
		  curr_proc_ptr -> value = thzdrop(curr_proc_ptr,
		   &curr_proc_ptr -> int_ptr);
              }
	 if (curr_proc_ptr -> in_cps_status == 0) continue;

	 curr_proc_ptr -> status = DORMANT;

	 /* if we dropped in to this point, setjmp and go switch processes -
	    if this was jumped to via a longjmp, go back around loop       */
	   
	 if (setjmp(curr_proc_ptr -> state) == 0)
	     longjmp (curr_proc_ptr -> appl_ptr -> state, 2);
	 }                                 // end of 'forever' loop ('A')

   /************************************************************/
   /* at this point, process has terminated, so go do clean-up */

   curr_proc_ptr -> status = TERMINATED;
  

   /* termination logic starts here */

   if (curr_proc_ptr -> stack != 0)  {
	printf("Process %s - stack not empty\n",curr_proc_ptr -> procname);
	return;
	}

   close_input_cps(curr_proc_ptr);
   close_output_cps(curr_proc_ptr);

   if (curr_proc_ptr -> trace) printf("%s Terminated\n",curr_proc_ptr -> procname);

    /* add curr_proc_ptr process back onto top of ready queue */
   curr_proc_ptr -> next_proc = curr_proc_ptr -> appl_ptr -> first_ready_proc;
   curr_proc_ptr -> appl_ptr -> first_ready_proc = curr_proc_ptr;

   longjmp (curr_proc_ptr -> appl_ptr -> state, 2);

   Z:

   #ifdef DEBUG
       segm = FP_SEG(curr_proc_ptr -> wptr);
       offs = FP_OFF(curr_proc_ptr -> wptr);
       printf("free wptr - seg, off %04X:%04X\n", segm, offs);

   gets(pause);
   #endif

   farfree(curr_proc_ptr -> wptr); 

   #ifdef DEBUG
       printf("Stack freed\n");
   #endif
   longjmp(curr_proc_ptr -> appl_ptr -> state, 3);  // see if any other processes ready to run

   /* process is now active - give control to it  */

 Y: curr_proc_ptr -> status = ACTIVE;
   longjmp(curr_proc_ptr -> state, 2);

X: /* no processes left - free control blocks and detect deadlock, if any */
   #ifdef DEBUG
      printf("Closing down\n");
   #endif
 
   thxfcbs(appl_ptr);

   #ifdef DEBUG
      printf("Closing down 2\n");
   #endif

   #if defined _Windows
   for (i=0; i < modentnas; i++) {
      FreeLibrary(modents[i].hinst);
   } /* endfor */

   #endif


   /* if (debug_threads) {
      
      printf("SS:%04X, SP:%04X\n",_SS,_SP);
      }
   */
    /* 
    if( _freemod( handle ))
    {
       printf("Error in freeing the %s DLL module\n",
                modname);
       exit(-1);
    }
    */

  

   end_time = time(tp);
   secs = end_time - st_time;
   if (timereq)
      printf("Elapsed time in seconds: %d\n",secs);
   exit(0);

scan_err: printf("Scan error\n");
   exit(4);

   }

   int check_input_cps(process *pptr)
   {
    /*  return 0 if data in cnxt; 1 if upstream not closed;
	 2 if upstream closed */
      unsigned int endsw;
      int i;
      cp *cpp;
      cnxt *cnp;
      endsw = 2;
      cpp = pptr -> in_cps;
      while (cpp != 0)
      {
	for (i = 0; i < cpp -> elem_count; i++) {
	   cnp = (cnxt *) cpp -> elem_list[i].gen.connxn;

	   if (cnp == 0 || cpp -> elem_list[i].is_IIP)
	      continue;
	   if (cnp -> IPcount != 0)
	      return(0);
	   if (cnp -> nonterm_upstream_cpelem_count != 0 &&
	     ! (cpp -> elem_list[i].closed)) endsw = 1;
	   }
	cpp = cpp -> next_cp;
      }
      return(endsw);
   }

   void close_input_cps(process *pptr)
   {
      cp *cpp;
      int i, value;
      port_ent port;
      int thzclos(process *pptr, port_ent *peptr,int i);

      cpp = pptr -> in_cps;
      while (cpp != 0)
      {

        port.cpptr = cpp;
        port.ret_code = 0;
        for (i = 0; i < cpp -> elem_count; i++) {
           if (cpp -> elem_list[i].gen.connxn == 0)
              continue;
	   if (! cpp -> elem_list[i].is_IIP)
	       value = thzclos(pptr, &port, i);
	    }
	   cpp = cpp -> next_cp;
      }
   }

   void close_output_cps(process *pptr)
   {
      cp *cpp;
      int i, value;
      port_ent port;
      int thzclos(process *pptr, port_ent *peptr,int i);

      cpp = pptr -> out_cps;
      while (cpp != 0)
      {

        port.cpptr = cpp;
        port.ret_code = 0;
        for (i = 0; i < cpp -> elem_count; i++) {
           if (cpp -> elem_list[i].gen.connxn == 0)
              continue;
	   value = thzclos(pptr, &port, i);
	    }
	   cpp = cpp -> next_cp;
      }

   }

   void disp_IP(IPh HUGE * this_IP) {
    char * dptr;
    IPh HUGE * ip;
    int i;
    unsigned j;
    long size;
    char *type;

    dptr = (char *) ((IP *) this_IP) -> datapart;
    ip = this_IP;
    size = ip -> IP_size;
    type = ip -> type;
    printf ("IP not disposed of - Length: %ld, Type: %s, Data:",
       size,  type);
    for (i = 0; i < size; i++)  {
	   j = (int) *(dptr + i);
	   printf("%c",j);
	   }
    printf("%c\n", (int) *(dptr + size));
   }

   int buildnet(label_ent * label_ptr, process *mother,
       appl *appl_ptr, label_ent *label_tab) {

   
   cp *cpp, *cpp2, *cppa;
   process *this_proc;
   proc_ent *curr_proc;
   int value, i, j;
   cnxt *cnxt_ptr;
   process *upstream_proc, *downstream_proc;
   cnxt_ent *curr_cnxt;
   label_ent *leptr;
   #if __OS2__ == 1
    PFN epaddr;                   /* pointer to component entry-point */
    APIRET rc;                    /* return code           */
                                  /* from DosQueryProcAddr */ 
    unsigned long handle;
    char dllname[20];

   #elif defined _Windows

    char szBuf[80];
    HINSTANCE hinstThComps;
    char dllname_2[20];
    char dllname[20];
   #endif
   
   curr_proc = label_ptr -> proc_ptr;

   while (curr_proc -> proc_name[0] != '\0') {
      this_proc = (process *) malloc(sizeof(process));
      strcpy(this_proc -> procname, curr_proc -> proc_name);
      strcpy(this_proc -> compname, curr_proc -> comp_name);

      this_proc -> appl_ptr = appl_ptr;

      this_proc -> next_proc = 0;
      this_proc -> has_run = FALSE;

      this_proc -> status = NOT_INITIATED;
      this_proc -> next_sibling_proc = 0;
      this_proc -> first_owned_IP = 0;
      this_proc -> proc_anchor.svc_addr = (int (*) (int, void * vp, ...)) thz;
      this_proc -> proc_anchor.proc_ptr = this_proc;
      this_proc -> mother_proc = mother;
      this_proc -> faddr = 0;
      this_proc -> in_cps = 0;
      this_proc -> out_cps = 0;
      this_proc -> begin_cp = 0;
      this_proc -> end_cp = 0;
      strcpy(this_proc -> int_pe.port_name, " ");
      this_proc -> int_pe.elem_count = 1;
      this_proc -> int_pe.cpptr = 0;
      this_proc -> int_pe.ret_code = 0;
      this_proc -> stack = 0;
      this_proc -> trace = curr_proc -> trace;
      this_proc -> terminating = FALSE;
      curr_proc -> proc_block = this_proc;
      curr_proc++;

   }

   curr_cnxt = label_ptr -> cnxt_ptr;

   while (curr_cnxt -> upstream_name[0] != '\0') {

     if (curr_cnxt -> downstream_name[0] == '*') {
         cpp2 = mother -> out_cps;
         while (cpp2 != 0) {
	   if (strcmp(curr_cnxt -> downstream_port_name, cpp2 -> port_name)
                    == 0)
	      break;
	   cpp2 = cpp2 -> next_cp;
           }
	 if (cpp2 == 0)
	     cnxt_ptr = 0;
	   else {
             cnxt_ptr = (cnxt *) cpp2 ->
		 elem_list[curr_cnxt -> downstream_elem_no].gen.connxn;
	     cpp2 -> elem_list[curr_cnxt -> downstream_elem_no].subdef =
		 TRUE;
	     }

         goto build_out_cp;
         }
     downstream_proc = find_proc(label_ptr -> proc_ptr,
         curr_cnxt -> downstream_name);
     if (downstream_proc == 0)
	return(4);

     cpp = downstream_proc -> in_cps;

     while (cpp != 0) {
       if (strcmp(curr_cnxt -> downstream_port_name, cpp -> port_name) == 0)
	  break;
       cpp = cpp -> next_cp;
       }
       i = curr_cnxt -> downstream_elem_no;
       if (cpp == 0) {
	  if (i < 20)
	     i = 20 - 1;
	  j = 0;
	  }
       else {
         if (i < cpp -> elem_count)
	    goto tdnast;
         j = cpp -> elem_count;
	 }
       i = ((i + 20) / 20) * 20;
       cppa =  (cp *)malloc(sizeof(cp) + (i-1) * sizeof(cp_elem));
       strcpy(cppa -> port_name, curr_cnxt -> downstream_port_name);
       cppa -> elem_count = i;
       cppa -> direction = INPUT;
       for (i = 0; i < j; i++)
	  memcpy (&cppa -> elem_list[i], &cpp -> elem_list[i],
		    sizeof(cp_elem));
       if (j > 0)
	 strcpy(cpp -> port_name, " ");
       cpp = cppa;
       cpp -> next_cp = downstream_proc -> in_cps;
       downstream_proc -> in_cps = cpp;

       for (i = j; i < cpp -> elem_count; i++) {
	  cpp -> elem_list[i].gen.connxn = 0;
	  cpp -> elem_list[i].closed = FALSE;
	  cpp -> elem_list[i].is_IIP = FALSE;
	  cpp -> elem_list[i].subdef = FALSE;
	  }

tdnast:
     if (curr_cnxt -> downstream_port_name[0] == '*')
         downstream_proc -> begin_cp = cpp;

     i = curr_cnxt -> downstream_elem_no;

     if (curr_cnxt -> upstream_name[0] != '!') {

       if ((cnxt_ptr = (cnxt *) cpp -> elem_list[i].gen.connxn) == 0) {
	 if (cpp -> elem_list[i].is_IIP)
	   printf("Cannot have connection and IIP on same port element\n");
       if (curr_cnxt -> upstream_name[0] == '*') {
         cpp2 = mother -> in_cps;
         while (cpp2 != 0) {
	   if (strcmp(curr_cnxt -> upstream_port_name, cpp2 -> port_name) ==
                      0)
	      break;
	   cpp2 = cpp2 -> next_cp;
           }
	 if (cpp2 == 0)
	    cnxt_ptr = 0;
	   else
	    {
             cnxt_ptr = (cnxt *) cpp2 ->
		    elem_list[curr_cnxt -> upstream_elem_no].gen.connxn;
	     if (cpp2 -> elem_list[curr_cnxt -> upstream_elem_no].subdef)
		 printf("Input port %s of subnet already defined\n",
                       cpp2 -> port_name);
	      else {
		 if (cnxt_ptr != 0) {
                    cpp2 -> elem_list[curr_cnxt -> upstream_elem_no].subdef =
		       TRUE;
                    if (cpp -> elem_list[i].gen.connxn != 0) {
                       printf("Connection already in use\n");
                       return(4);
                       }
                    else
                       cpp -> elem_list[i].gen.connxn = cnxt_ptr;

                    if (cpp2 -> elem_list[curr_cnxt -> upstream_elem_no].is_IIP)
	                cpp -> elem_list[i].is_IIP = TRUE;
	              else
                        cnxt_ptr -> fed_proc = downstream_proc;
		    }
	         }
	    }

         goto get_next_conn;
         }

       cnxt_ptr =  (cnxt *)malloc(sizeof(cnxt));
       cnxt_ptr -> first_IPptr = 0;
       cnxt_ptr -> last_IPptr = 0;
       cnxt_ptr -> IPcount = 0;
       cnxt_ptr -> max_IPcount = curr_cnxt -> capacity;
       if (cnxt_ptr -> max_IPcount == -1)
          cnxt_ptr -> max_IPcount = 1;
       cnxt_ptr -> procs_wtg_to_send = 0;
       cnxt_ptr -> total_upstream_cpelem_count = 0;
       cnxt_ptr -> nonterm_upstream_cpelem_count = 0;
       if (cpp -> elem_list[i].gen.connxn != 0) {
            printf("Connection already in use\n");
            return(4);
            }
       else
           cpp -> elem_list[i].gen.connxn = cnxt_ptr;

       cnxt_ptr -> fed_proc = downstream_proc;
       cnxt_ptr -> fedproc_wtg_to_recv = FALSE;
       }
  build_out_cp:
       upstream_proc = find_proc(label_ptr -> proc_ptr,
              curr_cnxt -> upstream_name);

       cpp = upstream_proc -> out_cps;
       while (cpp != 0) {
	 if (strcmp(curr_cnxt -> upstream_port_name, cpp -> port_name) == 0)
	    break;
	 cpp = cpp -> next_cp;
       }
       i = curr_cnxt -> upstream_elem_no;
       if (cpp == 0) {
	  if (i < 20)
	     i = 20 - 1;
	  j = 0;
	  }
       else {
         if (i < cpp -> elem_count)
	    goto tupast;
         j = cpp -> elem_count;
	 }
       i = ((i + 20) / 20) * 20;
       cppa =  (cp *)malloc(sizeof(cp) + (i-1) * sizeof(cp_elem));

       strcpy(cppa -> port_name, curr_cnxt -> upstream_port_name);
       cppa -> elem_count = i;
       cppa -> direction = OUTPUT;
       for (i = 0; i < j; i++)
	  memcpy (&cppa -> elem_list[i], &cpp -> elem_list[i],
		    sizeof(cp_elem));
       if (j > 0)
	 strcpy(cpp -> port_name, " ");
       cpp = cppa;
       cpp -> next_cp = upstream_proc -> out_cps;
       upstream_proc -> out_cps = cpp;

       for (i = j; i < cpp -> elem_count; i++) {
	  cpp -> elem_list[i].gen.connxn = 0;
	  cpp -> elem_list[i].closed = FALSE;
	  cpp -> elem_list[i].is_IIP = FALSE;
	  cpp -> elem_list[i].subdef = FALSE;
	  }

tupast:
       if (curr_cnxt -> upstream_port_name[0] == '*')
         upstream_proc -> end_cp = cpp;

       i = curr_cnxt -> upstream_elem_no;

       curr_cnxt -> gen.connxn = cnxt_ptr;
       }
      else
        cpp -> elem_list[i].is_IIP = TRUE;
      if (cpp -> elem_list[i].gen.connxn != 0) {
	 printf("Connection already in use\n");
	 return(4);
	 }

      cpp -> elem_list[i].gen.connxn = (cnxt*) curr_cnxt -> gen.connxn;

get_next_conn:  curr_cnxt++;
   }


   curr_proc = label_ptr -> proc_ptr;

   while (curr_proc -> proc_name[0] != '\0') {
     this_proc = (process *) curr_proc -> proc_block;
     if (curr_proc -> composite) {

        leptr = &label_tab[curr_proc -> label_count];

/* this will cause recursive use of buildnet */
        buildnet(leptr,
              this_proc,
              appl_ptr,
              label_tab);

        cpp = this_proc -> out_cps;
        while (cpp != 0)  {
           for (i = 0; i < cpp -> elem_count; i++) {
              cnxt_ptr = (cnxt *) cpp -> elem_list[i].gen.connxn;
              if (cnxt_ptr == 0)
                 continue;
	      if (!cpp -> elem_list[i].subdef)
		   printf("Port %s used by process %s but not defined\n",
		     cpp -> port_name, this_proc -> procname);
              cpp -> elem_list[i].gen.connxn = 0;
              }
           cpp = cpp -> next_cp;
           }

	cpp = this_proc -> in_cps;
        while (cpp != 0)
          {
             for (i = 0; i < cpp -> elem_count; i++) {
	       cnxt_ptr = (cnxt *) cpp -> elem_list[i].gen.connxn;
               if (cnxt_ptr == 0)
                  continue;
	       if (!cpp -> elem_list[i].subdef)
		    printf("Port %s used by process %s but not defined\n",
		      cpp -> port_name, this_proc -> procname);
               cpp -> elem_list[i].gen.connxn = 0;
              }
          cpp = cpp -> next_cp;
          }

        this_proc -> next_sibling_proc = appl_ptr -> first_child_comp;
        appl_ptr -> first_child_comp = this_proc;
        }
     else {
	if (curr_proc -> faddr == 0) {

           #if __OS2__ == 1
            strcpy(dllname, curr_proc -> comp_name);
	     
            /*  dynamically load the 'mark' DLL  */
            if (_loadmod(dllname, &handle) )
              {
                printf("Error loading module %s\n", dllname );
                exit (-1);
              }
 
            /*  get function address from DLL  */
            rc = DosQueryProcAddr( handle, 0, dllname, &epaddr );
            if (rc != 0) 
                {
                  printf("Error locating address of function %s\n",
                     dllname);
                 _freemod( handle );
                  exit(-1);
                 }
            curr_proc -> faddr = (int (*) (anchor)) epaddr;


           #elif defined _Windows
	     SetErrorMode(SEM_NOOPENFILEERRORBOX);

	     /* Load the library module. */
	     strcpy(dllname, curr_proc -> comp_name);
	     strcpy(dllname_2, curr_proc -> comp_name);
             i = findmod(dllname);
	     if (i == -1) {
               hinstThComps = LoadLibrary(strcat(dllname, ".DLL"));

	       if (hinstThComps <= HINSTANCE_ERROR) {
	         strcpy(szBuf, "LoadLibrary failed");
	         MessageBox(NULL, szBuf, "Library Functions", MB_ICONHAND);
	         }
	   
	       else
                 {
             /* Retrieve the address of the actual function. */
	   
               (FARPROC) curr_proc -> faddr  =
	        GetProcAddress(hinstThComps, dllname_2);
               
               if (curr_proc -> faddr == NULL) {
	         strcpy(szBuf, "GetProcAddress failed");
	         MessageBox(NULL, szBuf, "Library Functions", MB_ICONHAND);
	       }
               i = modentnas;
	       strcpy(modents[i].name, dllname);
	       modents[i].addr = curr_proc -> faddr;
	       modents[i].hinst = hinstThComps;
	       modentnas++;
             }
           }
           else 
	      curr_proc -> faddr = modents[i].addr;
        #else
           printf("Module not found %s\n", 
               curr_proc -> comp_name);
	#endif   
	}

	this_proc -> faddr =  curr_proc -> faddr;
	this_proc -> must_run = curr_proc -> must_run;
        cpp = this_proc -> out_cps;
        while (cpp != 0)  {
           for (i = 0; i < cpp -> elem_count; i++) {
              cnxt_ptr = (cnxt *) cpp -> elem_list[i].gen.connxn;
              if (cnxt_ptr == 0)
                 continue;
              cnxt_ptr -> total_upstream_cpelem_count++;
	      cnxt_ptr -> nonterm_upstream_cpelem_count++;
              }
           cpp = cpp -> next_cp;
           }

	 this_proc -> next_sibling_proc = appl_ptr -> first_child_proc;
         appl_ptr -> first_child_proc = this_proc;
        }

    curr_proc++;
   }
 }

   process * find_proc(proc_ent *proc_tab, char name[32])
  {
   proc_ent *PEptr;
   PEptr = proc_tab;
   while (PEptr -> proc_name[0] != '\0' &&
       strcmp(PEptr -> proc_name, name) != 0) {
	  PEptr++;
         }
   if (PEptr -> proc_name[0] == '\0') {
      printf ("Process name %s not found\n", name);
      return(0);
      }
   return (process *)(PEptr -> proc_block);
}

   int findmod (char * p) {
    int i;
    for (i = 0; i < modentnas; i++) {
       if (0 == strcmp(modents[i].name, p))
            return i;
       } /* endfor */
     return -1;
   }
