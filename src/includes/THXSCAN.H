  

#if __OS2__ == 1 

  #define HUGE
  #define FAR
  #define PASCAL
 
#else
   #define HUGE huge
   #ifndef FAR
     #define FAR far
   #endif
   #ifndef PASCAL
     #define PASCAL pascal
   #endif
#endif


#if __OS2__ == 1
    #define THRCOMP extern "C" int
#elif defined _Windows
    #define THRCOMP  extern "C" int FAR PASCAL _export
#else
    #define THRCOMP  extern "C" int FAR PASCAL
#endif


   struct _cnxt_ent {
     char upstream_name[32];       /* if 1st char is !,        */
     char upstream_port_name[32];     /* connxn points at IIP */
     int upstream_elem_no;
     char downstream_name[32];
     char downstream_port_name[32];
     int downstream_elem_no;
     union cnxt_union {IIP *IIPptr; void *connxn;} gen;
     int capacity;
     };

typedef struct _cnxt_ent cnxt_ent;

struct _label_ent {
	 char label[32];
         char file[10];
	 struct _cnxt_ent *cnxt_ptr;
	 struct _proc_ent *proc_ptr;
         char ent_type;
	 int proc_count;
	 int cnxt_count;
	 };

typedef struct _label_ent label_ent;

   struct _proc_ent {
     char proc_name[32];
     char comp_name[10];
     int (FAR PASCAL *faddr) (anchor anch);
     void *proc_block;
     int label_count;
     int trace;
     int composite;
     int must_run;
     };

typedef struct _proc_ent proc_ent;

