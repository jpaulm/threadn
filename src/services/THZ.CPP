   #include <stdarg.h>
   #include <setjmp.h>
   #include <stdio.h>
   #include <string.h>
   #include "thxiip.h"
   #include "threads.h"


   int thzsend(process *pptr, void **ptr, port_ent *peptr, int elem_no);
   int thzrecv(process *pptr, void **ptr, port_ent *peptr, int elem_no,
       long *size, char **typep);
   int thzcrep(process *pptr, void **ptr, long IPsize, char *type);
   int thzdrop(process *pptr, void **ptr);
   int thzdfpt(process *pptr, int port_count, port_ent *peptr);
   int thzpush(process *pptr, void **ptr);
   int thzpop(process *pptr, void **ptr, long *size, char **typep);
   int thzclos(process *pptr, port_ent *peptr, int elem_no);


   int thz(int code, process *pptr, ...)
   {
   va_list ap;
   int value, port_count, elem_no, ch;
   long IPsize;
   long *size;
   char *type;
   char **typep;
   port_ent *peptr;
   void **ptr;
   void thzputs(process * p, char * buffer);
   void thzputc(process * p, int k);  
   char *string;


   if (1 == pptr -> appl_ptr -> dynam) {
     pptr -> ds = _DS;
     _DS = pptr -> appl_ptr -> ds;
     }

   va_start (ap, pptr);
   switch (code) {
   case 1: ptr = va_arg(ap,void **);
           peptr = va_arg(ap,port_ent *);
	   elem_no = va_arg(ap,int);
	   value = thzsend(pptr, ptr, peptr, elem_no); break;
   case 2: ptr = va_arg(ap,void **);
	   peptr = va_arg(ap,port_ent *);
	   elem_no = va_arg(ap,int);
           size = va_arg(ap,long *);
	   typep = va_arg(ap, char **);
	   value = thzrecv(pptr, ptr, peptr, elem_no,
	     size, typep); break;
   case 3: ptr = va_arg(ap,void **);
           IPsize = va_arg(ap,long);
	   type = va_arg(ap,char *);
	   value = thzcrep(pptr, ptr, IPsize, type); break;
   case 4: ptr = va_arg(ap,void **);
	   value = thzdrop(pptr, ptr); break;
   case 5: port_count = va_arg(ap,int);
           peptr = va_arg(ap,port_ent *);
	   value = thzdfpt(pptr, port_count, peptr); break;
   case 6: ptr = va_arg(ap,void **);
	   value = thzpush(pptr, ptr); break;
   case 7: ptr = va_arg(ap,void **);
           size = va_arg(ap,long *);
	   typep = va_arg(ap, char **);
	   value = thzpop(pptr, ptr, size, typep); break;
   case 8: peptr = va_arg(ap,port_ent *);
	   elem_no = va_arg(ap,int);
	   value = thzclos(pptr, peptr, elem_no); break;
   case 9: string = va_arg(ap,char *);
           thzputs(pptr, string);  break;
   case 10: ch = va_arg(ap, int);
	   thzputc(pptr, ch);  break;
   }
   va_end(ap);
   if (code < 9 && pptr -> trace && value > 0)
       MSG2("%s: Service retcode %d\n",
	  pptr -> procname, value);

   if (1 == pptr -> appl_ptr -> dynam) 
       _DS = pptr -> ds;
     

   return(value);

}

   void thzputs(process * p, char * buffer) {
   /* p -> appl_ptr -> puts_ptr = buffer;
   if (0 == setjmp(p -> puts_state))
     longjmp(p -> appl_ptr -> puts_state, 0); */
   puts(buffer);
   }


   void thzputc(process * p, int k){
   /* p -> appl_ptr -> putc_char = k;
   if (0 == setjmp(p -> putc_state))
     longjmp(p -> appl_ptr -> putc_state, 0); */
   putchar(k);

   }