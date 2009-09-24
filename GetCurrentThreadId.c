#define __WIN32_LEAN_AND_MEAN__
#include <windows.h>
#include <stdio.h>
#include "vmnotify.h"

int main(int argc, char **argv) {
  DWORD tid;
  vm_mark_buf_in(NULL, 0);
  tid = GetCurrentThreadId();
  vm_mark_buf_out(&tid, sizeof(DWORD));
  return 0;
}
