#if __OS2__ == 1 

  #define HUGE
  #define FAR
  #define PASCAL
#else
  #define HUGE huge
  #define FAR far
  #define PASCAL pascal

#endif


 struct  _anchor
    {

    int (FAR *svc_addr) (int, void * vptr, ...);

    void *reserved;
     } ;

 typedef struct _anchor anchor;

