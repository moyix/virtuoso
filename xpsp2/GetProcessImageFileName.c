#define __WIN32_LEAN_AND_MEAN__
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#include <stdio.h>
#include "vmnotify.h"


int main(int argc, char **argv) {
  char *name;
  DWORD outcb;
  HANDLE h;
  int i;
  DWORD pid = atoi(argv[1]);
  name = (char *) malloc(1024);
  vm_mark_buf_in(&pid, 4);
  vm_mark_buf_in(&name, 4);
  h = OpenProcess(PROCESS_QUERY_INFORMATION, 0, pid);
  outcb = GetProcessImageFileName(h, name, 1024);
  vm_mark_buf_out(name, 1024);
  printf("%s\n", name);
  return 0;
}
