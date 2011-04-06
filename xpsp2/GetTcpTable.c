/* ©2011 Massachusetts Institute of Technology */
#include <stdio.h>
#include <windows.h>

#include <Iphlpapi.h>

#include "vmnotify.h"
#pragma comment(lib, "Ws2_32")

typedef DWORD (*myGetTcpInfo)(PVOID, PDWORD, BOOL, ULONG, TCP_TABLE_CLASS, ULONG);

int main(int argc, char **argv) {
    unsigned char *buf;
    int status, x;
    MIB_TCPTABLE_OWNER_PID *ret;
    myGetTcpInfo gettcp;
    HMODULE iphlp;
    DWORD dwSize = 0x1000;

    buf = (unsigned char *) malloc(0x1000);
    
    vm_mark_buf_in(&buf, 4);
    iphlp = LoadLibrary("iphlpapi.dll");
    gettcp = (myGetTcpInfo) GetProcAddress(iphlp, "GetExtendedTcpTable");
    status =     gettcp(buf,                        // pTcpTable
                        &dwSize,                    // pdwSize
                        FALSE,                      // bOrder
                        AF_INET,                    // ulAf
                        TCP_TABLE_OWNER_PID_ALL,    // TableClass
                        0);                         // Reserved
    vm_mark_buf_out(buf, 0x1000);
    
    printf(status == NO_ERROR ? "Success\n" : "Fail\n");
    
    ret = (MIB_TCPTABLE_OWNER_PID *)buf;
    for(x = 0; x < ret->dwNumEntries; x++) {
        printf("Pid: %4d %15s : %-6d -> %15s : %-6d\n",
                ret->table[x].dwOwningPid,
                inet_ntoa(*((struct in_addr *) &ret->table[x].dwLocalAddr)),
                ntohs(ret->table[x].dwLocalPort),
                inet_ntoa(*((struct in_addr *) &ret->table[x].dwRemoteAddr)),
                ntohs(ret->table[x].dwRemotePort)
              );
    }

    return status;
}
