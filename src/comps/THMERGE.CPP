/* THMERGE takes streams from its input port array IN[n] and puts them out
   on its output port OUT. All entities from IN[0] are sent to OUT, then
   all entities from IN[1] etc.

   CALLING THMERGE:
   input data stream 1 -> IN[0] combine,
   [input data stream 2 -> IN[1] combine,]
   .
   .
   .
   [input data stream n -> IN[n-1] combine,]
   combine(THMERGE) OUT -> output data stream;

   LIMITATIONS:
   none

   DEFAULTS:
   none

*/

  #include <setjmp.h>
  #include <stdio.h>
  #include <string.h>
  #include "compsvcs.h"


  THRCOMP thmerge(anchor proc_anchor)
{
  void *ptr;
  int value, i;
  long size;
  char *type;
  port_ent port_tab[2];

  value = dfsdfpt(proc_anchor, 2, port_tab,"IN","OUT");


  for (i = 0; i < port_tab[0].elem_count - 1; i++) {
     value = dfsrecv(proc_anchor, &ptr, &port_tab[0], i, &size, &type);
     while (value == 0) {
         value = dfssend(proc_anchor, &ptr, &port_tab[1], 0);
         value = dfsrecv(proc_anchor, &ptr, &port_tab[0], i, &size, &type);
         }
     }
  return(0);
}

