/*  Private THREADS Control Blocks */

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

// Define 1-parameter macro for tracing

#define MSG1(text,par1)  {printf(text, par1); \
                         }
// Define 2-parameter macro for tracing

#define MSG2(text,par1,par2) {printf(text, par1, par2); \
		              }

// Define 3-parameter macro for tracing

#define MSG3(text,par1,par2,par3) { printf(text, par1, par2, par3); \
				   }



// In what follows, IP means Information Packet (sorry!)


struct _port_ent              // block representing port in process's
                              //   stack storage - this relates the
			      //   name used by the component code
			      //   to the actual port control
			      //   block (cp)  - set by THZDFPT
                              //------------------------------------
   {
      char port_name[32];     // port name  (from THZDFPT's parameters)
                              // following fields are set by THZDFPT
      void *cpptr;            // - pointer to actual port control block
      int elem_count;         // - no. of elements in port control
                              //     block: >1 implies array port
      int ret_code;           // - return code: 2 indicates port closed 
   };

typedef struct _port_ent port_ent;

struct _appl {                // control block representing application
                              //----------------------------------------
   char name[32];             // name of application
   struct _process *first_ready_proc;  // ptr to first ready process
   struct _process *first_child_proc;  // ptr to first child process
   struct _process *first_child_comp;  // ptr to first child component
   jmp_buf state;                 // state of scheduler - used by
				  //    longjmp from components to
                                  //    scheduler
   jmp_buf alloc_state;           // used to manage alloc requests
				  //    from processes
   jmp_buf free_state;            // used to manage free requests
				  //    from processes

   struct _IPh HUGE * alloc_ptr;  // ptr to allocated IP
   long alloc_lth;                // length of requested IP
   int dynam;                     // DLLs being dynamically loaded (interpretive mode)
   unsigned int ds;               // save area for data segment value
   } ;

typedef struct _appl appl;

struct _anchor {              // anchor - used to communicate between
			      //    scheduler and components - it is
			      //    passed to the components by the
			      //    scheduler, and they must pass it to
			      //    the THREADS services
                              //--------------------------------------
   int (FAR  * svc_addr) (int, void * vptr, ...);   // - ptr to service
                              // interface subroutine - THZ
   struct _process *proc_ptr; //    - pointer to process control block
    } ;

typedef struct _anchor anchor;

struct _process {             // process control block
                              //--------------------------------------
   char procname[32];         // process name
   char compname[10];         // component name
   struct _process *next_proc;   // ptr to next process in chain
                              //       of ready processes (dynamic)
   jmp_buf state;             // state of component - used by longjmp
                              //    from scheduler back to component 
   appl *appl_ptr;            // ptr to application control block
   struct _process *next_sibling_proc;  // ptr to next sibling process
			      //   within subnet (static)
   struct _process *mother_proc;  // ptr to 'mother' process - process
			      // which is 'expanded' to subnet - it
			      // does not participate in process
                              // scheduling
   struct _cp *in_cps;        // ptr to first input port control block
   struct _cp *out_cps;       // ptr to first output port control block
   struct _IPh HUGE *first_owned_IP;   // ptr to first IP owned by this
                              //     process 
   struct _cp *begin_cp;      // ptr to 'beginning' port - if specified,
			      //   process is delayed until IP arrives
			      //   at this port
   struct _cp *end_cp;        // ptr to 'ending' port - if specified,
			      //   process sends a signal to this port
			      //   when it finally terminates  
   struct _port_ent int_pe;   // 'port_ent' block internal to process
			      //    control block  - allows 'ending' port
                              //    logic to use THZSEND
   void *int_ptr;             // holds ptr to IP created by 'ending'
                              //    port logic 
   int value;                 // holds return code from 'ending' port
                              //    call to THZSEND
   int in_cps_status;         // holds result of last call to
			      //    check_input_cps for this process
			      //  possible values are:
			      //      0 if data in connection
			      //      1 if upstream not closed
                              //      2 if upstream closed 
   int (FAR PASCAL *faddr)(_anchor anch);   // address of code to be
                              //     executed by this process
   far char *wptr;            //  address of storage to be used as stack
                              //     for this process
   struct _anchor proc_anchor;  // anchor to be passed to service calls
   struct _IPh HUGE *stack;   //  ptr to first IP in process stack -
			      //     managed by THZPUSH and THZPOP
   char status;               //  status of process execution:
                              //     values defined in #defines below
   unsigned trace;            //  trace required for process
   unsigned terminating;      //  process is terminating
   unsigned must_run;         //  process must run at least once
   unsigned has_run;          //  process has run at least once
   unsigned composite;        //  process is 'mother' of a subnet
   jmp_buf alloc_state;       //  used to return to process after
			      //    alloc request satisfied
   jmp_buf free_state;        //  used to return to process after
			      //    free request satisfied
   unsigned int ds;           // save area for data segment value


  } ;

  /* following #defines are possible values of 'status' above */

   #define NOT_INITIATED         ' '    // not initiated
   #define ACTIVE                'A'    // active
   #define DORMANT               'D'    // waiting to be triggered
   #define SUSPENDED_ON_SEND     'P'    // suspended on send 
   #define SUSPENDED_ON_RECV     'G'    // suspended on receive
   #define READY_TO_BE_INITIATED 'I'    // ready to be initiated
   #define READY_TO_RESUME       'R'    // ready to resume
   #define TERMINATED            'T'    // terminated

typedef struct _process process;

struct _cnxt {                // control block for a connection
                              //--------------------------------------  
   char name[32];             // name
   process *fed_proc;         // ptr to process 'fed' by this connection
                              //  (can only be one) 
   struct _process *procs_wtg_to_send;  // ptr to first of list of
			      //    processes waiting to send to this
                              //    connection
   struct _IPh HUGE *first_IPptr;  // ptr to first IP in connection 
   struct _IPh HUGE *last_IPptr;   // ptr to last IP in connection
   long max_IPcount;          // maximum count of IPs allowed = capacity
   long IPcount;              // actual count of IPs in connection
   int  total_upstream_cpelem_count;   // total no. of upstream port
                              //      elements            
   int  nonterm_upstream_cpelem_count; // no. of upstream port elements
			      //    which have not terminated - 0 means
                              //    the connection is closed
   unsigned fedproc_wtg_to_recv;  // the 'fed' process is waiting to
                              //    receive data - TRUE or FALSE
    } ;

typedef struct _cnxt cnxt;

struct _IPh {                 //  Information Packet header
                              //-------------------------------------
   char FAR *type;            //  ptr to string specifying IP 'type'
   struct _IPh HUGE *next_IP; //  ptr to next IP in connection or stack
   struct _IPh HUGE *prev_IP; //  ptr to previous IP in connection or
                              //      stack 
   void FAR *owner;           //  ptr to 'owner' - may be process or
                              //      connection
   int reserved;              //  padding
   unsigned on_stack;         //  IP is on stack - TRUE or FALSE
   long IP_size;              //  size of IP - excluding header
   } ;

typedef struct _IPh IPh;

struct _IP {                  //  Information Packet
			      //--------------------------------------
   struct _IPh IP_header;     //    header, followed by 0 - 32767 
   char unsigned datapart[32767];  //   bytes
   } ;

typedef struct _IP IP;


#define guard_value 219  /* solid rectangle */

struct _cp_elem {     // port control block element -
		      //   port is represented by port control block,
		      //   followed by an array of port control block
		      //   elements
                      //-----------------------------------------------
   union cp_union {struct _cnxt FAR *connxn; struct _IIP FAR *IIPptr;}
	     gen;     //  this field may point at a connection or
		      //    an Initial IP (IIP)
		      //  (an IIP is like a parameter - it is specified
		      //    in the network, and then turned into an
		      //    actual IP at run-time by THZRECV) 
   unsigned closed;   //  port element is closed
   unsigned is_IIP;   //  port element points at an IIP
   unsigned subdef;   //  port elem is defined in a subnet definition
   } ;

typedef struct _cp_elem cp_elem;

struct _cp {          //  port control block (see above)
                      //-----------------------------------------
   char port_name[32];   // port name
   struct _cp *next_cp;  // next port in input port chain or output
                         //  port chain - these are separate chains
   int elem_count;       //  count of elements in port 
   unsigned direction;   //  input/output port:
			 //   0 = input
			 //   1 = output 

   struct _cp_elem elem_list[1]; // array of port control block elements

   } ;

typedef struct _cp cp;





