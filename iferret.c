
#include <stdio.h>
#include <string.h>

#define K 1024
#define M (K * K)
#define VIDEO_MEM ((256 + 64) * K)
#define BIOS_MEM (8192 * K)

unsigned int phys_ram_size;


int main (int argc, char **argv) {
  char *log_prefix, filename[1024];
  int i,num_logs, ram;
  
  if (argc != 4) {
    printf ("Usage: iferret ramsize log_prefix num_logs\n");
    exit(1);
  }
  ram = atoi(argv[1]);
  phys_ram_size = ram * M + BIOS_MEM + VIDEO_MEM;
  log_prefix = argv[2];
  num_logs = atoi(argv[3]);
  
  printf ("ram is %d MB, giving total mem of %d\n", ram, phys_ram_size);

  if_log_create();
  for (i=0; i<num_logs; i++) {
    sprintf(filename, "%s-%d", log_prefix, i);
    printf ("reading log: %s\n", filename);
    if_log_spit(filename);
  }
}
