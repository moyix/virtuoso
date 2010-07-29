#define __WIN32_LEAN_AND_MEAN__
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#include <stdio.h>
#include "vmnotify.h"


int main(int argc, char **argv) {
  DWORD *devs;
  DWORD outcb;
  int i;
  devs = (DWORD *) malloc(1024);
  vm_mark_buf_in(&devs, 4);
  EnumDeviceDrivers(devs, 1024, &outcb);
  vm_mark_buf_out(devs, 1024);
  for(i = 0; i < outcb/sizeof(DWORD); i++)
    printf("%#x\n", devs[i]);
  return 0;
}
