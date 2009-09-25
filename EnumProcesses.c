#define __WIN32_LEAN_AND_MEAN__
#include <windows.h>
#include <stdio.h>
#include "vmnotify.h"

int main(int argc, char **argv) {
  DWORD *pids;
  DWORD outcb;
  buf = (DWORD *) malloc(256);
  vm_mark_buf_in(&pids, 4);
  EnumProcesses(pids, 256, outcb);
  vm_mark_buf_out(pids, 256);
  return 0;
}
