#if __OS2__ == 1 

  #define HUGE
  #define FAR
  #define PASCAL

#else 

  #define HUGE huge
  #define FAR far
  #define PASCAL pascal

#endif


   #include "thzcbs.h"

   int dfscrep(anchor proc_anchor, void **ptr, long size, char *type);
   int dfssend(anchor proc_anchor, void **ptr, port_ent *peptr,
       int elem_no);
   int dfsrecv(anchor proc_anchor, void **ptr, port_ent *peptr,
       int elem_no, long *size, char **type);
   int dfsdrop(anchor proc_anchor, void **ptr);
   int dfsdfpt(anchor proc_anchor, int port_count, port_ent *peptr, ...);
   int dfspush(anchor proc_anchor, void **ptr);
   int dfspop(anchor proc_anchor, void **ptr, long *size, char **type);
   int dfsclos(anchor proc_anchor, port_ent *peptr,
       int elem_no);
   void dfsputs(anchor proc_anchor, char * p);
   void dfsputc(anchor proc_anchor, int k);

#if __OS2__ == 1 

   #define THRCOMP extern "C" int _Export 

#elif  defined _Windows
   #define THRCOMP  extern "C" int FAR PASCAL _export

#else
   #define THRCOMP extern "C" int FAR PASCAL

#endif