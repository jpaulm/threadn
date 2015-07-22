

/* THASSIGN assigns a value passed in on the VALUE port to each entity
   recieved on the IN port. CHAR and INT formats are supported.  If CHAR is
   specified, the length of the string must be specified as well.  For INT,
   length will be 4 bytes.  START_POS can be specified.  If there are any
   parameter problems, the input entities will be passed on unmodified.

   CALLING THASSIGN:
   'VALUE, FORMAT{, START_POS{, LENGTH}}' -> VALUE add_value,
   input data stream -> IN add_value(THASSIGN) OUT -> output data stream;

   LIMITATIONS:
   INTegers cannot exceed 64M
   CHARacter strings cannot be longer than 256 Bytes

   DEFAULTS:
   KEYVALUE   none
   FORMAT     none
   START_POS  defaults to 0.  This assigns the value at the beginning
	   of the string.
   LENGTH defaults to 4, which be the right length for an INT.

*/

  #include <setjmp.h>
  #include <stdlib.h>
  #include <string.h>
  #include "compsvcs.h"


  THRCOMP THASSIGN(anchor proc_anchor)
{
  void *ptr;
  void *optr;
  char *start_ptr, *length_ptr;
  int  value, i, length, start_pos, PARMOK;
  long size, value_int;
  char *type;
  port_ent port_tab[3];
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
  optstr *value_ptr, *format_ptr;

  value = dfsdfpt(proc_anchor, 3, port_tab, "VALUE", "IN", "OUT");

/* obtain input parameters VALUE, FORMAT, STARTPOS, LENGTH
*/
  PARMOK = 1;
  length = 4;
  start_pos = 0;
  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  if (value == 0)
    {
      value_ptr = (optstr *) strtok((char *)ptr, ",/. ;:");
      if (value_ptr != 0)
	{
	  format_ptr = (optstr *) strtok(0, ",/. ;:");
	  if (format_ptr != 0)
	    {
	      if (!strcmp((char *)format_ptr, "INT"))
                {
		  value_int = atol((char *) value_ptr);
		  value_ptr =(optstr *) &value_int;
	        }
	      start_ptr = strtok(0, ",/. ;:");
	      if (start_ptr != 0)
	        {
	          start_pos = atoi(start_ptr);
		  if (!strcmp((char *)format_ptr, "CHAR"))
		  {
		    length_ptr =  strtok(0, ",/. ;:");
	            if (length_ptr != 0)
		      length = atoi(length_ptr);
		  }
		}
	    }
	  else PARMOK = 0;
	}
      else PARMOK = 0;

      optr = ptr;
    }

/* process input entities
*/
  value = dfsrecv(proc_anchor, &ptr, &port_tab[1], 0, &size, &type);
  while (value == 0)
    {
      inptr = (instr *) ptr;
      if (PARMOK)
      {
        for (i = 0; i < length; i++)
	  {
	    inptr -> string[i + start_pos] = value_ptr -> string[i];
          }
      }
      value = dfssend(proc_anchor, &ptr, &port_tab[2], 0);
      if (value != 0)
	  value = dfsdrop(proc_anchor, &ptr);
      value = dfsrecv(proc_anchor, &ptr, &port_tab[1], 0, &size, &type);
    }
  value = dfsdrop(proc_anchor, &optr);
  return(0);
}

