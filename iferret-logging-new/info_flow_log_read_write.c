
#define SYSOP(op) glue(INFO_FLOW_OP_SYS_,op)
     
// All syscalls iferret log entries containt this info.
#define IFLS_CORE(op)     \
  IFLW_PUT_OP(SYSOP(op)); \
  IFLW_PUT_UINT8_T(is_sysenter); \
  IFLW_PUT_STRING(command); \
  IFLW_PUT_UINT32_T(pid); \
  IFLW_PUT_UINT32_T(eip_for_callsite); 

// sys call with no args
#define IFLS(op) \
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
)





// sys call with one arg -- an int
#define IFLS_I(op,val)	\
IFLW_WRAPPER ( \
  IFLS_CORE(op);  \
  IFLW_PUT_UINT32_T(val); \
) 

     // sys call with two args -- both ints
#define IFLS_II(op,val1,val2) \
IFLW_WRAPPER ( \
  IFLS_CORE(op);   \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_UINT32_T(val2); \
) 

     // sys call with three args -- all ints
#define IFLS_III(op,val1,val2,val3)  \
IFLW_WRAPPER ( \
  IFLS_CORE(op);   \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_UINT32_T(val2); \
  IFLW_PUT_UINT32_T(val3); \
) 

     // sys call with four args -- all ints
#define IFLS_IIII(op,val1,val2,val3,val4)		\
IFLW_WRAPPER ( \
  IFLS_CORE(op);   \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_UINT32_T(val2); \
  IFLW_PUT_UINT32_T(val3); \
  IFLW_PUT_UINT32_T(val4); \
) 

     // sys call with five args -- all ints
#define IFLS_IIIII(op,val1,val2,val3,val4,val5)	\
IFLW_WRAPPER ( \
  IFLS_CORE(op);   \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_UINT32_T(val2); \
  IFLW_PUT_UINT32_T(val3); \
  IFLW_PUT_UINT32_T(val4); \
  IFLW_PUT_UINT32_T(val5); \
) 

     // sys call with six args -- all ints
#define IFLS_IIIIII(op,val1,val2,val3,val4,val5,val6)	\
IFLW_WRAPPER ( \
  IFLS_CORE(op);   \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_UINT32_T(val2); \
  IFLW_PUT_UINT32_T(val3); \
  IFLW_PUT_UINT32_T(val4); \
  IFLW_PUT_UINT32_T(val5); \
  IFLW_PUT_UINT32_T(val6); \
) 

     // sys call with seven args -- all ints
#define IFLS_IIIIIII(op,val1,val2,val3,val4,val5,val6,val7)	\
IFLW_WRAPPER ( \
  IFLS_CORE(op);   \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_UINT32_T(val2); \
  IFLW_PUT_UINT32_T(val3); \
  IFLW_PUT_UINT32_T(val4); \
  IFLW_PUT_UINT32_T(val5); \
  IFLW_PUT_UINT32_T(val6); \
  IFLW_PUT_UINT32_T(val7); \
) 



     // sys call with one arg -- a string 
#define IFLS_S(op,val) \
IFLW_WRAPPER ( \
  IFLS_CORE(op);	       \
  IFLW_PUT_STRING(val);  \
)


     // sys call with two args -- both strings
#define IFLS_SS(op,val1,val2)			\
IFLW_WRAPPER ( \
  IFLS_CORE(op);	       \
  IFLW_PUT_STRING(val1);  \
  IFLW_PUT_STRING(val2);  \
)

     // sys call with two args -- a string and an int
#define IFLS_SI(op,val1,val2) \
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_STRING(val1); \
  IFLW_PUT_UINT32_T(val2); \
) 

     // sys call -- int string
#define IFLS_IS(op,val1,val2) \
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_STRING(val2); \
) 

     // sys call with three args -- a string and two ints
#define IFLS_SII(op,val1,val2,val3)	\
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_STRING(val1); \
  IFLW_PUT_UINT32_T(val2); \
  IFLW_PUT_UINT32_T(val3); \
) 


     // sys call -- int string int 
#define IFLS_ISI(op,val1,val2,val3)			\
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_STRING(val2); \
  IFLW_PUT_UINT32_T(val3); \
) 

     // sys call -- string int string
#define IFLS_SIS(op,val1,val2,val3)			\
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_STRING(val1); \
  IFLW_PUT_UINT32_T(val2); \
  IFLW_PUT_STRING(val3); \
) 

     // sys call -- string string int
#define IFLS_SSI(op,val1,val2,val3)			\
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_STRING(val1); \
  IFLW_PUT_STRING(val2); \
  IFLW_PUT_UINT32_T(val3); \
) 

     // sys call -- int string string
#define IFLS_ISS(op,val1,val2,val3)			\
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_STRING(val2); \
  IFLW_PUT_STRING(val3); \
) 


     // sys call -- int string int int int
#define IFLS_ISIII(op,val1,val2,val3,val4,val5)	\
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_STRING(val2); \
  IFLW_PUT_UINT32_T(val3); \
  IFLW_PUT_UINT32_T(val4); \
  IFLW_PUT_UINT32_T(val5); \
) 

     // sys call -- int string int int
#define IFLS_ISII(op,val1,val2,val3,val4)	\
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_STRING(val2); \
  IFLW_PUT_UINT32_T(val3); \
  IFLW_PUT_UINT32_T(val4); \
) 

     // sys call -- int string int string int
#define IFLS_ISISI(op,val1,val2,val3,val4,val5)	\
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_STRING(val2); \
  IFLW_PUT_UINT32_T(val3); \
  IFLW_PUT_STRING(val4); \
  IFLW_PUT_UINT32_T(val5); \
) 

     // sys call -- int string int string
#define IFLS_ISIS(op,val1,val2,val3,val4)	\
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_STRING(val2); \
  IFLW_PUT_UINT32_T(val3); \
  IFLW_PUT_STRING(val4); \
) 

     // sys call -- string string int int
#define IFLS_SSII(op,val1,val2,val3,val4)	\
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_STRING(val1); \
  IFLW_PUT_STRING(val2); \
  IFLW_PUT_UINT32_T(val3); \
  IFLW_PUT_UINT32_T(val4); \
)


     // sys call -- string string string int
#define IFLS_SSSI(op,val1,val2,val3,val4)	\
IFLW_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_STRING(val1); \
  IFLW_PUT_STRING(val2); \
  IFLW_PUT_STRING(val3); \
  IFLW_PUT_UINT32_T(val4); \
)



void info_flow_log_sssi_write(info_flow_op_enum_t op, char *s1, char *s2, char *s3, int i1) {
  info_flow_log_write_uint32_t(op);  
  info_flow_log_sentinel();
#ifdef USE_SENTINEL
  info_flow_log_write_uint32_t(THE_SENTINEL);
#endif
  info_flow_log_write_string(s1);
  info_flow_log_write_string(s2);
  info_flow_log_write_string(s3);
  info_flow_log_write_int(i1);
}


void info_flow_log_sssi_read(info_flow_op_enum_t op, char *s1, char *s2, char *s3, int *ai1) {
  info_flow_log_read_uint32_t(op);  
#ifdef USE_SENTINEL
  {
    uint32_t sent;
    info_flow_log_read_uint32_t(&sent);
    assert (sent == THE_SENTINEL);
  }
#endif
  info_flow_log_read_string(s1);
  info_flow_log_read_string(s2);
  info_flow_log_read_string(s3);
  info_flow_log_read_int(ai1);
}

void info_flow_log_op(uint8_t write, info_flow_op_enum_t op) {
  info_flow_log_uint32_t(op);
}

void info_flow_log_sentinel(uint8_t write) {
#ifdef USE_SENTINEL
  if (write) {
    info_flow_log_write_uint32_t(THE_SENTINEL);
  }
  else {
    uint32_t sent;
    info_flow_log_read_uint32_t(&sent);
    assert (sent == THE_SENTINEL);
  }
}
#endif

void info_flow_log_uint32_t(uint8_t write, uint32_t *ai) {
  if (write) {   
    *((uint32_t *)if_log_ptr) = *ai;	
  }
  else {
    *ai =  *((uint32_t *)if_log_ptr);
  }
  if_log_ptr += sizeof(uint32_t);  
}


void info_flow_log_string(uint8_t write, char *s) {
  int i;			
  if (write) {
    int n = strlen(s);		
    if (n > MAX_STRING_LEN) { 
      n = MAX_STRING_LEN; 
    }
    info_flow_log_uint32_t(1, &n);
    for (i=0; i<n; i++) {		
      info_flow_log_uint8_t(1, &(str[i]));
    }
  }				


void info_flow_log_sssi(uint8_t write, info_flow_op_enum_t op, char *s1, char *s2, char *s3, uint32_t *ai1) {
  info_flow_log_op(write,op);  
  info_flow_log_sentinel(write);
  info_flow_log_string(write,s1);
  info_flow_log_string(write,s2);
  info_flow_log_string(write,s3);
  info_flow_log_uint32_t(write,ai1);
}






#define WITH_NAME(r,rest) \
{ \
  char name[120]; \
  target_phys_addr_t paddr; \
  paddr = cpu_get_phys_addr(env, r); \
  if (paddr!=-1)	{ \
    copy_string_phys(name, paddr, 120); \
    rest; \
  } \
} 

#define WITH_NAMES(r1,r2,rest) \
`{ \
  char name1[120], name2[120]; \
  target_phys_addr_t paddr; \
  paddr = cpu_get_phys_addr(env, r1); \
  if (paddr!=-1)	{ \
    copy_string_phys(name1, paddr, 120); \
    paddr = cpu_get_phys_addr(env, r2); \
    if (paddr!=-1)	{ \
      copy_string_phys(name2, paddr, 120);  \
      rest; \
    } \
  } \
}



// one arg
#define IFLS_S_SIMP(op,r) \
  WITH_NAME(r,IFLS_S(op,name))

// two args
#define IFLS_IS_SIMP(op,r1,r2) \
  WITH_NAME(r2,IFLS_IS(op,r1,name))

#define IFLS_SI_SIMP(op,r1,r2) \
  WITH_NAME(r1,IFLS_SI(op,name,r2))

#define IFLS_SS_SIMP(op,r1,r2) \
  WITH_NAMES(r1,r2,IFLS_SS(op,name1,name2))

// three args
#define IFLS_SII_SIMP(op,r1,r2,r3) \
 WITH_NAME(r1,IFLS_SII(op,name,r2,r3))

#define IFLS_ISI_SIMP(op,r1,r2,r3) \
 WITH_NAME(r2,IFLS_ISI(op,r1,name,r3))

#define IFLS_IIS_SIMP(op,r1,r2,r3) \
  WITH_NAME(r3,IFLS_IIS(op,r1,r2,name))

#define IFLS_ISS_SIMP(op,r1,r2,r3) \
 WITH_NAMES(r2,r3,IFLS_ISS(op,r1,name1,name2))

#define IFLS_SSI_SIMP(op,r1,r2,r3) \
 WITH_NAMES(r1,r2,IFLS_SSI(op,name1,name2,r3))

#define IFLS_SIS_SIMP(op,r1,r2,r3) \
 WITH_NAMES(r1,r3,IFLS_SIS(op,name1,r2,name2))

// four args
#define IFLS_ISIII_SIMP(op,r1,r2,r3,r4,r5) \
 WITH_NAME (r2,IFLS_ISIII(op,r1,name,r3,r4,r5))

#define IFLS_ISII_SIMP(op,r1,r2,r3,r4) \
  WITH_NAME(r2,IFLS_ISII(op,r1,name,r3,r4))

#define IFLS_ISISI_SIMP(op,r1,r2,r3,r4,r5) \
 WITH_NAMES(r2,r4,IFLS_ISISI(op,r1,name1,r3,name2,r5))

#define IFLS_ISIS_SIMP(op,r1,r2,r3,r4) \
 WITH_NAMES(r2,r4,IFLS_ISIS(op,r1,name1,r3,name2))

#define IFLS_SSII_SIMP(op,r1,r2,r3,r4) \
  WITH_NAMES(r1,r2,IFLS_SSII(op,name1,name2,r3,r4))

// ugh.  three strings!
//#define IFLS_SSSI_SIMP(op,r1,r2,r3,r4)	\
void info_flow_sssi_simp(info_flow_op_enum_t op, uint32_t r1, uint32_t r2, 
			 uint32_t r2, uint32_t r4) {
  
  char name1[120], name2[120], name3[120];		    
  paddr = cpu_get_phys_addr(env, r1); 
  if (paddr!=-1)	{ 
    copy_string_phys(name1, paddr, 120); 
    paddr = cpu_get_phys_addr(env, r2); 
    if (paddr!=-1)	{ 
      copy_string_phys(name2, paddr, 120);  
      paddr = cpu_get_phys_addr(env, r3); 
      if (paddr!=-1)	{			
        copy_string_phys(name3, paddr, 120);	
	IFLS_SSSI(op,name1,name2,name3,r4);	
      }						
    }						
  } 
}
