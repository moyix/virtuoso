
#include <stdio.h>
#include <zlib.h>
#include <stdint.h>
#include "info_flow.h"


uint32_t if_log_rollup_count = 0;  

// save current if log to a file for some reason    
void if_log_to_file() {
  char filename[1024];
  gzFile out;
  //  FILE *fp;

  snprintf (filename, 1024, "/scratch/tmp/ifl-%d-%d", getpid(), if_log_rollup_count);
  //  fp = fopen (filename, "w");
  out = gzopen(filename,"wb");

  //  fwrite(if_log_base, 1, if_log_ptr-if_log_base, fp);
  gzwrite(out, (char *) if_log_base, (unsigned int) (if_log_ptr - if_log_base)); 
  //  fclose(fp);
  gzclose(out);

  printf ("wrote if log to %s\n", filename);
  if_log_rollup_count ++;

  // processing complete; ready to write over old log.  
  if_log_ptr = if_log_base; 

}




// info-flow log is full.  Dump it to a file. 
void if_log_rollup() {

  if_log_to_file();

}

