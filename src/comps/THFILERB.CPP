

  #include <setjmp.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "compsvcs.h"


  THRCOMP thfilerb(anchor proc_anchor)
{
  void *ptr;
  char *p;
  char fname[256];
  char *ssp;
  int value, m;
  long i, j, k, size;
  char *type;
  unsigned long len;
  port_ent port_tab[2];
  FILE *fp;
  int ch;
  char buffer[256];

  value = dfsdfpt(proc_anchor, 2, port_tab,"OPT","OUT");
  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  memcpy(fname,ptr,size);
  fname[size] = '\0';
   if ((fp = fopen(fname,"rb")) == NULL) {
	 sprintf(buffer, "Cannot open file %s", fname);
         dfsputs(proc_anchor,buffer);
	 return(8);
     }

  value = dfsdrop(proc_anchor, &ptr);

  ssp = (char *) malloc(64000);

  while(1) {
     for (i = 0; i < 64000; i++) {
       ch = fgetc(fp);
       if (feof(fp)) break;
       *(ssp + i) = (unsigned) ch;
       }

     value = dfscrep(proc_anchor, &ptr, i, "A");
     memcpy (ptr, ssp, i);
     value = dfssend(proc_anchor, &ptr, &port_tab[1], 0);

     if (i < 64000) break;
     }
   fclose(fp);
  free(ssp);
  return(0);
}

