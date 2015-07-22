/* THVIEW allows the user to view entities as they pass through the network.
   It will substitute asterisks for any characters which cannot be displayed
   on the screen, unless the option 'C' is provided at the OPT input port.
   There is an option to display the values in hexadecimal as well as the
   (translated) characters.  This is accomplished by passing an IP
   beginning with 'H' into the OPT port.  The option 'h' will display hex
   only.

  CALLING THVIEW:
   input data stream -> IN view_data OUT -> output data stream,
   ["H" -> OPT] view_data(THVIEW);

  LIMITATIONS:
   None

  DEFAULTS:
   non-HEX (ie. character) display only
*/

  #include <setjmp.h>
  #include <stdio.h>
  /* #include <iostream.h> */
  #include <string.h>
  #include "compsvcs.h"

  THRCOMP thview(anchor proc_anchor)
{
  void *ptr;
  char *dptr;
  int value;
  int j, k1, k2;
  long size, i;
  char *type;
  port_ent port_tab[3];
  char array[256];
  char sw = 'T';
  unsigned char k;
  char *x;
  char buffer [256];
  char * cp;

  
/* set up the translation string to remove unprintable characters */

  memset(array, '*', 256);
  cp = array + 32;
  memcpy(cp, " !\"#$%&'()*+,-./", 16);
  cp = array + 48;
  memcpy(cp, "0123456789", 10);
  cp = array + 58;
  memcpy(cp, ":;<=>?@", 7);
  cp = array + 65;
  memcpy(cp, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26);
  cp = array + 91;
  memcpy(cp, "[\\]^_ ", 6);
  cp = array + 97;
  memcpy(cp, "abcdefghijklmnopqrstuvwxyz", 26);
  cp = array + 123;
  memcpy(cp, "{|}", 3);
  value = dfsdfpt(proc_anchor, 3, port_tab,"OPT","IN","OUT");

/* check for the HEX parameter
*/
  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  
  if (value == 0) {
      memcpy(&sw,ptr,1);
      
      value = dfsdrop(proc_anchor, &ptr);
  }

/* process input entities  */

   
  value = dfsrecv(proc_anchor, &ptr, &port_tab[1], 0, &size, &type);
  
  
  while (value == 0) {

      dptr = (char *) ptr;

      if (sw == 'h') {
	    sprintf(buffer,
		 "Length: %ld, Type: %s ", size,  type);
	    dfsputs(proc_anchor, buffer);
            }
        else {
	    sprintf(buffer,
	         "Length: %ld, Type: %s, Data: ", size,  type);
	    dfsputs(proc_anchor, buffer);
            }
	    	    
      if (sw != 'h') {
          for (i = 0; i < size; i++)  {
             k = (unsigned char) *(dptr + i);
             if (sw == 'T'|| sw == 'H') {
               j = (int) k;
	       k = array[j];
	       }
             dfsputc(proc_anchor, k);
	     }
	  dfsputc(proc_anchor, '\n');
          }
	    

      if (sw == 'H' || sw == 'h') {
         sprintf(buffer,"Hex: ");
         dfsputs(proc_anchor, buffer);
     
         for (i = 0; i < size; i++)  {
	    k = (int) * (dptr + i);
            k1 = "0123456789ABCDEF"[k/16];
            k2 = "0123456789ABCDEF"[k%16];
	    dfsputc(proc_anchor, k1);
	    dfsputc(proc_anchor, k2);
	    }
         dfsputc(proc_anchor, '\n');
	 
         }

      value = dfssend(proc_anchor, &ptr, &port_tab[2], 0);
      
      if (value == 2)
	 value = dfsdrop(proc_anchor, &ptr);

      value = dfsrecv(proc_anchor, &ptr, &port_tab[1], 0, &size, &type);
      
    }

  return(0);
}