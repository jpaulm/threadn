
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "compsvcs.h"


  THRCOMP thrembrk(anchor proc_anchor)
{
  void *ptr;
  void *optr;
  int value;
  long size;
  char *type;
  char *otype;
  unsigned long len;
  port_ent port_tab[2];

  value = dfsdfpt(proc_anchor, 2, port_tab,"IN","OUT");

  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  if (value != 0) return(0);
  if (strcmp(type,"(") == 0)
     value = dfsdrop(proc_anchor, &ptr);
  else
     value = dfssend(proc_anchor, &ptr, &port_tab[1], 0);

  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  optr = ptr;
  otype = type;
  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);

  while (value == 0)
    {
      value = dfssend(proc_anchor, &optr, &port_tab[1], 0);
      optr = ptr;
      otype = type;
      value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
      }
  if (strcmp(otype,")") == 0)
      value = dfsdrop(proc_anchor, &optr);
  else
      value = dfssend(proc_anchor, &optr, &port_tab[1], 0);

   return(0);
}

