#include <sys/types.h>
#include <unistd.h>

#include "exec.h"
//#include "lookup_table.h"
#include "host-utils.h"
#include "iferret_log.h"
#include "iferret_syscall.h"

target_phys_addr_t cpu_get_phys_addr(CPUState *env, target_ulong addr);


void iferret_log_socketcall(iferret_syscall_t *scp) {
  target_phys_addr_t paddr;
  switch (EBX) {
  case 1: // socket
    { 
      int family, type, protocol, *ptr;
      paddr = cpu_get_phys_addr(env, ECX);
      char tempbuf[1024];
      if (paddr!=-1)       {
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	ptr = (int*) tempbuf;
	family = *ptr++; type =*ptr++; protocol = *ptr++;
	iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_SOCKET,family,type,protocol);
      }
    }
    break;
  case 2: // bind
    {
      int fd, len, group, *ptr; struct sockaddr_in *sap;
      unsigned char b0, b1, b2, b3, *bptr, b4, b5, b6, b7,*str; 
      unsigned short *sptr;
      unsigned char pkttype, halen; // , sll_addr[8];
      paddr = cpu_get_phys_addr(env, ECX);
      if (paddr!=-1)       {
	target_ulong sap_addr;
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	ptr = (int*) tempbuf;
	fd = *ptr++;
	sap_addr = (target_ulong) *ptr++; 
	len= *ptr++;
	paddr = cpu_get_phys_addr(env, sap_addr);
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the args
	//bptr = (unsigned char*)&sa.sin_addr.s_addr;
	bptr = tempbuf;
	b0=*bptr++; b1=*bptr++; b2=*bptr++; b3=*bptr++;
	b4=*bptr++; b5=*bptr++; b6=*bptr++; b7=*bptr++;
	bptr = tempbuf; bptr+=2;
	sap = (struct sockaddr_in*) tempbuf;
	ptr = (int*) tempbuf; ptr++; pid =*ptr++;  group = *ptr++;
	if (sap->sin_family == 1 ) { // PF_LOCAL
	  iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_BIND_PF_LOCAL,fd,b0,b1,b2,b3,b4,b5,b6,b7,tempbuf);
	  //           fprintf(logfile,"family  1; fd %d; file %s", fd, bptr);
	}
	else if (sap->sin_family == 2 ) { // PF_INET
	  //	  char str[1024];
	  //	  sprintf(str,"%d.%d.%d.%d:%d",b4,b5,b6,b7,b2*256+b3);
	  iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_BIND_PF_INET,fd,b0,b1,b2,b3,b4,b5,b6,b7,tempbuf);

	  //           fprintf(logfile,"family  2; fd %d; %d.%d.%d.%d:%d", 
	  //                   fd, b4, b5, b6, b7, b2*256+b3);
	}
	else if (sap->sin_family == 16 ) {// PF_NETLINK
	  iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_BIND_PF_NETLINK,fd,b0,b1,b2,b3,b4,b5,b6,b7,tempbuf);                  
	  //           fprintf(logfile,"family 16; fd %d; pid %d; group %d", 
	  //                   fd, pid, group);
	}
	else if (sap->sin_family == 17 )  {// PF_PACKET
	  char str[1024];
	  bptr = tempbuf; bptr += 8; sptr = (unsigned short*)bptr;
	  bptr +=2; pkttype =*bptr; halen =*bptr++;
	  //                fprintf(logfile,"family 17; fd %d; protocol 0x%x; ifindex 0x%x",
	  //                fd, b2*256+b3, pid);
	  //                fprintf(logfile,"family 17; fd %d; protocol %d; ifindex %d; hatype %d; pkttype %d; halen %d; addr %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x", 
	  //                sap->sin_family, fd, b2*256+b3, pid, *sptr, pkttype, halen, 
	  //                tempbuf[12], tempbuf[13], tempbuf[14], tempbuf[15],
	  //                tempbuf[16], tempbuf[17], tempbuf[18], tempbuf[19]);
	  //	  sprintf(str,"%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x", 
	  //		  tempbuf[12], tempbuf[13], tempbuf[14], tempbuf[15],
	  //		  tempbuf[16], tempbuf[17], tempbuf[18], tempbuf[19]);
	  //	  iferret_log_syscall_write(scp,EBX,sap->sin_family,fd,*sptr,pktype,halen,addr);
	  iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_BIND_PF_PACKET,fd,b0,b1,b2,b3,b4,b5,b6,b7,*sptr,pktype,halen,tempbuf[12], tempbuf[13], tempbuf[14], tempbuf[15],tempbuf[16], tempbuf[17], tempbuf[18], tempbuf[19]);
	} else {
	  //	  char str[1024];
	  // fprintf(logfile,"family %2d; fd %d; %d.%d.%d.%d-%d.%d.%d.%d(%c%c%c%c%c%c%c%c); len %d", 
	  //                   sap->sin_family, fd, b0, b1, b2, b3,b4, b5, b6, 
	  //                   b7, b0, b1, b2, b3,b4, b5, b6, b7, len);
	  //	  sprintf(str,"%d.%d.%d.%d-%d.%d.%d.%d(%c%c%c%c%c%c%c%c", 
	  //		  b0, b1, b2, b3,b4, b5, b6, 
	  //		  b7, b0, b1, b2, b3,b4, b5, b6, b7);
	  //	  iferret_log_syscall_write(scp,EBX,sap->sin_family,fd,godknowswhast);
	  iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_BIND_ELSE,sap->sin_family, fd,b0,b1,b2,b3,b4,b5,b6,b7);
	}
      }
    }
    break;
  case 3: // connect
    {
      int fd, len, *ptr; struct sockaddr_in *sap;
      unsigned char b0, b1, b2, b3, *bptr, b4, b5, b6, b7, *str; 
      //   fprintf(logfile,"PID %3d (%16s)[sys_connect 102]%d: ", pid, command, EBX);
      paddr = cpu_get_phys_addr(env, ECX);
      if (paddr!=-1)       {
	target_ulong sap_addr;
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	ptr = (int*) tempbuf;
	fd = *ptr++; 
	sap_addr = *ptr++; 
	len = *ptr++;
	paddr = cpu_get_phys_addr(env, (target_ulong) sap_addr);
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the args
	bptr = tempbuf;
	b0=*bptr++; b1=*bptr++; b2=*bptr++; b3=*bptr++;
	b4=*bptr++; b5=*bptr++; b6=*bptr++; b7=*bptr++;
	bptr = tempbuf; bptr+=2;
	sap = (struct sockaddr_in*) tempbuf;
	if (sap->sin_family == 1) {// PF_LOCAL
	  //           fprintf(logfile,"socket %d; family  1; file %s", fd, bptr);
	  //	  str = bptr;
	  iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_CONNECT_PF_LOCAL, fd,b0,b1,b2,b3,b4,b5,b6,b7,tempbuf);

	}
	else if (sap->sin_family == 2) { // PF_INET
	  //	  char str[1024];
	  //           fprintf(logfile,"socket %d; family  2; %d.%d.%d.%d:%d", 
	  //                   fd, b4, b5, b6, b7, b2*256+b3);
	  //	  sprintf(str,"%d.%d.%d.%d:%d", b4, b5, b6, b7, b2*256+b3);              
	  iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_CONNECT_PF_INET, fd,b0,b1,b2,b3,b4,b5,b6,b7,tempbuf);
	}
	else {
	  //           fprintf(logfile,"socket %d; family %2d;",
	  //                   fd, sap->sin_family);
	  iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_CONNECT_PF_ELSE, sap->sin_family, fd,b0,b1,b2,b3,b4,b5,b6,b7,tempbuf);
	}
      }
      //   fprintf(logfile,"\n");
    }        
    break;
  case 4: // listen
    //      fprintf(logfile,"PID %3d (%16s)[sys_listen  102]%d: ", pid, command, EBX);
    paddr = cpu_get_phys_addr(env, ECX);
    if (paddr!=-1) {
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
      //   fprintf(logfile,"socket %d", *(int*) tempbuf);
      iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_LISTEN,*((int*) tempbuf));
    }
    //      fprintf(logfile,"\n");
    break;
  case 5: // accept
    //      fprintf(logfile,"PID %3d (%16s)[sys_accept  102]%d: ", pid, command, EBX);
    paddr = cpu_get_phys_addr(env, ECX);
    if (paddr!=-1) {
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
      //   fprintf(logfile,"socket %d", *(int*) tempbuf);
      iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_ACCEPT,*(int*) tempbuf); 
    }
    //      fprintf(logfile,"\n");      
    break;
  case 6: // getsockname
    //      fprintf(logfile,"PID %3d (%16s)[sys_getskna 102]%d: ", pid, command, EBX);
    paddr = cpu_get_phys_addr(env, ECX);
    if (paddr!=-1) {
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
      //   fprintf(logfile,"socket %d", *(int*) tempbuf);
      iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_GETSOCKNAME,*(int*) tempbuf); 
    }
    //      fprintf(logfile,"\n");      
    break;
  case 7: // getpeername
    //      fprintf(logfile,"PID %3d (%16s)[sys_getpeer 102]%d: ", pid, command, EBX);
    paddr = cpu_get_phys_addr(env, ECX);
    if (paddr!=-1) {
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
      //   fprintf(logfile,"socket %d", *(int*) tempbuf);
      iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_GETPEERNAME,*(int*) tempbuf); 
    }
    //      fprintf(logfile,"\n");     
    break;
  case 8: 
    {// socketpair
      int domain, type, protocol, *ptr;
      // int * socket_vector;
      //      fprintf(logfile,"PID %3d (%16s)[sys_skpair  102]%d: ", pid, command, EBX);
      paddr = cpu_get_phys_addr(env, ECX);
      if (paddr!=-1)       {
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	ptr = (int *) tempbuf;
	domain =*ptr++; 
	type=*ptr++; 
	protocol=*ptr++; 
	//      socket_vector= (int *)*ptr++;
	// fprintf(logfile,"domain %d; type %d; protocol %d\n", *(int*) tempbuf);
	//	IFLW_PUT_UINT32_T(domain);
	//	IFLW_PUT_UINT32_T(type);
	//	IFLW_PUT_UINT32_T(protocol);
	iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_SOCKETPAIR,domain,type,protocol,*(int*)tempbuf); 
      }
    }          
    break;    
  case 9: // send
    {
      int fd, msg, len, *ptr;
      //   fprintf(logfile,"PID %3d (%16s)[sys_send    102]%d: ", pid, command, EBX);
      paddr = cpu_get_phys_addr(env, ECX);
      if (paddr!=-1)       {
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	ptr = (int*) tempbuf;
	fd = *ptr++; msg=*ptr++; len= *ptr++;
	//   fprintf(logfile,"socket %d ", fd);            
	bzero(tempbuf, 120);
	paddr = cpu_get_phys_addr(env, msg);
	cpu_physical_memory_read(paddr, tempbuf, 30); 
	for (i=0; i<30; i++)
	  if (iscntrl(tempbuf[i])) tempbuf[i]='.';
	//   fprintf(logfile,"(%s)(%d)", tempbuf, len);
	iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_SEND,fd,tempbuf);
      }
      //   fprintf(logfile,"\n");  
    }
    break;
  case 10: // recv
    {
      int fd, len, *ptr;
      unsigned int msg;
          
      //   fprintf(logfile,"PID %3d (%16s)[sys_recv    102]%d: ", pid, command, EBX);
      paddr = cpu_get_phys_addr(env, ECX);
      if (paddr!=-1)       {
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	ptr = (int*) tempbuf;
	fd = *ptr++; msg=*ptr++; len= *ptr;
	//   fprintf(logfile,"socket %d, msg 0x%08x\n", fd, msg);
	iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_RECV,fd,msg,len);
      }
    }
    break;
  case 11: // sendto
    {
      int fd,msg, len, *ptr; struct sockaddr_in *sap;
      unsigned char b0, b1, b2, b3, *bptr, b4, b5, b6, b7, *str;
       
      //   fprintf(logfile,"PID %3d (%16s)[sys_sendto  102]%d: ", pid, command, EBX);
      paddr = cpu_get_phys_addr(env, ECX);
      if (paddr!=-1)       {
	target_ulong sap_addr;
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	ptr = (int*) tempbuf;
	fd =  *ptr++; 
	msg = *ptr++;
	len = *ptr++;
	ptr++;
	sap_addr = *ptr;
	//   fprintf(logfile,"socket %d --> ", fd);
	bzero(tempbuf, 120);
	paddr = cpu_get_phys_addr(env, sap_addr);
	cpu_physical_memory_read(paddr, tempbuf, 120); 
	bptr = tempbuf;
	b0=*bptr++; b1=*bptr++; b2=*bptr++; b3=*bptr++;
	b4=*bptr++; b5=*bptr++; b6=*bptr++; b7=*bptr++;
	bptr = tempbuf; bptr+=2;     
	sap = (struct sockaddr_in*) tempbuf;
	if (sap->sin_family == 1 ) {// PF_LOCAL
	  //              IFLW_PUT_STRING(bptr);
	  //           fprintf(logfile,"[dest: family  1; file %s]", bptr); 
	  iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_SENDTO_PF_LOCAL, fd,b0,b1,b2,b3,b4,b5,b6,b7,tempbuf);
	}
	else if (sap->sin_family == 2 ) {// PF_INET
	  char str[1024];
	  //           fprintf(logfile,"[dest: family  2; %d.%d.%d.%d:%d]", 
	  //                   b4, b5, b6, b7, b2*256+b3);
	  sprintf(str,"%d.%d.%d.%d:%d", b4, b5, b6, b7, b2*256+b3);
	  iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_SENDTO_PF_INET, fd,b0,b1,b2,b3,b4,b5,b6,b7,tempbuf);
	}
	else {
	  //fprintf(logfile,"[dest: family %2d]", sap->sin_family);
	  iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_SENDTO_PF_ELSE, sap->sin_family,fd, b0,b1,b2,b3,b4,b5,b6,b7,tempbuf);
	}
	/*
	bzero(tempbuf, 120);
	paddr = cpu_get_phys_addr(env, msg);
	cpu_physical_memory_read(paddr, tempbuf, 30); 
	for (i=0; i<30; i++)
	  if (iscntrl(tempbuf[i])) tempbuf[i]='.';
	*/
	//   fprintf(logfile," (%s)(%d)", tempbuf, len);
      }
      //   fprintf(logfile,"\n");  
    }
    break;
  case 12: // recvfrom
    {
      int fd, *ptr;
      //   fprintf(logfile,"PID %3d (%16s)[sys_recvfro 102]%d: ",
      //            pid, command, EBX);
      paddr = cpu_get_phys_addr(env, ECX);
      if (paddr!=-1)       {
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	ptr = (int*) tempbuf;
	fd = *ptr++; 
	//   fprintf(logfile,"socket %d  ", fd);
	iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_RECVFROM,fd);
      }
      //   fprintf(logfile,"\n");  
    }
    break;
  case 13: // shutdown
    {
      int fd, *ptr;
       
      //   fprintf(logfile,"PID %3d (%16s)[sys_shutdow 102]%d: ",
      //            pid, command, EBX);
      paddr = cpu_get_phys_addr(env, ECX);
      if (paddr!=-1)       {
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	ptr = (int*) tempbuf;
	fd = *ptr++; 
	iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_SHUTDOWN,fd,*ptr);
	/*
	  if (*ptr == 0 ) 
	  fprintf(logfile,"socket %d (SHUT_RD)",   fd);
	  else if (*ptr == 1 )
	  fprintf(logfile,"socket %d (SHUT_WR)",   fd);
	  else if (*ptr == 2 )
	  fprintf(logfile,"socket %d (SHUT_RDWR)", fd);
	*/
      }
      //   fprintf(logfile,"\n");
    }
    break;
  case 14: // setsockopt
    {
      int fd, level, option, *ptr;
       
      //   fprintf(logfile,"PID %3d (%16s)[sys_setskop 102]%d: ",
      //            pid, command, EBX);
      paddr = cpu_get_phys_addr(env, ECX);
      if (paddr!=-1)       {
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	ptr = (int*) tempbuf;
	fd = *ptr++; level=*ptr++; option=*ptr++;
	//   fprintf(logfile,"socket %d; level %d; option %d", 
	//            fd, level, option);
	iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_SETSOCKOPT,fd,level,option);
      }
      //   fprintf(logfile,"\n");
    }
    break;
  case 15: // getsockopt
    {
      int fd, level, option, *ptr;
       
      //   fprintf(logfile,"PID %3d (%16s)[sys_getskop 102]%d: ", pid, command, EBX);
      paddr = cpu_get_phys_addr(env, ECX);
      if (paddr!=-1)       {
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	ptr = (int*) tempbuf;
	fd = *ptr++; level=*ptr++; option=*ptr++;
	//   fprintf(logfile,"socket %d; level %d; option %d", fd, level, option);
	iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_GETSOCKOPT,fd,level,option);
      }
      //   fprintf(logfile,"\n");
    }
    break;
  case 16: // sendmsg
    {
      int fd, *ptr;
       
      //   fprintf(logfile,"PID %3d (%16s)[sys_sendmsg 102]%d: ", pid, command, EBX);
      paddr = cpu_get_phys_addr(env, ECX);
      if (paddr!=-1)       {
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	ptr = (int*) tempbuf;
	fd = *ptr++; 
	//   fprintf(logfile,"socket %d", fd);
	iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_SENDMSG,fd);
      }
      //   fprintf(logfile,"\n");
    }
    break;
  case 17: // recvmsg
    {
      int fd, *ptr;
       
      //   fprintf(logfile,"PID %3d (%16s)[sys_recvmsg 102]%d: ", pid, command, EBX);
      paddr = cpu_get_phys_addr(env, ECX);
      if (paddr!=-1)       {
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	ptr = (int*) tempbuf;
	fd = *ptr++; 
	//   fprintf(logfile,"socket %d", fd);
	iferret_log_socketcall_write_va(scp,IFLO_SYS_SOCKETCALL_RECVMSG,fd);
      }
      //   fprintf(logfile,"\n");
    }
    break;
  }
  break;
}
