%token NUMBER4
%token NUMBER3
%token NUMBER2
%token DIGIT
%token WORD
%token SYSCALL_PARAM
%token MEMORY_ACCESS
%token FILE_ACCESS
%token ADD_SYSCALL_NUM
%token ADD_BUFF_NAME
%token ADD_BUFF_LEN
%token ADD_RW
%token ADD_PROC
%token ADD_FILENAME
%token ADD_TIME
%token SEPARATOR
%token COMMENT
%token READ
%token WRITE

%{
  char *label_suffix = NULL;
  uint8_t add_syscall_num = FALSE;
  uint8_t add_buff_name= FALSE;
  uint8_t add_buff_len = FALSE;
  uint8_t add_rw = FALSE;
  uint8_t add_proc = FALSE;
  uint8_t add_filename = FALSE;
  uint8_t add_time = FALSE;
  int syscall_num = -1;
  char *buff_name = NULL;
  int p_arg_num = -1;
  int n_arg_num = -1;
  uint8_t read = FALSE;
  uint8_t write = FALSE;
%}

%%

labeling_instruction:
     what_to_label ',' how_to_label SEPARATOR
     ;

what_to_label:
     syscall_param   { add_syscall_param_rule(); }
     | memory_access { add_memory_access_rule(); }
     | file_access   { add_file_acccess_rule(); }
     ;

syscall_param:
     SYSCALL_PARAM syscall_num buff_name p_arg_num n_arg_num process_opt 
     ;

memory_access:
     MEMORY_ACCESS rw process_opt 
     ;
 
file_access:
     FILE_ACCESS file_name rw process_opt
     ;

how_to_label:
     label_suffix             { label_suffix = $1; }
     ADD_SYSCALL_NUM          { add_syscall_num = TRUE; }
     ADD_BUFF_NAME            { add_buff_name = TRUE; }
     ADD_BUFF_LEN             { add_buff_len = TRUE; } 
     ADD_RW                   { add_rw add_proc = TRUE; }
     ADD_PROC                 { add_proc = TRUE; }
     ADD_FILENAME             { add_filename = TRUE; }
     ADD_TIME                 { add_time = TRUE; }
     ;

syscall_num: 
     NUMBER3                  { syscall_num = $1; }
     ;

buff_name: 
     WORD                     { buff_name = $1; }
     ;
     
p_arg_num: 
     arg_num                  { p_arg_num = $1; }
     ;

n_arg_num:
     arg_num                  { n_arg_num = $1; }
     ;

arg_num:
     DIGIT               
     ;

process_opt:
     process                  
     |
     ;

process:
     NUMBER4                  { process_id = $1; }
     | WORD                   { process_name = $1; }
     ;

rw:
     READ                     { read = TRUE; }
     | WRITE                  { write = TRUE; }

file_name:
     WORD                     {file_name = $1; }
     ;



