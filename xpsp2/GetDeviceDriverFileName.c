#define __WIN32_LEAN_AND_MEAN__
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#include <stdio.h>
#include "vmnotify.h"


int main(int argc, char **argv) {
  char *name;
  DWORD outcb;
  int i;
  DWORD base = strtoul(argv[1], NULL, 0);
  name = (char *) malloc(1024);
  vm_mark_buf_in(&base, 4);
  vm_mark_buf_in(&name, 4);
  outcb = GetDeviceDriverFileName(base, name, 1024);
  vm_mark_buf_out(name, 1024);
  printf("%s\n", name);
  return 0;
}
