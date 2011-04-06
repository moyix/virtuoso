/* ©2011 Massachusetts Institute of Technology */
#define __WIN32_LEAN_AND_MEAN__
#include <windows.h>
#include <stdio.h>
#include "vmnotify.h"

int main(int argc, char **argv) {
  DWORD pid;
  vm_mark_buf_in(NULL, 0);
  pid = GetCurrentProcessId();
  vm_mark_buf_out(&pid, sizeof(DWORD));
  return 0;
}
