

  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "compsvcs.h"

  THRCOMP THREVERS(anchor proc_anchor)
{
  void *ptr;
  void *optr;
  int value;
  long size;
  long level = 0;
  char *type;
  port_ent port_tab[2];

  value = dfsdfpt(proc_anchor, 2, port_tab,"IN","OUT");

  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  while (value == 0) {
     if (strcmp(type,")") == 0 && level == 1) {
       value = dfspop(proc_anchor, &optr, &size, &type);
       while (value == 0) {
         value = dfssend(proc_anchor, &optr, &port_tab[1], 0);
         value = dfspop(proc_anchor, &optr, &size, &type);
       }
     value = dfssend(proc_anchor, &ptr, &port_tab[1], 0);
     }
     else
       if (level == 0)
           value = dfssend(proc_anchor, &ptr, &port_tab[1], 0);
       else
           value = dfspush(proc_anchor, &ptr);

     if (strcmp(type,"(") == 0)
       level++;
      else
        if (strcmp(type,")") == 0)
           level--;

     value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  }

   return(0);
}

