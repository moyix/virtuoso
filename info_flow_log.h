

#define USE_SENTINEL 1

typedef enum {
  IFAT_UINT8=1,    // IFAT = Info flow arg type
  IFAT_UINT16=2,
  IFAT_UINT32=3,
  IFAT_UINT64=4, 
  IFAT_STRING=5,
} info_flow_op_arg_type_enum_t;


typedef struct info_flow_op_arg_struct_t {
  info_flow_op_arg_type_enum_t type;   // tags the union
  union {
    uint8_t u8;        // unsigned ints of various sizes
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    char *str          // a string
  } val
} info_flow_op_arg_t;



typedef struct info_flow_op_struct_t {
  info_flow_op_enum_t num;     // the number of the op
  info_flow_op_arg_t arg[12];  // up to 12 arguments
  uint32_t num_args;          // number of arguments 
} Info_flow_op_t;

/*
  '0' means 0 bytes, i.e. no args for this op
  '1' means 1 byte, i.e. uint8_t
  '2' means 2 bytes, i.e. uint16_t
  '4' means 4 bytes, i.e. uint32_t
  '8' means 8 bytes, i.e. uint64_t
  's' means a string
*/



typedef struct Info_flow_syscall_struct_t {
  uint32_t eax;
  uint8_t is_sysenter;
  uint32_t pid;
  uint32_t callsite_eip;
  char *command;
} Info_flow_syscall_t;
