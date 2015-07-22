
   #include <stdarg.h>
   #include <string.h>
   #include <stdio.h>
   #include "thzcbs.h" 

   int const ONE = 1;
   int const TWO = 2;
   int const THREE = 3;
   int const FOUR = 4;
   int const FIVE = 5;
   int const SIX = 6;
   int const SEVEN = 7;
   int const EIGHT = 8;
   int const NINE = 9;
   int const TEN = 10;

   int dfssend(anchor proc_anchor, void **ptr, port_ent *peptr,
       int elem_no)
{
   int value;

   value = proc_anchor.svc_addr (ONE, proc_anchor.reserved, ptr, peptr,
       elem_no)
       ;
   return(value);
}


   int dfsrecv(anchor proc_anchor, void **ptr, port_ent *peptr,
       int elem_no, long *size, char **type)
{
   int value;

   value = proc_anchor.svc_addr (TWO, proc_anchor.reserved, ptr, peptr,
       elem_no, size, type);
   return(value);
}

   int dfscrep(anchor proc_anchor, void **ptr, long size, char *type)
{
   int value;

   value = proc_anchor.svc_addr (THREE, proc_anchor.reserved, ptr, size,
       type);
   return(value);
}

 int dfsdrop(anchor proc_anchor, void **ptr)
{
   int value;

   value = proc_anchor.svc_addr (FOUR, proc_anchor.reserved, ptr);
   return(value);
}

 int dfsdfpt(anchor proc_anchor, int port_count, port_ent *peptr, ...)
{
   va_list ap;
   int i;
   int value;

   va_start (ap, peptr);
   for (i = 0; i < port_count; i++) {
	strcpy((peptr+i) -> port_name, va_arg(ap, char *));
	}
   va_end (ap);
   value = proc_anchor.svc_addr (FIVE, proc_anchor.reserved, port_count,
     peptr);
   return(value);
}

   int dfspush(anchor proc_anchor, void **ptr )
{
   int value;

   value = proc_anchor.svc_addr (SIX, proc_anchor.reserved, ptr);
   return(value);
}


   int dfspop(anchor proc_anchor, void **ptr, long *size, char **type)
{
   int value;

   value = proc_anchor.svc_addr (SEVEN, proc_anchor.reserved, ptr, size, type);
   return(value);
}

   int dfsclos(anchor proc_anchor, port_ent *peptr,
       int elem_no)
{
   int value;

   value = proc_anchor.svc_addr (EIGHT, proc_anchor.reserved, peptr,
       elem_no);
   return(value);
}


   void dfsputs(anchor proc_anchor, char * p) {


   proc_anchor.svc_addr (NINE, proc_anchor.reserved, p);
   return;
}

   void dfsputc(anchor proc_anchor, int k) {


   proc_anchor.svc_addr (TEN, proc_anchor.reserved, k);
   return;
}
