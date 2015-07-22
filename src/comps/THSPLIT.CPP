
/* THSPLIT is a parameter based splitter.  It accepts a KEY value on its OPT
   port and splits the input stream into two output streams based on whether
   the input entities contain the KEY value at STARTPOS for a length of LENGTH
   Matching entities are sent out MATCH, and others out NOMATCH.  The length
   and starting position of the KEY value in the input entities can be
   controlled by parameters.

   CALLING THSPLIT:
   "KEY_VALUE[, LENGTH[, START_POS]]" -> OPT splitter,
   input data stream -> IN splitter[ MATCH -> matched data stream],
   splitter(THSPLIT) [ NOMATCH -> mismatched data stream];

   LIMITATIONS:
   None

   DEFAULTS:
   KEYVALUE   defaults to NULL, which will not likely have matches, so the
	   data will be piped to NOMATCH.
   Key LENGTH defaults to 265, which will in effect compare the whole string
	   until NULL terminated, since KEYVAL will be less than 256 bytes.
   START_POS  defaults to 0.  This begins the compare at the beginning
	   of the string.

*/

  #include <setjmp.h>
  #include <stdlib.h>
  #include <string.h>
  #include "compsvcs.h"


  THRCOMP thsplit(anchor proc_anchor)
{
  void *ptr;
  void *optr;
  char *sptr, *lptr;
  int value, i, reject, length, start_pos;
  long size;
  char *type;
  port_ent port_tab[4];
  struct _instr
  {
     char string[1];
  };
  typedef struct _instr instr;

  struct _optstr
  {
     char string[1];
  };
  typedef struct _optstr optstr;

  instr *inptr;
  optstr *cptr;

  value = dfsdfpt(proc_anchor, 4, port_tab,
                 "OPT","IN","MATCH","NOMATCH");

/* obtain input parameters KEYVAL, LENGTH, STARTPOS
*/
  length = 256;
  start_pos = 0;
  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  if (value == 0)
    {
      cptr = (optstr *) strtok((char *)ptr, ",/.;:");
      if (cptr != 0)
	{
	  lptr = strtok(0, ",/.;:");
	  if (lptr != 0)
	    {
	      length = atoi(lptr);
	      sptr =  strtok(0, ",/.;:");
	      if (sptr != 0)
		start_pos = atoi(sptr);
	    }
	}
      optr = ptr;
    }

/* process input entities
*/
  value = dfsrecv(proc_anchor, &ptr, &port_tab[1], 0, &size, &type);
  while (value == 0)
    {
      reject = 0;
      inptr =(instr *) ptr;
      for (i = 0; i < length; i++) {
	 if (inptr -> string[i + start_pos] != cptr -> string[i]) {
	    reject = 1;
	    break;
	    }
      	 }
      if (reject)
          value = dfssend(proc_anchor, &ptr, &port_tab[3], 0);
	else
	  value = dfssend(proc_anchor, &ptr, &port_tab[2], 0);
      if (value != 0)
	  value = dfsdrop(proc_anchor, &ptr);
      value = dfsrecv(proc_anchor, &ptr, &port_tab[1], 0, &size, &type);
      }
  value = dfsdrop(proc_anchor, &optr);
  return(0);
}

