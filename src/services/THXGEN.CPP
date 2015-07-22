   #include <stdarg.h>
   #include <stdio.h>
   #include <io.h>
   #include <ctype.h>
   #include <setjmp.h>
   #include <stdlib.h>
   #include <malloc.h>
   #include <string.h>
   #include "threads.h"
   #include "thxiip.h"
   #include "thxscan.h"
   typedef enum {FALSE, TRUE} Boolean;

   int thxscan(FILE *fp, cnxt_ent *cnxt_tab, proc_ent *proc_tab,
                   label_ent *label_tab, char file_name[10],
                   int * label_cp, int * proc_cp, int * cnxt_cp);

   main(int argc, char *argv[])
   
    {

   cnxt_ent *cnxt_tab, *cnxt_ptr;
   proc_ent *proc_tab, *proc_ptr;
   int saved_cnxt_count;
   label_ent *label_tab, *label_ptr;
   char gen_area [200];
   char gen_comp_area[200];
   char * gen_ptr, *gen_comp_ptr;
   FILE *fp, *fpo, *fpol;
   IIP *IIPptr;
   int i;
   int label_ct = 0;
   int label_count = 0;
   int proc_ct = 0;
   int proc_count = 0;
   int cnxt_count = 0;
   char fname[256];
   char file_name[10];

   if (argc != 2) {
	 printf("You forgot to enter the file name\n");
	 return(8);
	 }
   strcpy(fname,argv[1]);
   strcat(fname,".net");

   if ((fp = fopen(fname,"r")) == NULL) {
	 printf("Cannot open Input Network\n");
	 return(8);
       }
   strcpy(fname,argv[1]);
   strcat(fname,".cpp");
   if ((fpo = fopen(fname,"w")) == NULL) {
	 printf("Cannot open Output Network Program\n");
	 return(8);
       }
   strcpy(fname,argv[1]);
   strcat(fname,".lnk");
   if ((fpol = fopen(fname,"w")) == NULL) {
	 printf("Cannot open Network Components List\n");
	 return(8);
       }

   cnxt_tab = (cnxt_ent *)calloc(200,sizeof(cnxt_ent));
   proc_tab = (proc_ent *)calloc(100,sizeof(proc_ent));
   label_tab = (label_ent *)calloc(100,sizeof(label_ent));

   strcpy(file_name,"\0");

   if (thxscan(fp, cnxt_tab, proc_tab, label_tab, file_name,
       &label_count, &proc_count, &cnxt_count) != 0) {
      printf("Scan error\n");
      return(8);
      }
   gen_ptr = gen_area;
   strcpy(gen_ptr, "#include \"thxiip.h\"\n");
   fputs(gen_area,fpo);

   gen_ptr = gen_area;
   strcpy(gen_ptr, "#include \"thxanch.h\"\n");
   fputs(gen_area,fpo);


   gen_ptr = gen_area;
   strcpy(gen_ptr, "#include \"thxscan.h\"\n");
   fputs(gen_area,fpo);


   gen_ptr = gen_area;
   strcpy(gen_ptr, "#include <stdio.h>\n");
   fputs(gen_area,fpo);
                               
   gen_ptr = gen_area;
   strcpy(gen_ptr,
   "void FAR threads(label_ent * label_tab, int dynam,  FILE * fp, int timereq);\n");
   fputs(gen_area,fpo);



   for (label_ct = 0; label_ct < label_count; label_ct++) {
        label_ptr = &label_tab[label_ct];
        if (label_ptr -> ent_type == 'L') {
        proc_ptr = label_ptr -> proc_ptr;
       while (proc_ptr -> proc_name[0] != '\0') {
         if (!proc_ptr -> composite) {
           gen_ptr = gen_area;
	   strcpy(gen_ptr, "THRCOMP ");
           gen_ptr = strchr(gen_ptr, '\0');
           strcpy(gen_ptr, proc_ptr -> comp_name);
           gen_ptr = strchr(gen_ptr,'\0');
	   strcpy(gen_ptr, "(anchor anch);\n");

           fputs(gen_area,fpo);
           }
         proc_ptr++;

          }
      }
   }

    proc_count = 0;
   cnxt_count = 0;
   for (label_ct = 0; label_ct < label_count; label_ct++) {
     label_ptr = &label_tab[label_ct];
     if (label_ptr -> ent_type == 'L') {
     label_ptr -> proc_count = proc_count;
     label_ptr -> cnxt_count = cnxt_count;
     proc_ptr = label_ptr -> proc_ptr;
     while (proc_ptr -> proc_name[0] != '\0') {
       gen_ptr = gen_area;

       strcpy(gen_ptr, "proc_ent P");
       gen_ptr = strchr(gen_ptr, '\0');

       itoa(proc_count,gen_ptr,10);
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, " = {\"");
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, proc_ptr -> proc_name);
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, "\\0\", \"");
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, proc_ptr -> comp_name);
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, "\\0\", ");
       gen_ptr = strchr(gen_ptr,'\0');

       if (!proc_ptr -> composite) {
       	  strcpy(gen_ptr, "(int (FAR PASCAL*)(anchor anch))");
	  gen_ptr = strchr(gen_ptr,'\0');  
          strcpy(gen_ptr, proc_ptr -> comp_name);
          gen_ptr = strchr(gen_ptr,'\0');
          strcpy(gen_ptr, ", 0, 0,");
          gen_ptr = strchr(gen_ptr,'\0');
	  /*
          gen_comp_ptr = gen_comp_area;
          strcpy(gen_comp_ptr, proc_ptr -> comp_name);
          gen_comp_ptr = strchr(gen_comp_ptr,'\0');

          strcpy(gen_comp_ptr, " ");
          fputs(gen_comp_area, fpol);
          */
          }
       else {
          strcpy(gen_ptr, " 0, 0, ");
          gen_ptr = strchr(gen_ptr,'\0');

          itoa(proc_ptr -> label_count, gen_ptr, 10);
          gen_ptr = strchr(gen_ptr,'\0');
	  strcpy(gen_ptr, ", ");
          gen_ptr = strchr(gen_ptr,'\0');
          }
       if (proc_ptr -> trace)
	   strcpy(gen_ptr, "  1, ");       /* trace */
	 else
	   strcpy(gen_ptr, "  0, ");
       gen_ptr = strchr(gen_ptr,'\0');
       itoa(proc_ptr -> composite, gen_ptr, 10);
       gen_ptr = strchr(gen_ptr,'\0');
       strcpy(gen_ptr, ", ");
       gen_ptr = strchr(gen_ptr,'\0');
       itoa(proc_ptr -> must_run, gen_ptr, 10);
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, "};\n");

       proc_ptr++;
       proc_count++;

       fputs(gen_area, fpo);

       }
    gen_ptr = gen_area;
    strcpy(gen_ptr, "proc_ent P");
    gen_ptr = strchr(gen_ptr,'\0');
    itoa(proc_count,gen_ptr,10);
    gen_ptr = strchr(gen_ptr,'\0');
    strcpy(gen_ptr, " = {\"\\0\", \"\\0\", 0, 0, 0, 0, 0, 0};\n");
    proc_count++;
    fputs(gen_area,fpo);


    cnxt_ptr = label_ptr -> cnxt_ptr;
    saved_cnxt_count = cnxt_count;
    while (cnxt_ptr -> upstream_name[0] != '\0') {
       if (cnxt_ptr -> upstream_name[0] == '!') {
          gen_ptr = gen_area;
          strcpy(gen_ptr, "IIP I");
          gen_ptr = strchr(gen_ptr, '\0');

          itoa(cnxt_count,gen_ptr,10);
          gen_ptr = strchr(gen_ptr,'\0');

          strcpy(gen_ptr, " = {");
          gen_ptr = strchr(gen_ptr,'\0');
	  IIPptr = cnxt_ptr -> gen.IIPptr;

          itoa(IIPptr -> IIP_len, gen_ptr, 10);
          gen_ptr = strchr(gen_ptr,'\0');

          strcpy(gen_ptr, ", \"");
          gen_ptr = strchr(gen_ptr,'\0');

          for (i = 0; i < IIPptr -> IIP_len; i++) {
	     if (IIPptr -> datapart[i] == '\\')
	       strcpy (gen_ptr, "\\\\");
	     else
	       if (IIPptr -> datapart[i] == '\'')
	         strcpy (gen_ptr, "\\\'");
	       else
	         if (IIPptr -> datapart[i] == '\"')
	           strcpy (gen_ptr, "\\\"");
	         else
	           if (IIPptr -> datapart[i] == '\?')
	             strcpy (gen_ptr, "\\\?");
                   else {
	             *gen_ptr = IIPptr -> datapart[i];
		     gen_ptr++;
		     *gen_ptr = '\0';
		     }
	     gen_ptr = strchr(gen_ptr,'\0');
	     }

          strcpy(gen_ptr, "\"};\n");
          fputs(gen_area,fpo);
          }

       cnxt_count++;
       cnxt_ptr++;


       }


    cnxt_ptr = label_ptr -> cnxt_ptr;
    cnxt_count = saved_cnxt_count;
    while (cnxt_ptr -> upstream_name[0] != '\0') {
       gen_ptr = gen_area;
       strcpy(gen_ptr, "cnxt_ent C");
       gen_ptr = strchr(gen_ptr, '\0');

       itoa(cnxt_count,gen_ptr,10);
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, " = {\"");
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, cnxt_ptr -> upstream_name);
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, "\\0\", \"");
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, cnxt_ptr -> upstream_port_name);
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, "\\0\", ");
       gen_ptr = strchr(gen_ptr,'\0');

       itoa(cnxt_ptr -> upstream_elem_no,gen_ptr,10);
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, ", \"");
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, cnxt_ptr -> downstream_name);
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, "\\0\", \"");
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, cnxt_ptr -> downstream_port_name);
       gen_ptr = strchr(gen_ptr,'\0');

       strcpy(gen_ptr, "\\0\", ");
       gen_ptr = strchr(gen_ptr,'\0');

       itoa(cnxt_ptr -> downstream_elem_no,gen_ptr,10);
       gen_ptr = strchr(gen_ptr,'\0');

       if (cnxt_ptr -> upstream_name[0] == '!') {
          strcpy(gen_ptr, ", &I");
	  gen_ptr = strchr(gen_ptr,'\0');
	  itoa(cnxt_count,gen_ptr,10);
          gen_ptr = strchr(gen_ptr,'\0');
          strcpy(gen_ptr, ", 0};\n");
	  }
       else {
          strcpy(gen_ptr, ", 0, ");
	  gen_ptr = strchr(gen_ptr,'\0');
          if (cnxt_ptr -> capacity == -1)
             strcpy(gen_ptr, "6");
          else
	     itoa(cnxt_ptr -> capacity,gen_ptr,10);
          gen_ptr = strchr(gen_ptr,'\0');
          strcpy(gen_ptr, "};\n");
            }

       cnxt_count++;
       cnxt_ptr++;

       fputs(gen_area,fpo);
       }

    gen_ptr = gen_area;
    strcpy(gen_ptr, "cnxt_ent C");
    gen_ptr = strchr(gen_ptr,'\0');
    itoa(cnxt_count,gen_ptr,10);
    gen_ptr = strchr(gen_ptr,'\0');
    strcpy(gen_ptr, " = {\"\\0\", \" \",0, \" \", \" \", 0, 0, 0};\n");
    fputs(gen_area,fpo);
    cnxt_count++;
    }
    }


    gen_ptr = gen_area;
    strcpy(gen_ptr, "label_ent LABELTAB = {\" \", \"\\0\", &C0, &P0, \'L\', 0, 0};\n");
    fputs(gen_area,fpo);
    for (label_ct = 1; label_ct < label_count; label_ct++) {
      label_ptr = &label_tab[label_ct];
      if (label_ptr -> label[0] == '\0') {
      gen_ptr = gen_area;
      strcpy(gen_ptr, "label_ent L");
      gen_ptr = strchr(gen_ptr,'\0');
      itoa(label_ct,gen_ptr,10);
      gen_ptr = strchr(gen_ptr,'\0');
      strcpy(gen_ptr, " = {\"\\0\", \"\\0\", 0, 0, \' \', 0, 0};\n");

      }

      else
      if (label_ptr -> ent_type == 'L') {

      gen_ptr = gen_area;
      strcpy(gen_ptr, "label_ent L");
      gen_ptr = strchr(gen_ptr,'\0');
      itoa(label_ct,gen_ptr,10);
      gen_ptr = strchr(gen_ptr,'\0');
      strcpy(gen_ptr, " = {\"");
      gen_ptr = strchr(gen_ptr,'\0');
      strcpy(gen_ptr, label_ptr -> label);
      gen_ptr = strchr(gen_ptr,'\0');
      strcpy(gen_ptr, "\\0\", \"\\0\", &C");
      gen_ptr = strchr(gen_ptr,'\0');
      itoa(label_ptr -> cnxt_count,gen_ptr,10);
      gen_ptr = strchr(gen_ptr,'\0');
      strcpy(gen_ptr, ", &P");
      gen_ptr = strchr(gen_ptr,'\0');
      itoa(label_ptr -> proc_count,gen_ptr,10);
      gen_ptr = strchr(gen_ptr,'\0');
      strcpy(gen_ptr, ", \'L\', 0, 0};\n");
      }
      else
      {
      gen_ptr = gen_area;
      strcpy(gen_ptr, "label_ent L");
      gen_ptr = strchr(gen_ptr,'\0');
      itoa(label_ct,gen_ptr,10);
      gen_ptr = strchr(gen_ptr,'\0');
      strcpy(gen_ptr, " = {\"");
      gen_ptr = strchr(gen_ptr,'\0');
      strcpy(gen_ptr, label_ptr -> label);
      gen_ptr = strchr(gen_ptr,'\0');
      strcpy(gen_ptr, "\\0\", \"\\0\", 0, 0, \'");
      gen_ptr = strchr(gen_ptr,'\0');
      *gen_ptr = label_ptr -> ent_type;
      gen_ptr++;
      strcpy(gen_ptr, "\', 0, 0 };\n");
      }

      fputs(gen_area,fpo);


   }

   gen_ptr = gen_area;
   strcpy(gen_ptr, "label_ent LX = {\"\\0\", \"\\0\", 0, 0, \' \', 0, 0};\n");
   fputs(gen_area,fpo);

   gen_ptr = gen_area;
   strcpy(gen_ptr, "int comptab = 0;\n");
   fputs(gen_area,fpo);
   gen_ptr = gen_area;
   strcpy(gen_ptr, "void main() {\n");
   fputs(gen_area,fpo);gen_ptr = gen_area;
   strcpy(gen_ptr, "threads(&LABELTAB, 0, 0, 0);\n}\n");
   fputs(gen_area,fpo);

   for (proc_ct = 0; proc_ct < proc_count; proc_ct++) {
       if (proc_tab[proc_ct].comp_name[0] == '\0'||
	          proc_tab[proc_ct].composite)
	  continue;
       i = 0;
       while (i < proc_ct && strcmp(proc_tab[i].comp_name,
		   proc_tab[proc_ct].comp_name) != 0)
	  i++;
       if (i == proc_ct) {
          gen_comp_ptr = gen_comp_area;
          strcpy(gen_comp_ptr, proc_tab[proc_ct].comp_name);
          gen_comp_ptr = strchr(gen_comp_ptr,'\0');

          strcpy(gen_comp_ptr, " ");
          fputs(gen_comp_area, fpol);
       }
   }
   printf("Gen finished\n");
   free(cnxt_tab);
   free(proc_tab);
   free(label_tab);
   fclose(fp);
   fclose(fpo);
   fclose(fpol);
   return(0);
 }