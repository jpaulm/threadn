

  #include <setjmp.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "compsvcs.h"
  #include <io.h>
#include <dos.h>
#include <fcntl.h>


  THRCOMP thfilebn(anchor proc_anchor)
{
  void *ptr;
  char fname[256];
  char *sp;
  char *ssp;
  int value;
  long size;
  char *type;

  unsigned long len;
  port_ent port_tab[2];
  int fh;
  int bytes, count;
  char buffer[256];


  value = dfsdfpt(proc_anchor, 2, port_tab,"OPT","OUT");
  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  memcpy(fname,ptr,size);
  fname[size] = '\0';

   if (-1 == (fh = _open(fname,
	  O_RDWR | O_BINARY)))  {
          sprintf(buffer, "Cannot open file %s", fname);
          dfsputs(proc_anchor,buffer);
          return (8);
	  }
  value = dfsdrop(proc_anchor, &ptr);
  ssp =(char *) malloc(32767);

  while (1) {

    bytes = _read(fh, ssp, 32767);
    if (bytes == 0)
      break;
    sp = ssp;
    while (bytes > 0) {
      if (bytes > 30) 
          count = 30;
        else
          count = bytes;
      value = dfscrep(proc_anchor, &ptr, count, "A");
      memcpy (ptr, sp, count);
      value = dfssend(proc_anchor, &ptr, &port_tab[1], 0);
      bytes -= count;
      sp+=count;
      }
    }

  _close(fh);
  free(sp);
  return(0);
}

