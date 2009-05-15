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
%token READ
%token WRITE
%token SEPARATOR
%token COMMENT
%token NUMBER4
%token NUMBER3
%token NUMBER2
%token DIGIT
%token WORD

%{
  extern char *label_suffix = NULL;
  extern uint8_t add_syscall_num = FALSE;
  extern uint8_t add_buff_name= FALSE;
  extern uint8_t add_buff_len = FALSE;
  extern uint8_t add_rw = FALSE;
  extern uint8_t add_proc = FALSE;
  extern uint8_t add_filename = FALSE;
  extern uint8_t add_time = FALSE;
  externint syscall_num = -1;
  char *buff_name = NULL;
  int p_arg_num = -1;
  int n_arg_num = -1;
  uint8_t read = FALSE;
  uint8_t write = FALSE;
%}

%%

labeling_spec_file: /* empty line */
     | labeling_spec_file labeling_rule
     ;

labeling_rule:
     what_to_label ',' how_to_label SEPARATOR  
{ 
  assert(the_rule.what.kind != ILSW_NONE);
  assert(the_rule.how.label_suffix != ils_empty_string);
  ils_add_labeling_rule();
}
     ;


what_to_label:
     syscall_param  
     | memory_access 
     | file_access 
;


syscall_param:
     SYSCALL_PARAM syscall_num buff_name p_arg_num n_arg_num process_opt 
 { 
   assert(the_rule.what.val.syscall_buff.syscall_num != -1);
   assert(the_rule.what.val.syscall_buff.buff_naem != ils_empty_string);
   assert(the_rule.what.val.syscall_buff.p_arg_num != -1);
   assert(the_rule.what.val.syscall_buff.n_arg_num != -1);
   the_rule.what.kind = ILSW_SYSCALL_BUFF;
   ils_add_labeling_rule();
 }     
 ;

memory_access:
     MEMORY_ACCESS rw process_opt 
{ 
  assert(the_rule.what.val.mem_access.rw != ILSRW_NONE);
  assert(the_process != ILSP_NONE);
  the_rule.what.val.mem_access.process = the_process
  ils_add_labeling_rule();
}
;
 
file_access:
     FILE_ACCESS file_name rw process_opt
{ 
  assert(the_rule.what.val.file_access.file_name != ils_empty_string);
  assert(the_rule.what.val.file_access.rw != ILSRW_NONE);
  assert(the_process != ILSP_NONE);
  the_rule.what.val.file_access.process = the_process;  
  ils_add_labeling_rule();
}
;

how_to_label:
     label_suffix      { the_rule.how.label_suffix = ils_get_string($1); }
     ADD_SYSCALL_NUM   { the_rule.how.add_syscall_num = TRUE; }
     ADD_BUFF_NAME     { the_rule.how.add_buff_name = TRUE; }
     ADD_BUFF_LEN      { the_rule.how.add_buff_len = TRUE; } 
     ADD_RW            { the_rule.how.add_rw add_proc = TRUE; }
     ADD_PROC          { the_rule.how.add_proc = TRUE; }
     ADD_FILENAME      { the_rule.how.add_filename = TRUE; }
     ADD_TIME          { the_rule.how.add_time = TRUE; }
     ;

syscall_num: 
     NUMBER3    { the_rule.what.val.syscall_buff.syscall_num = $1; }
     ;

buff_name: 
     WORD       { the_rule.what.val.syscall_buff.buff_name = $1; }
     ;
     
p_arg_num: 
     arg_num    { the_rule.what.val.syscall_buff.p_arg_num = $1; }
     ;

n_arg_num:
     arg_num    { the_rule.what.val.syscall_buff.n_arg_num = $1; }
     ;

arg_num:
     DIGIT               
     ;

process_opt:
     process                  
     |
     ;

process:
     NUMBER4     { 
                   the_process.kind = ILSP_PROCESS_ID;
                   the_process.id = $1; 
                 }
     | WORD      {
                    the_process.kind = ILSP_PROCESS_NAME;
		    process.name = $1; 
                 }
     ;

rw:
       READ      { the_rw = ILSRW_READ; }
     | WRITE     { the_rw = ILSRW_WRIE; }

file_name:
     WORD        { 
                   the_file_name.len = strlen($1);
                   the_file_name.buff = strdup($1);
                 }
     ;


label_suffix:    
     WORD 
     ;
