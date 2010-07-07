#include <stdio.h>
#include <windows.h>

#include <Iphlpapi.h>
#pragma comment(lib, "Iphlpapi")

#include "vmnotify.h"


int main(int argc, char **argv) {
    unsigned char *buf;
    int status;
    DWORD dwSize = 0x1000;

    buf = (unsigned char *) malloc(0x1000);
    
    vm_mark_buf_in(&buf, 4);
    status = GetExtendedTcpTable(buf,                        // pTcpTable
                        &dwSize,                    // pdwSize
                        FALSE,                      // bOrder
                        AF_INET,                    // ulAf
                        TCP_TABLE_OWNER_PID_ALL,    // TableClass
                        0);                         // Reserved
    vm_mark_buf_out(buf, 0x1000);
    
    printf(status == NO_ERROR ? "Success\n" : "Fail\n");
    
    return status;
}
