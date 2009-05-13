%{
  #include <stdlib.h>
  void yyerror(char *);
  #include "y.tab.h"
%}

%%


"syscall_param"    return  SYSCALL_PARAM;

"memory_access"    return  MEMORY_ACCESS;

"file_access"      return  FILE_ACCESS;

"add_syscall_num"  return  ADD_SYSCALL_NUM;

"add_buff_name"    return  ADD_BUFF_NAME;

"add_buff_len"     return  ADD_BUFF_LEN;

"add_rw"           return  ADD_RW;

"add_proc"         return  ADD_PROC; 

"add_filename "    return  ADD_FILENAME;

"add_time"         return  ADD_TIME;

"read"             return READ;

"write"            return WRITE;

;                  return  SEPARATOR;

^\/\/(.*)\n        return  COMMENT;  


[0-9][0-9][0-9][0-9]   {
                          yylval = atoi(yytext);
                          return  NUMBER4;
                       }

[0-9][0-9][0-9]        {
                          yylval = atoi(yytext);
                          return  NUMBER3;
                       }

[0-9][0-9]             {
                          yylval = atoi(yytext);
                          return  NUMBER2;
                       }

[0-9]                  {
                          yylval = atoi(yytext);
                          return  DIGIT;
                       }

[A-Za-z][A-Za-z0-9\-_.]+  {
                            yylval = yytext;
                            return  WORD;
                          } 



%%

int yywrap(void) {
  return 1;
}

