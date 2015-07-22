
  #include <stdlib.h>
   #include <stdio.h>
  #include <string.h>
  #include "compsvcs.h"


  THRCOMP thencbrk(anchor proc_anchor)
{
  void *ptr;
  int value;
  long size;
  char *type;
  port_ent port_tab[2];

  value = dfsdfpt(proc_anchor, 2, port_tab,"IN","OUT");
  value = dfscrep(proc_anchor, &ptr, 0, "(");
  value = dfssend(proc_anchor, &ptr, &port_tab[1], 0);

  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);

  while (value == 0)
    {
      value = dfssend(proc_anchor, &ptr, &port_tab[1], 0);
      value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
      }
  value = dfscrep(proc_anchor, &ptr, 0, ")");
  value = dfssend(proc_anchor, &ptr, &port_tab[1], 0);
  return(0);
}

