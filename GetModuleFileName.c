#define __WIN32_LEAN_AND_MEAN__
#include <windows.h>
#include <stdio.h>
#include "vmnotify.h"

int main(int argc, char **argv) {
  char *buf;
  buf = (char *) malloc(256);
  vm_mark_buf_in(&buf, 4);
  GetModuleFileName(NULL, buf, 256);
  vm_mark_buf_out(buf, 256);
  printf("%s\n", buf);
  return 0;
}
