
/* THSPLITT is a table based splitter.  It accepts KEY entities from one
   input port and loads them into a table.  It then accepts data entities
   from another port, scans the table for a match, and if matched sends the
   input data entities out the MATCH port.  Otherwise, the entities are
   sent out the NOMATCH port.  The length and start position of the key
   comparison can be controlled by a key parameter.

   CALLING THSPLITT:
   ['KEYLEN[, DATA_START_POS[, KEY_START_POS]]' -> KEYPARM split_proc,]
   key data stream   -> KEYS split_proc [MATCH -> matched data stream],
   input data stream -> IN   split_proc [NOMATCH -> unmatched data stream],
   split_proc(THSPLITT);

   LIMITATIONS:
    Currently limited to 100 key values, this can be changed by changing the
    size of the array *key_ptr.  The next limit on key values is 32k since
    integer indexes are used.

   DEFAULTS:
    KEYLEN defaults to 256, which will in effect compare the whole string
	   until NULL terminated, since most strings will be less than 256.
    DATA_START_POS defaults to 0.  This begins the compare at the
	   beginning of the string.
    KEY_START_POS  defaults to 0.

*/
  #include <setjmp.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "compsvcs.h"

  
  THRCOMP thsplitt(anchor proc_anchor)
  {
  void *input_ptr,*parm_ptr;
  void *key_ptr[100];
  int value,key_length,data_start_pos,key_start_pos,out_port;
  int keylimit,key_count;
  long size;
  char *type,*KEY_LENGTH,*DATA_START_POS,*KEY_START_POS;
  port_ent port_tab[5];
  value = dfsdfpt(proc_anchor, 5, port_tab,
                  "IN","KEYS","MATCH","NOMATCH","KEYPARM");

  /* read in the key parameters which are of the form
     KEY_LENGTH, DATA_START_POSITION, KEY_START_POSITION
  */
  key_length = 256;
  data_start_pos = 0;
  key_start_pos = 0;
  value = dfsrecv(proc_anchor, &parm_ptr, &port_tab[4], 0, &size, &type);
  if (value == 0)
    {
      KEY_LENGTH = strtok((char *)parm_ptr,",/.;: ");
      if (KEY_LENGTH != 0)
	{
	  key_length = atoi(KEY_LENGTH);
	  DATA_START_POS = strtok(0,",/.;: ");
	  if (DATA_START_POS != 0)
	    {
	      data_start_pos = atoi(DATA_START_POS);
	      KEY_START_POS = strtok(0,",/.;: ");
	      if (KEY_START_POS != 0)
		key_start_pos = atoi(KEY_START_POS);
	    }
	}
      value = dfsdrop(proc_anchor, &parm_ptr);
    }



  /* read in all key entities and initialize a pointer table with the
     pointers to the entities.  This pointer table will be used to reference
     the keys later in the compare section
  */
  keylimit = 0;
  key_count = 0;
  value = dfsrecv(proc_anchor, &key_ptr[key_count],
		  &port_tab[1], 0, &size, &type);
  while (value == 0)
    {
      keylimit = keylimit + 1;
      key_count = key_count + 1;
      value = dfsrecv(proc_anchor, &key_ptr[key_count],
		      &port_tab[1], 0, &size, &type);
    }

  /* This is the main processing loop.  Each data entity is received, then its
     key value is compared to the table of key values.  If it equals any of
     the keys in the table, the entity is passed out the MATCH port.
     Otherwise, it is passed out the NOMATCH port.
  */
  while (dfsrecv(proc_anchor, &input_ptr, &port_tab[0], 0, &size, &type) ==0)
    {
      out_port = 3; /*NOMATCH*/
      for (key_count = 0; key_count < keylimit; key_count++)
	{
	  value = strncmp((char *)input_ptr + data_start_pos,
                          (char *)key_ptr[key_count] + key_start_pos,
			  key_length);
	  if (value == 0)
	    {
	      out_port = 2;  /*MATCH*/
	      key_count = keylimit;
	    }
	}
      value = dfssend(proc_anchor, &input_ptr, &port_tab[out_port], 0);
      if (value == 2)
	 value = dfsdrop(proc_anchor, &input_ptr);
    }

/* After processing all input data entities, discard the key entities to
   clean up storage, and avoid the warning message.
*/
    for (key_count = 0; key_count < keylimit; key_count++)
      {
	value = dfsdrop(proc_anchor, &key_ptr[key_count]);
      }
  return(0);
}