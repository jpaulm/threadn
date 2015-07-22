

  #include <stdio.h>
  #include <string.h>
  #include "compsvcs.h"


  THRCOMP threpl(anchor proc_anchor)
{
  void *ptr;
  void *ptr_2;
  int value, i;
  long size;
  char *type;
  port_ent port_tab[2];

  value = dfsdfpt(proc_anchor, 2, port_tab,"IN","OUT");
  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  while (value == 0)
    {
      for (i = 0; i < port_tab[1].elem_count - 1; i++) {
        value = dfscrep(proc_anchor, &ptr_2, size, type);
        memcpy(ptr_2, ptr, size);
        value = dfssend(proc_anchor, &ptr_2, &port_tab[1], i + 1);
	if (value != 0)
	  value = dfsdrop(proc_anchor, &ptr_2);
	}
      value = dfssend(proc_anchor, &ptr, &port_tab[1], 0);
      value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
    }
  return(0);
}

