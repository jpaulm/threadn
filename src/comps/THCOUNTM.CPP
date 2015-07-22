

  /* THCOUNTM counts the number of entities in the incoming data stream and
     sends out a COUNT entity after each thousand with the ASCII translation
     of the count.  The incoming entities can optionally be passed to the
     output stream.

     CALLING THCOUNTM:
     input data stream -> IN count_ents [OUT -> output data stream ],
     count_ents(THCOUNTM) COUNT -> count entities;

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

  struct _Data
  {
     char string[1];
  };
  typedef struct _Data Data;


  THRCOMP thcountm(anchor proc_anchor)
  {
  void *ptr, *count_ptr;
  Data *dptr;
  int value, thousand;
  long size, count;
  char *type;
  port_ent port_tab[3];
  value = dfsdfpt(proc_anchor, 3, port_tab, "IN", "OUT", "COUNT");

/* count incoming entities
*/
  count = 0;
  thousand = 0;
  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  while (value == 0)
    {
      count++;
      thousand++;
      value = dfssend(proc_anchor, &ptr, &port_tab[1], 0);
      if (value == 2)
	 value = dfsdrop(proc_anchor, &ptr);

/* create and send count entity  after each thousand
*/
      if (thousand > 1000)
	{
          value = dfscrep(proc_anchor, &count_ptr, 10, "COUNT");
          dptr = (Data *) ptr;
          ltoa(count,dptr->string,10);
          value = dfssend(proc_anchor, &count_ptr, &port_tab[2], 0);
          if (value != 0)
            {
              value = dfsdrop(proc_anchor, &count_ptr);
            }
	  thousand = 0;
	}

      value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
    }

/* create and send count entity  at end
*/
  value = dfscrep(proc_anchor, &count_ptr, 10, "COUNT");
  dptr = (Data *) ptr;
  ltoa(count,dptr->string,10);
  value = dfssend(proc_anchor, &count_ptr, &port_tab[2], 0);
  if (value != 0)
    {
      value = dfsdrop(proc_anchor, &count_ptr);
    }
  return(0);
}










