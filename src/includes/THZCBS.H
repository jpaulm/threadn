
   #include "thxanch.h"

   struct _port_ent
   {
      char port_name[32];
      void *reserved;
      int elem_count;
      int ret_code;
   };

   typedef struct _port_ent port_ent;
