  /* THCOUNT counts the number of entities in the incoming data stream and
     sends out a COUNT entity with the ASCII translation of the count.  The
     incoming entities can optionally be passed to the output stream.

     CALLING THCOUNT:
     input data stream -> IN count_ents [OUT -> output data stream ],
     count_ents(THCOUNT) COUNT -> count entity;

     LIMITATIONS:
     There cannot be more than 16 Million entities to count.

     DEFAULTS:
     None

*/


  #include <setjmp.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "compsvcs.h"

  THRCOMP thcount(anchor proc_anchor)
  {
  void *ptr;
  char *dptr;
  int value;
  long size, count;
  char *type;
  port_ent port_tab[3];


  value = dfsdfpt(proc_anchor, 3, port_tab,"IN","OUT","COUNT");

/* count incoming entities
*/
  count = 0;
  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  while (value == 0)
    {
      count++;
      value = dfssend(proc_anchor, &ptr, &port_tab[1], 0);
      if (value == 2)
	 value = dfsdrop(proc_anchor, &ptr);

      value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
    }

/* create and send count entity
*/
  value = dfscrep(proc_anchor, &ptr, 10, "COUNT");
  dptr = (char *) ptr;
  ltoa(count,dptr,10);
  value = dfssend(proc_anchor, &ptr, &port_tab[2], 0);
  return(0);
}










