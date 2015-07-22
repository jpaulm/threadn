
/* THCOLL8 is a stream collator.  It accepts a KEY entity from the KEY
   input port and merges input data entities from two INPUT ports based
   on their values in the KEY field.  The two input streams should be
   in collate sequence themselves, or the output stream will not be in
   sequence either.  The length and start position of the key
   comparison can be controlled by a key parameter.

   CALLING THCOLL8:
   {'KEYLEN{, KEY_START_POS}' -> KEYPARM stream_collator,}
   input data stream 1 -> IN[0] stream_collator OUT -> output data stream,
   input data stream 2 -> IN[1] stream_collator(THCOLL8);

   LIMITATIONS:
    key start position must be at the same spot in both input streams

   DEFAULTS:
    KEYLEN defaults to 256, which will in effect compare the whole string
	   until NULL terminated, since most strings will be less than 256.
    KEY_START_POS defaults to 0.  This begins the compare at the
	   beginning of the string.

*/
  #include <setjmp.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "compsvcs.h"

  
  THRCOMP thcoll8(anchor proc_anchor)
  {
  void *input_1_ptr, *input_2_ptr, *parm_ptr;
  int value,key_length,key_start_pos,any_more,no_more_1,no_more_2;
  long size;
  char *type,*KEY_LENGTH,*KEY_START_POS;
  port_ent port_tab[3];
  value = dfsdfpt(proc_anchor, 3, port_tab, "IN", "KEYPARM", "OUT");

  /* read in the key parameters which are of the form
     KEY_LENGTH, KEY_START_POSITION
  */
  key_length = 256;
  key_start_pos = 0;
  value = dfsrecv(proc_anchor, &parm_ptr, &port_tab[1], 0, &size, &type);
  if (value == 0)
    {
      KEY_LENGTH = strtok((char *)parm_ptr,",/.;: ");
      if (KEY_LENGTH != 0)
	{
	  key_length = atoi(KEY_LENGTH);
	  KEY_START_POS = strtok(0,",/.;: ");
	  if (KEY_START_POS != 0)
	    {
	      key_start_pos = atoi(KEY_START_POS);
	    }
	}
      value = dfsdrop(proc_anchor, &parm_ptr);
    }



/* This is the main processing loop.  Each data entity is received, then its
     key value is compared to the key of the entity on the other port.
     Whichever entity has the higher key value is passed out first and
     that port is read for a new entity.
*/
  any_more = 1;

  no_more_1 = dfsrecv(proc_anchor,
			       &input_1_ptr, &port_tab[0], 0, &size, &type);
  no_more_2 = dfsrecv(proc_anchor,
			       &input_2_ptr, &port_tab[0], 1, &size, &type);
  if ( no_more_1 && no_more_2 )
    {
      any_more = 0;
    }


  while (any_more)
    {
      if (no_more_1)
	{
	  value = dfssend(proc_anchor, &input_2_ptr, &port_tab[2], 0);
	  if (value != 0)
	    {
	      value = dfsdrop(proc_anchor, &input_2_ptr);
	    }
          no_more_2 = dfsrecv(proc_anchor,
			       &input_2_ptr, &port_tab[0], 1, &size, &type);
          if ( no_more_1 && no_more_2 )
            {
	      any_more = 0;
	    }
	}    /* end of handling for no entity on port 1 */
      else
	{
          if (no_more_2)
	    {
	      value = dfssend(proc_anchor, &input_1_ptr, &port_tab[2], 0);
	      if (value != 0)
	        {
	          value = dfsdrop(proc_anchor, &input_1_ptr);
	        }
              no_more_1 = dfsrecv(proc_anchor,
			       &input_1_ptr, &port_tab[0], 0, &size, &type);
              if ( no_more_1 && no_more_2 )
                {
	          any_more = 0;
	        }
	    }  /* end of handling for no entity on port 2 */

          else  /* there is an entity on both ports */
	    {
	      value = strncmp((char *)input_1_ptr + key_start_pos,
                              (char *)input_2_ptr + key_start_pos,
			      key_length);
	      if (value > 0)
	        {
		  value = dfssend(proc_anchor, &input_2_ptr, &port_tab[2], 0);
		  if (value != 0)
		    {
		      value = dfsdrop(proc_anchor, &input_2_ptr);
		    }
	          no_more_2 = dfsrecv(proc_anchor,
			       &input_2_ptr, &port_tab[0], 1, &size, &type);
	          if ( no_more_1 && no_more_2 )
        	    {
		      any_more = 0;
		    }
	        }
	      else
	        {
		  value = dfssend(proc_anchor, &input_1_ptr, &port_tab[2], 0);
		  if (value != 0)
		    {
		      value = dfsdrop(proc_anchor, &input_1_ptr);
		    }
	          no_more_1 = dfsrecv(proc_anchor,
			       &input_1_ptr, &port_tab[0], 0, &size, &type);
	          if ( no_more_1 && no_more_2 )
        	    {
		      any_more = 0;
		    }
	        }
	    }  /* end of handling for entity on both ports */
	}  /* end of else clause on no entity on port 1 */
    }   /* end of while any_more */

  return(0);
}