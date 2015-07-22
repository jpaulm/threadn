
/* THSOCKWT writes incoming entities to socket 4444 and
   optionally passes the entities on to an output stream.

   CALLING THSOCKWT:
   "filename.ext" -> OPT write_ents(THSOCKWT),
   input data stream -> IN write_ents [ OUT -> output data stream ];

   LIMITATIONS:
   Any special characters which cause problems for C's PUTC will not
   be written properly. Maybe?

   DEFAULTS:
   None

*/

  #include <setjmp.h>
  #include <stdio.h>
  #include <string.h>
  #include "compsvcs.h"
  #include <sys/types.h> /* See NOTES */
  #include <sys/socket.h>


  THRCOMP thsockwt(anchor proc_anchor)
{
  void *ptr;
  char *dptr;
  int value, i;
  long size;
  char *type;
  char string[256];
  int ch;
  port_ent port_tab[3];
  FILE *fp;
  char buffer[256];

  /* We'll leave OPT there for now */

  value = dfsdfpt(proc_anchor, 3, port_tab,"OPT","IN","OUT");

/*obtain filename parameter
*/
  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  memcpy(string,ptr,size);
  value = dfsdrop(proc_anchor, &ptr);
  string[size] = '\0';
  /*
   if ((fp = fopen(string,"wt")) == NULL) {
         sprintf(buffer, "Cannot open file %s", string);
         dfsputs(proc_anchor,buffer);
	 return(8);
     }
   */

   i = socket(AF_INET, SOCK_STREAM, 0);

    value = dfsrecv(proc_anchor, &ptr, &port_tab[1], 0, &size, &type);
    while (value == 0)
      {
      dptr = (char *) ptr;
      for (i = 0; i < size; i++) {
         ch = (int) * (dptr + i);
         value = fputc(ch, fp);
        }
      value = fputc('\n',fp);

      value = dfssend(proc_anchor, &ptr, &port_tab[2], 0);
      if (value == 2)
	 value = dfsdrop(proc_anchor, &ptr);
      value = dfsrecv(proc_anchor, &ptr, &port_tab[1], 0, &size, &type);
      }
  fclose(fp);
  return(0);
}

