#define __WIN32_LEAN_AND_MEAN__
#include <windows.h>
#include <stdio.h>
#include "vmnotify.h"

int main(int argc, char **argv) {
  PLARGE_INTEGER SystemTime;
  SystemTime = (PLARGE_INTEGER) malloc(sizeof(LARGE_INTEGER));
  vm_mark_buf_in(&SystemTime, sizeof(PLARGE_INTEGER));
  NtQuerySystemTime(SystemTime);
  vm_mark_buf_out(SystemTime, sizeof(DWORD));
  return 0;
}
