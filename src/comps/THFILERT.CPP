

  #include <setjmp.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "compsvcs.h"


  THRCOMP thfilert(anchor proc_anchor)
{
  void *ptr;
  char fname[256];
  char *sp;
  char *sp2;
  char *ssp;
  int value;
  long size;
  char *type;

  unsigned long len;
  port_ent port_tab[2];
  FILE *fp;
  char buffer[256];

  value = dfsdfpt(proc_anchor, 2, port_tab,"OPT","OUT");
  value = dfsrecv(proc_anchor, &ptr, &port_tab[0], 0, &size, &type);
  memcpy(fname,ptr,size);
  fname[size] = '\0';
   if ((fp = fopen(fname,"r")) == NULL) {
	 sprintf(buffer, "Cannot open file %s", fname);
         dfsputs(proc_anchor,buffer);
	 return(8);
     }

  value = dfsdrop(proc_anchor, &ptr);
  ssp =(char *) malloc(32767);
  sp = ssp;
  while (1) {
    while (1) {
      sp2 = fgets (sp, 32765, fp);
      if (sp2 == NULL)
	break;
      sp = sp2;
      len = strlen(sp);
      sp = sp + len;
      if (*(sp - 1) == '\n') {
	 --sp;
	 break;
	 }
      }
    value = dfscrep(proc_anchor, &ptr, sp - ssp, "A");
    memcpy (ptr, ssp, sp - ssp);
    value = dfssend(proc_anchor, &ptr, &port_tab[1], 0);
    if (sp2 == NULL)
   	break;

    sp = ssp;
    }
  fclose(fp);
  free(ssp);
  return(0);
}

