
// Note: 
// system call prototypes
// from  /usr/src/linux-headers-2.6.24-19/include/linux/syscalls.h


#include "linux_task_struct_offsets.h"


// current_task is pointer to vm physical memory at which linux task structure is
// located. 
// slot_offset is where in task_struct to find this slot.
// slot_size is size of data for that slot, in bytes.
// dest is where to copy that data
void copy_task_struct_slot(char *current_task, unsigned int slot_offset, slot_size, char *dest) {
  target_ulong paddr;

  bzero(dest,slot_size);
  paddr = cpu_get_phys_page_debug(env, current_task+slot_offset);
  if (paddr != -1) {
    cpu_physical_memory_read(paddr, &current_task, sizeof_slot);
  }
}

// copy string of length at most len-1
// from physical adddress phsaddr into buffer tempbuf
// null terminate
void copy_string_phys(char *tempbuf, char *physaddr, uint32_t len) {
  bzero(tempbuf, len);
  cpu_physical_memory_read(physaddr,tempbuf,len-1);
}


#define SYSOP(opn) glue(INFO_FLOW_OP_SYS_,opn)

// sys call with no args
#define IFLS(if_op) \
IF_WRAPPER ( \				
  IFLW_PUT_OP(SYSOP(if_op));    \
  IFLW_PUT_STRING(command);   \
)

// All syscalls iferret log entries containt this info.
#define IFLSCORE \
  IFLW_PUT_OP(SYSOP(if_op)); \
  IFLW_PUT_STRING(command); \
  IFLW_PUT_UINT32_T(pid); \
  IFLW_PUT_UINT32_T(eip_for_callsite); 


// sys call with one arg -- an int
#define IFLSI(if_op,command,val)	\
IF_WRAPPER ( \
  SYSCORE \
  IFLW_PUT_UINT32_T(val); \
) 

// sys call with two args -- two ints
#define IFLSII(if_op,command,val1,val2) \
IF_WRAPPER ( \
  SYSCORE \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_UINT32_T(val2); \
) 

// sys call with two args -- a string and an int
#define IFLSII(if_op,command,val1,val2) \
IF_WRAPPER ( \
  SYSCORE \
  IFLW_PUT_STRING(val1); \
  IFLW_PUT_UINT32_T(val2); \
) 
  

// write an entry to iferret log to capture
// context (eip & pid), number, and arguments of 
// current system call 
void iferret_log_syscall (uint8_t is_sysenter) {
  
  target_ulong paddr, regs_ebx; 
  char *current_task, **argvp, *tempbuf;
  char command[COMM_SIZE];
  int pid, uid, len, i, old_syscall_num;
  uint32_t eip_for_callsite;

  // find current_task, the ptr to the currently executing process' task_struct
  current_task = ESP & (CURRENT_TASK_MASK);
  paddr = cpu_get_phys_page_debug(env, current_task); 
  if (paddr!=-1) {
    cpu_physical_memory_read(paddr, &current_task, 4);
  }

  // grab process id, i.e. current_task->pid
  copy_task_struct_slot(current_task, PID_OFFSET, PID_SIZE, &pid);
  copy_task_struct_slot(current_task, UID_OFFSET, UID_SIZE, &uid);
  copy_task_struct_slot(current_task, COMM_OFFSET, COMM_SIZE, command);
  
  if(!table_initialized){
    init_table();
    table_initialized = 1;	
  }

  if (is_sysenter) {
    paddr = cpu_get_phys_page_debug(env, saved_esp+4*3);
    if (paddr!=-1) {
      cpu_physical_memory_read(paddr, &stack_val, 4);
    } else {
      printf("paddr is -1, oops!\n");	
      exit(1);
    }
    add_element(pid,stack_val,EAX);
    eip_for_callsite = stack_val);
  }
  else {
    if(EAX != 11 && EAX != 119){
      add_element(pid,old_eip,EAX);
    }
    eip_for_callsite = old_eip;
  }

  fprintf(logfile, "PID: %d, stack size:%d\n",pid,get_stack_size(pid));

/////////////////////////////////////
// the syscalls, by the numbers
  
  switch (EAX) {	
  case 0 : // sys_ni_syscall
    IFLS(NI_SYSCALL_0);
    break;
  case 1 : 
    // long sys_exit(int error_code);
    IFLS(EXIT);
    break;
  case 2 :
    // NB: missing from syscalls.h?  defunct?
    // long sys_fork(void);
    IFLS(FORK);
    break;
  case 3 : 
    // ssize_t sys_read(unsigned int fd, char __user *buf,
    //	                size_t count);
    IFLSI(READ,EBX);
    break;
  case 4 :
    // ssize_t sys_write(unsigned int fd, const char __user *buf,
    //                   size_t count);
    IFLSII(WRITE,EBX,EDX);
    break;
  case 5 :
    // long sys_openat(int dfd, const char __user *filename, int flags,
    //    	      int mode);
    {
      char *filenam;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(filename, paddr, 120); 
	IFLSSI(OPEN,filename,ECX);
      }
    } 
    break;
  case 6 :
    // long sys_close(unsigned int fd);
    IFLSI(CLOSE,EBX);    
    break;
  case 7 :
    // long sys_waitpid(pid_t pid, int __user *stat_addr, int options);
    IFLS(WAITPID);
    break;
  case 8 : 
    // long sys_creat(const char __user *pathname, int mode);
    {
      char *pathname;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(pathname, paddr, 120); 
	IFLSSI(CREAT,pathname,ECX);
      } 
    }
    break;
  case 9: 
    // long sys_link(const char __user *oldname,
    //	             const char __user *newname);
    {
      char *oldname, *newname;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(oldname, paddr, 120); 
	paddr = cpu_get_phys_page_debug(env, ECX);
	if (paddr!=-1)	{
	  copy_string_phys(newname, paddr, 120); 
	  IFLSSS(LINK,oldname,newname);
	}
      }
    } 
    break;
  case 10 : 
    // long sys_unlink(const char __user *pathname);
    {
      char *pathname;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(pathname, paddr, 120); 
	IFLSS(UNLINK,pathname);
      }
    }
    break;
  case 11: 
    // Also missing from syscalls.h 
    // This one is there, though? 
    // long sys_execve(char *name, char **argv,char **envp, struct pt_regs regs)
    {
      int i,n;
      char *name, **orig_argvp, **argvp;

      // first, get the name
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr != -1) {
	copy_string_phys(name, paddr, 120);	
	paddr = cpu_get_phys_page_debug(env, ECX);
	cpu_physical_memory_read(paddr, &argvp, 4);  // The first argument
	orig_argvp = argvp;      
	// first, figure out how many args
	n=0;
	while (argvp) {
	  n++;
	  if (n>12) break;
	  paddr = cpu_get_phys_page_debug(env, ECX+pid*4);
	  cpu_physical_memory_read(paddr, &argvp, 4);  // next arg
	}
	i = 0;
	// write op plus num args 
	IFLSI(EXECVE,n);
	while (argvp) {
	  paddr = cpu_get_phys_page_debug(env, argvp);	
	  copy_string_phys(command, paddr, 120); // arg 
	  IFLW_PUT_STRING(command);
	  i++;
	  if (i >12) break;
	  //paddr = cpu_get_phys_page_debug(env, argvp);
	  paddr = cpu_get_phys_page_debug(env, ECX+pid*4);
	  cpu_physical_memory_read(paddr, &argvp, 4);  // next arg
	}
      }
    }
    break;
  case 12 :
    //  long sys_chdir(const char __user *filename);
    {
      char *filename;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(filename, paddr, 120); 
	IFLSS(CHDIR,filename);
      } 
    }
    break;
  case 13 : 
    // long sys_time(time_t __user *tloc);
    IFLS(TIME);
    break;
  case 14 :
    // long sys_mknod(const char __user *filename, int mode,
    //	              unsigned dev);
    {
      char *filename;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(filename, paddr, 120); 
	IFLSSII(MKNOD,filename,ECX,EDX);
      } 
    }
    break;
  case 15 :
    // long sys_chmod(const char __user *filename, mode_t mode);
    {
      char *filename;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(filename, paddr, 120); 
	IFLSSI(MKNOD,filename,ECX);
      } 
    }
    break;
  case 16 : 
    // long sys_lchown16(const char __user *filename,
    //                   old_uid_t user, old_gid_t group);
    {
      char *filename;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(filename, paddr, 120); 
	IFLSSII(LCHOWN16,filename,ECX,EDX);
      } 
    }
    break;
  case 17 : // sys_ni_syscall
    IFLS(NI_SYSCALL_17);
    break;
  case 18 :
    // long sys_stat(char __user *filename,
    //               struct __old_kernel_stat __user *statbuf);
    {
      char *filename;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(filename, paddr, 120); 
	IFLSS(STAT,filename);
      }
    }
    break;
  case 19 :
    // off_t sys_lseek(unsigned int fd, off_t offset,
    //                 unsigned int origin);
    IFLSIII(LSEEK,EBX,ECX,EDX);
    break;
  case 20 :
    // long sys_getpid(void);
    IFLS(GETPID);
    break;
  case 21: 
    // long sys_mount(char __user *dev_name, char __user *dir_name,
    //                char __user *type, unsigned long flags,
    //	              void __user *data);
    {
      char *dev_name, *dir_name;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(dev_name, paddr, 120); 
	paddr = cpu_get_phys_page_debug(env, ECX);
	if (paddr!=-1)	{
	  copy_string_phys(dir_name, paddr, 120); 	
	  IFLSSS(MOUNT,dev_name,dir_name);
	}
      }
    } 
    break;
  case 22 :
    // long sys_oldumount(char __user *name);
    paddr = cpu_get_phys_page_debug(env, EBX);
    if (paddr!=-1)	{
      copy_string_phys(filename, paddr, 120); 
      IFLSS(OLDUMOUNT,filename);
    } 
    break;
  case 23 : 
    // long sys_setuid16(old_uid_t uid);
    IFLSI(SETUID16,EBX);
    break;
  case 24 : 
    // long sys_getuid16(void);
    IFLS(GETUID16);
    break;
  case 25 :
    // long sys_stime(time_t __user *tptr);
    IFLS(STIME);
    break;
  case 26 :
    // long sys_ptrace(long request, long pid, long addr, long data);
    IFLSIII(PTRACE,EBX,ECX,EDX);
    break;
  case 27 :
    // unsigned long sys_alarm(unsigned int seconds);
    IFLSI(ALARM,EBX);
    break;
  case 28 : 
    // I think this is really oldfstat.  can't find a prototype
    IFLS(OLDFSTAT);
    break;
  case 29 : 
    //  long sys_pause(void);
    IFLS(PAUSE);
    break;
  case 30 :     
    // long sys_utime(char __user *filename,
    //                struct utimbuf __user *times);
    {
      char *filename;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(filename, paddr, 120); 
	IFLSSI(UTIME,filename,ECX);
      } 
    }
    break;
  case 31 : // sys_ni_syscall
    IFLS(NI_SYSCALL_31);
    break;
  case 32 : // sys_ni_syscall
    IFLS(NI_SYSCALL_32);
    break;
  case 33 :
    // long sys_access(const char __user *filename, int mode);
    {
      char *filename;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(filename, paddr, 120);       
	IFLSSI(ACCESS,filename, ECX);
      } 
    }
    break;
  case 34 :
    // long sys_nice(int increment);
    IFLSI(NICE,EBX);
    break;
  case 35 : 
    // sys_ni_syscall
    IFLS(NI_SYSCALL_35);
    break;
  case 36 : 
    // long sys_sync(void);
    IFLS(SYNC);
    break;
  case 37 :
    // long sys_kill(int pid, int sig);
    IFLSII(KILL,EBX,ECX);
    break;
  case 38: 
    //  long sys_rename(const char __user *oldname,
    //	                const char __user *newname);
    {
      char *oldname, *newname;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(oldname, paddr, 120);       
	paddr = cpu_get_phys_page_debug(env, ECX);
	if (paddr!=-1)	{
	  copy_string_phys(newname, paddr, 120);       
	  IFLSSS(RENAME,oldname,newname);
	}
      }
    } 
    break;
  case 39: 
    // long sys_mkdir(const char __user *pathname, int mode);
    {
      char *pathname;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(pathname, paddr, 120);             
	IFLSSI(MKDIR,dirname,ECX);
      } 
    }
    break;
  case 40: 
    //  long sys_rmdir(const char __user *pathname);
    {
      char *pathname;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(pathname, paddr, 120);             
	IFLSS(RMDIR,pathname);
      } 
    }
    break;
  case 41: 
    //  long sys_dup(unsigned int fildes);
    IFLSI(DUP,EBX);
    break;
  case 42:
    // NB: Missing from syscalls.h. 
    // sys_pipe
    IFLS(PIPE);
    //    paddr = cpu_get_phys_page_debug(env, EBX);
    //    if (paddr!=-1)	{      
    //      fprintf(logfile,"PID %3d (%16s)", pid, command);
    //      fprintf(logfile,"[sys_pipe     42]\n");
    //} 
    break;
  case 43 : // sys_times
    // long sys_times(struct tms __user *tbuf);
    IFLS(TIMES);
    break;
  case 44 : // sys_ni_syscall    
    IFLS(NI_SYSCALL_44);
    break;
  case 45 : 
    // NB: Missing from syscalls.h 
    // sys_break
    IFLS(BREAK);
    break;
  case 46 :
    // long sys_setgid16(old_gid_t gid);
    IFLSI(SETGID16,EBX);
    break;
  case 47 : // sys_getgid16
    // long sys_getgid16(void);
    IFLS(GETGID16);
    break;
  case 48 : 
    // unsigned long sys_signal(int sig, __sighandler_t handler);
    IFLSII(SIGNAL,EBX,ECX);
    break;
  case 49 : 
    // long sys_geteuid16(void);
    IFLS(GETEUID16);
    break;
  case 50 :
    // long sys_getegid16(void);
    IFLS(GETEGID16);
    break;
  case 51 : 
    // long sys_acct(const char __user *name);
    {
      char *name;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(name, paddr, 120);             
	IFLSS(ACCT,name);
      }
    } 
    break;
  case 52 : 
    // NB: Missing from syscalls.h
    // sys_umount2
    {
      char *pathname;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(pathname, paddr, 120);
	IFLSS(MOUNT2,pathname);
      }
    } 
    break;
  case 53 : 
    // NB: Missing from syscalls.h
    // sys_lock
    IFLS(LOCK);
    break;
  case 54 : 
    // long sys_ioctl(unsigned int fd, unsigned int cmd,
    //                unsigned long arg);
    IFLSIII(IOCTL,EBX,ECX,EDX);
    break;
  case 55 : 
    // long sys_fcntl(unsigned int fd, unsigned int cmd, unsigned long arg);
    IFLSIII(FCNTL,EBX,ECX,EDX);
    break;
  case 56 :
    // NB: Missing from syscalls.h
    // sys_mpx
    IFLS(MPX);
    break;
  case 57 : 
    // long sys_setpgid(pid_t pid, pid_t pgid);    
    IFLSII(SETPGID,EBX,ECX);
    break;
  case 58 : 
    // NB: Missing from syscalls.h
    // sys_ulimit
    IFLS(ULIMIT);
    break;
  case 59 :
    // NB: Missing from syscalls.h
    // sys_oldolduname
    IFLS(OLDOLDUNAME);
    break;
  case 60 :
    // long sys_umask(int mask);
    IFLSI(UMASK,EBX);
    break;
  case 61 :
    // long sys_chroot(const char __user *filename);
    {
      char *filename;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(filename, paddr, 120);
	IFLSS(CHROOT, filename);
      }
    }
    break;
  case 62 :
    // long sys_ustat(unsigned dev, struct ustat __user *ubuf);
    IFLS(USTAT);
    break;
  case 63 : 
    // long sys_dup2(unsigned int oldfd, unsigned int newfd);
    IFLSII(DUP2,EBX,ECX);
    break;
  case 64 : 
    // long sys_getppid(void);
    IFLS(GETPPID);
    break;
  case 65 : 
    // long sys_getpgrp(void);
    IFLS(GETPGRP);
    break;
  case 66 : 
    // long sys_setsid(void);
    IFLS(SETSID);
    break;
  case 67 :
    // NB: Missing from syscalls.h
    // sys_sigaction 
    IFLSI(SIGACTION,EBX);
    break;
  case 68 :
    // long sys_sgetmask(void);
    IFLS(SGETMASK);
    break;
  case 69 : 
    // long sys_ssetmask(int newmask);
    IFLSI(SSETMASK,EBX);
    break;
  case 70 : 
    // long sys_setreuid(uid_t ruid, uid_t euid);
    IFLSII(SETREUID,EBX,ECX);
    break;
  case 71 :
    // long sys_setregid(gid_t rgid, gid_t egid);
    IFLSII(SETREGID,EBX,ECX);
    break;
  case 72 : 
    // NB: Missing from syscalls.h
    // sys_sigsuspend
    IFLS(SIGSUSPEND);
    break;
  case 73 : 
    // long sys_sigpending(old_sigset_t __user *set);
    IFLS(SIGPENDING);
    break;
  case 74 : 
    // long sys_sethostname(char __user *name, int len);
    {char *name;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	copy_string_phys(name, paddr, 120);
	IFLSSI(SETHOSTNAME,name,ECX);
      }
    }
    break;
  case 75 : 
    // long sys_setrlimit(unsigned int resource,
    //                    struct rlimit __user *rlim);
    IFLSII(SETRLIMIT,EBX,ECX);
    break;
  case 76 : // sys_getrlimit

HERE I AM!

    fprintf(logfile,"PID %3d (%16s)[sys_getrlimi 76]: resource %d\n", pid, command, EBX);
    break;
  case 77 : // sys_getrusage
    fprintf(logfile,"PID %3d (%16s)[sys_getrusag 77]: who %d\n", pid, command, EBX);
    break;
  case 78 : // sys_gettimeofday
    fprintf(logfile,"PID %3d (%16s)[sys_gettimeo 78]\n", pid, command);
    break;
  case 79 : // sys_settimeofday
    fprintf(logfile,"PID %3d (%16s)[sys_settimeo 79]\n", pid, command);
    break;
  case 80 : // sys_getgroups
    fprintf(logfile,"PID %3d (%16s)[sys_getgroup 80]\n", pid, command);
    break;
  case 81 : // sys_setgroups
    fprintf(logfile,"PID %3d (%16s)[sys_setgroup 81]\n", pid, command);
    break;
  case 82 : // sys_select
    {
      int fd, *ptr;
      fprintf(logfile,"PID %3d (%16s)[sys_select   82]: ", pid, command);
      if ( EBX > 0 ){
	if (!ECX) {
	  paddr = cpu_get_phys_page_debug(env, ECX);
	  fprintf(logfile, "readfds");
	  bzero(tempbuf, 120);
	  cpu_physical_memory_read(paddr, tempbuf, EBX*4); //-> get the fdset 
	  i=0; ptr = (int*)tempbuf; fd = *ptr++;
	  while (fd ) {
	    fprintf(logfile, " %d", fd);
	    i++;	
	    if ( i<EBX) fd = *ptr++;
	    else fd = 0;
	  }
	}
	if (!EDX) {
	  paddr = cpu_get_phys_page_debug(env, EDX);
	  fprintf(logfile, "writefds");
	  bzero(tempbuf, 120);
	  cpu_physical_memory_read(paddr, tempbuf, EBX*4); //-> get the fdset 
	  i=0; ptr = (int*)tempbuf; fd = *ptr++;
	  while (fd ) {
	    fprintf(logfile, " %d", fd);
	    i++;	
	    if ( i<EBX) fd = *ptr++;
	    else fd = 0;
	  }
	}
      }
      fprintf(logfile, "\n");
    }
    break;
  case 83 : // sys_symlink
    paddr = cpu_get_phys_page_debug(env, EBX);
    if (paddr!=-1)	{
      fprintf(logfile,"PID %3d (%16s)", pid, command);
      fprintf(logfile,"[sys_symlink  83]: ");
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
      fprintf(logfile,"%s -> ", tempbuf);

      paddr = cpu_get_phys_page_debug(env, ECX);
      if (paddr!=-1)	{
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, 120); 
	fprintf(logfile,"%s\n", tempbuf);
      }
    } 
    break;
  case 84 : // sys_oldlstat
    paddr = cpu_get_phys_page_debug(env, EBX);
    if (paddr!=-1)	{
      fprintf(logfile,"PID %3d (%16s)", pid, command);
      fprintf(logfile,"[sys_oldlstat 84]: ");
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
      fprintf(logfile,"%s\n", tempbuf);
    }
    break;
  case 85 : // sys_readlink
    paddr = cpu_get_phys_page_debug(env, EBX);
    if (paddr!=-1)	{
      fprintf(logfile,"PID %3d (%16s)", pid, command);
      fprintf(logfile,"[sys_readlink 85]: ");
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the link name
      fprintf(logfile,"%s\n", tempbuf);
    }
    break;
  case 86 : // sys_uselib
    paddr = cpu_get_phys_page_debug(env, EBX);
    if (paddr!=-1)	{
      fprintf(logfile,"PID %3d (%16s)", pid, command);
      fprintf(logfile,"[sys_uselib   86]: ");
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the lib name
      fprintf(logfile,"%s\n", tempbuf);
    }
    break;
  case 87 : // sys_swapon
    paddr = cpu_get_phys_page_debug(env, EBX);
    if (paddr!=-1)	{
      fprintf(logfile,"PID %3d (%16s)", pid, command);
      fprintf(logfile,"[sys_swapon   87]: ");
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
      fprintf(logfile,"%s\n", tempbuf);
    }
    break;
  case 88 : // sys_reboot
    fprintf(logfile,"PID %3d (%16s)[sys_reboot   88]\n", pid, command);
    break;
  case 89 : // sys_readdir
    fprintf(logfile,"PID %3d (%16s)[sys_readdir  89]: fd %d\n", pid, command, EBX);
    break;
  case 90 : // sys_mmap
    {
      int addr, size, fd, *ptr;
      fprintf(logfile,"PID %3d (%16s)[sys_mmap     90]: ", pid, command);
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
	bzero(tempbuf, 120);
	cpu_physical_memory_read(paddr, tempbuf, 24); //-> get the file name
	ptr = (int *)tempbuf;
	addr = *ptr++;
	len = *ptr++;
	ptr++; ptr++;
	fd = *ptr++;
	fprintf(logfile,"addr 0x%08x; len 0x%08x; fd; %d\n", addr,  len, fd);
      }
    }
    break;
  case 91 : // sys_munmap
    fprintf(logfile,"PID %3d (%16s)[sys_munmap   91]: addr 0x%08x; len 0x%08x\n", pid, command, EBX, ECX);
    break;
  case 92 : // sys_truncate
    fprintf(logfile,"PID %3d (%16s)[sys_truncate 92]: ", pid, command);
    paddr = cpu_get_phys_page_debug(env, EBX);
    bzero(tempbuf, 120);
    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the link name
    fprintf(logfile,"file %s; length %d\n", tempbuf, ECX);
    break;
  case 93 : // sys_ftruncate
    fprintf(logfile,"PID %3d (%16s)[sys_ftruncat 93]: fd %d; length %d\n", pid, command, EBX, ECX);
    break;
  case 94 : // sys_fchmod
    fprintf(logfile,"PID %3d (%16s)[sys_fchmod   94]: fd %d; mode %d\n", pid, command, EBX, ECX);
    break;
  case 95 : // sys_fchown
    fprintf(logfile,"PID %3d (%16s)[sys_fchown16 95]: fd %d; uid %d; group %d\n", pid, command, EBX, ECX, EDX);
    break;
  case 96 : // sys_getpriority
    fprintf(logfile,"PID %3d (%16s)[sys_getprior 96]: which %d; who %d\n", pid, command, EBX, ECX);
    break;
  case 97 : // sys_setpriority
    fprintf(logfile,"PID %3d (%16s)[sys_setprior 97]: which %d; who %d\n", pid, command, EBX, ECX);
    break;
  case 98 : // sys_profil
    fprintf(logfile,"PID %3d (%16s)[sys_ni_sysc  98]\n", pid, command);
    break;
  case 99 : // sys_statfs
    fprintf(logfile,"PID %3d (%16s)[sys_statfs   99]", pid, command);
    paddr = cpu_get_phys_page_debug(env, EBX);
    if (paddr!=-1)	{
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
      fprintf(logfile,"file %s\n", tempbuf);
    }
    break;
  case 100 : // sys_fstatfs
    fprintf(logfile,"PID %3d (%16s)[sys_fstatfs 100]: fd %d\n", pid, command, EBX);
    break;
  case 101 : // sys_ioperm
    fprintf(logfile,"PID %3d (%16s)[sys_ioperm  101]: from %d; num %d; turn_on %d\n", 
	    pid, command, EBX, ECX, EDX);
    break;
  case 102 : // sys_socketcall
    switch (EBX) {
    case 1: // socket
      { int family, type, protocol, *ptr;
	fprintf(logfile,"PID %3d (%16s)[sys_socket  102]%d: ", pid, command, EBX);
	paddr = cpu_get_phys_page_debug(env, ECX);
	if (paddr!=-1)	{
	  bzero(tempbuf, 120);
	  cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	  ptr = (int*) tempbuf;
	  family = *ptr++; type =*ptr++; protocol = *ptr++;
	  fprintf(logfile,"family %2d; type %d; protocol %d\n", family, type, protocol);
	}

      }
      break;
    case 2: // bind
      { int fd, len, group, *ptr; struct sockaddr_in *sap;
	unsigned char b0, b1, b2, b3, *bptr, b4, b5, b6, b7; 
	unsigned short *sptr;
	unsigned char pkttype, halen, sll_addr[8];

	fprintf(logfile,"PID %3d (%16s)[sys_bind    102]%d: ", pid, command, EBX);

	paddr = cpu_get_phys_page_debug(env, ECX);
	if (paddr!=-1)	{
	  bzero(tempbuf, 120);
	  cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
	  ptr = (int*) tempbuf;
	  fd = *ptr++; sap=*ptr++; len= *ptr++;
	  paddr = cpu_get_phys_page_debug(env, sap);
	  bzero(tempbuf, 120);
	  cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the args
	  //bptr = (unsigned char*)&sa.sin_addr.s_addr;
	  bptr = tempbuf;
	  b0=*bptr++; b1=*bptr++; b2=*bptr++; b3=*bptr++;
	  b4=*bptr++; b5=*bptr++; b6=*bptr++; b7=*bptr++;
	  bptr = tempbuf; bptr+=2;
	  sap = (struct sockaddr_in*) tempbuf;
	  ptr = (int*) tempbuf; ptr++; pid =*ptr++;  group = *ptr++;
	  if (sap->sin_family == 1 ) // PF_LOCAL
	    fprintf(logfile,"family  1; fd %d; file %s", fd, bptr);
	  else if (sap->sin_family == 2 ) // PF_INET
	    fprintf(logfile,"family  2; fd %d; %d.%d.%d.%d:%d", 
		    fd, b4, b5, b6, b7, b2*256+b3);
	  else if (sap->sin_family == 16 ) // PF_NETLINK
	    fprintf(logfile,"family 16; fd %d; pid %d; group %d", 
		    fd, pid, group);
	  else if (sap->sin_family == 17 )  {// PF_PACKET
	    bptr = tempbuf; bptr += 8; sptr = (unsigned short*)bptr;
	    bptr +=2; pkttype =*bptr; halen =*bptr++;
	    fprintf(logfile,"family 17; fd %d; protocol 0x%x; ifindex 0x%x",
		    fd, b2*256+b3, pid);
*/
/*
					     	fprintf(logfile,"family 17; fd %d; protocol %d; ifindex %d; hatype %d; pkttype %d; halen %d; addr %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x", 
						sap->sin_family, fd, b2*256+b3, pid, *sptr, pkttype, halen, 
						tempbuf[12], tempbuf[13], tempbuf[14], tempbuf[15],
						tempbuf[16], tempbuf[17], tempbuf[18], tempbuf[19]);
*/
/*					    }else 
					     	fprintf(logfile,"family %2d; fd %d; %d.%d.%d.%d-%d.%d.%d.%d(%c%c%c%c%c%c%c%c); len %d", 
						sap->sin_family, fd, b0, b1, b2, b3,b4, b5, b6, b7, b0, b1, b2, b3,b4, b5, b6, b7, len);
					}
					fprintf(logfile,"\n");
					}
					break;
				case 3: // connect
					{ int fd, len, *ptr; struct sockaddr_in *sap;
					unsigned char b0, b1, b2, b3, *bptr, b4, b5, b6, b7; 

                                	fprintf(logfile,"PID %3d (%16s)[sys_connect 102]%d: ", pid, command, EBX);

                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    ptr = (int*) tempbuf;
					    fd = *ptr++; sap=*ptr++; len= *ptr++;
                    			    paddr = cpu_get_phys_page_debug(env, sap);
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the args

					    bptr = tempbuf;
					    b0=*bptr++; b1=*bptr++; b2=*bptr++; b3=*bptr++;
					    b4=*bptr++; b5=*bptr++; b6=*bptr++; b7=*bptr++;
					    bptr = tempbuf; bptr+=2;
					    sap = (struct sockaddr_in*) tempbuf;
					    if (sap->sin_family == 1 ) // PF_LOCAL
					     	fprintf(logfile,"socket %d; family  1; file %s", fd, bptr);
					    else if (sap->sin_family == 2 ) // PF_INET
					     	fprintf(logfile,"socket %d; family  2; %d.%d.%d.%d:%d", 
							fd, b4, b5, b6, b7, b2*256+b3);
					    else
					    	fprintf(logfile,"socket %d; family %2d;",
								fd, sap->sin_family);
					}
					fprintf(logfile,"\n");
					}
					break;
				case 4: // listen
                                	fprintf(logfile,"PID %3d (%16s)[sys_listen  102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    fprintf(logfile,"socket %d", *(int*) tempbuf);
					}
					fprintf(logfile,"\n");
					break;
				case 5: // accept
                                	fprintf(logfile,"PID %3d (%16s)[sys_accept  102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    fprintf(logfile,"socket %d", *(int*) tempbuf);
					}
					fprintf(logfile,"\n");

					break;
				case 6: // getsockname
                                	fprintf(logfile,"PID %3d (%16s)[sys_getskna 102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    fprintf(logfile,"socket %d", *(int*) tempbuf);
					}
					fprintf(logfile,"\n");

					break;
				case 7: // getpeername
                                	fprintf(logfile,"PID %3d (%16s)[sys_getpeer 102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    fprintf(logfile,"socket %d", *(int*) tempbuf);
					}
					fprintf(logfile,"\n");

					break;
				case 8: {// socketpair
					int domain, type, protocol, *socket_vector, *ptr;
                                	fprintf(logfile,"PID %3d (%16s)[sys_skpair  102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    ptr = (int *) tempbuf;
				  	    domain =*ptr++; type=*ptr++; protocol=*ptr++; socket_vector=*ptr++;
					    fprintf(logfile,"domain %d; type %d; protocol %d\n", *(int*) tempbuf);
					}

					}
					break;
				case 9: // send
					{
					int fd, msg, len, *ptr;

                                	fprintf(logfile,"PID %3d (%16s)[sys_send    102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    ptr = (int*) tempbuf;
					    fd = *ptr++; msg=*ptr++; len= *ptr++;
					    fprintf(logfile,"socket %d ", fd);

					    bzero(tempbuf, 120);
                    			    paddr = cpu_get_phys_page_debug(env, msg);
					    cpu_physical_memory_read(paddr, tempbuf, 30); 
					    for (i=0; i<30; i++)
						if (iscntrl(tempbuf[i])) tempbuf[i]='.';
					    fprintf(logfile,"(%s)(%d)", tempbuf, len);

					}
					fprintf(logfile,"\n");

					}
					break;
				case 10: // recv
					{
					int fd, len, *ptr;
					unsigned int msg;

                                	fprintf(logfile,"PID %3d (%16s)[sys_recv    102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    ptr = (int*) tempbuf;
					    fd = *ptr++; msg=*ptr++; len= *ptr;
					    fprintf(logfile,"socket %d, msg 0x%08x\n", fd, msg);
					}

					}
					break;
				case 11: // sendto
					{
					int fd,msg, len, *ptr; struct sockaddr_in *sap;
					unsigned char b0, b1, b2, b3, *bptr, b4, b5, b6, b7;

                                	fprintf(logfile,"PID %3d (%16s)[sys_sendto  102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    ptr = (int*) tempbuf;
					    fd = *ptr++; msg=*ptr++; len= *ptr++; ptr++; sap=(struct sockaddr_in *)*ptr;
					    fprintf(logfile,"socket %d --> ", fd);

					    bzero(tempbuf, 120);
                    			    paddr = cpu_get_phys_page_debug(env, sap);
					    cpu_physical_memory_read(paddr, tempbuf, 120); 
					    bptr = tempbuf;
					    b0=*bptr++; b1=*bptr++; b2=*bptr++; b3=*bptr++;
					    b4=*bptr++; b5=*bptr++; b6=*bptr++; b7=*bptr++;
					    bptr = tempbuf; bptr+=2;

					    sap = (struct sockaddr_in*) tempbuf;
					    if (sap->sin_family == 1 ) // PF_LOCAL
					     	fprintf(logfile,"[dest: family  1; file %s]", bptr);
					    else if (sap->sin_family == 2 ) // PF_INET
					     	fprintf(logfile,"[dest: family  2; %d.%d.%d.%d:%d]", 
							b4, b5, b6, b7, b2*256+b3);
					    else
					    	fprintf(logfile,"[dest: family %2d]", sap->sin_family);

					    bzero(tempbuf, 120);
                    			    paddr = cpu_get_phys_page_debug(env, msg);
					    cpu_physical_memory_read(paddr, tempbuf, 30); 
					    for (i=0; i<30; i++)
						if (iscntrl(tempbuf[i])) tempbuf[i]='.';
					    fprintf(logfile," (%s)(%d)", tempbuf, len);

					}
					fprintf(logfile,"\n");

					}
					break;
				case 12: // recvfrom
					{
					int fd, *ptr;

                                	fprintf(logfile,"PID %3d (%16s)[sys_recvfro 102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    ptr = (int*) tempbuf;
					    fd = *ptr++; 
					    fprintf(logfile,"socket %d  ", fd);
					}
					fprintf(logfile,"\n");

					}
					break;
				case 13: // shutdown
					{
					int fd, *ptr;

                                	fprintf(logfile,"PID %3d (%16s)[sys_shutdow 102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    ptr = (int*) tempbuf;
					    fd = *ptr++; 
					    if (*ptr == 0 )
					    	fprintf(logfile,"socket %d (SHUT_RD)",   fd);
					    else if (*ptr == 1 )
					    	fprintf(logfile,"socket %d (SHUT_WR)",   fd);
					    else if (*ptr == 2 )
					    	fprintf(logfile,"socket %d (SHUT_RDWR)", fd);
					}
					fprintf(logfile,"\n");
					}
					break;
				case 14: // setsockopt
					{
					int fd, level, option, *ptr;

                                	fprintf(logfile,"PID %3d (%16s)[sys_setskop 102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    ptr = (int*) tempbuf;
					    fd = *ptr++; level=*ptr++; option=*ptr++;
					    fprintf(logfile,"socket %d; level %d; option %d", fd, level, option);
					}
					fprintf(logfile,"\n");
					}
					break;
				case 15: // getsockopt
					{
					int fd, level, option, *ptr;

                                	fprintf(logfile,"PID %3d (%16s)[sys_getskop 102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    ptr = (int*) tempbuf;
					    fd = *ptr++; level=*ptr++; option=*ptr++;
					    fprintf(logfile,"socket %d; level %d; option %d", fd, level, option);
					}
					fprintf(logfile,"\n");
					}
					break;
				case 16: // sendmsg
					{
					int fd, *ptr;

                                	fprintf(logfile,"PID %3d (%16s)[sys_sendmsg 102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    ptr = (int*) tempbuf;
					    fd = *ptr++; 
					    fprintf(logfile,"socket %d", fd);
					}
					fprintf(logfile,"\n");
					}
					break;
				case 17: // recvmsg
					{
					int fd, *ptr;

                                	fprintf(logfile,"PID %3d (%16s)[sys_recvmsg 102]%d: ", pid, command, EBX);
                    			paddr = cpu_get_phys_page_debug(env, ECX);
					if (paddr!=-1)	{
					    bzero(tempbuf, 120);
					    cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
					    ptr = (int*) tempbuf;
					    fd = *ptr++; 
					    fprintf(logfile,"socket %d", fd);
					}
					fprintf(logfile,"\n");
					}
					break;
				}
                                break;
                        case 103 : // sys_syslog
				{
				int size=30;
                    		paddr = cpu_get_phys_page_debug(env, ECX);
                                fprintf(logfile,"PID %3d (%16s)[sys_syslog  103]: \n", pid, command);
				bzero(tempbuf, 120);
				if (EDX>0 && EDX <= 30) size = EDX; 
                        	cpu_physical_memory_read(paddr, tempbuf, size); 
 	                        for (i=0; i<size; i++)
                                	if (iscntrl(tempbuf[i])) tempbuf[i]='.';
				fprintf(logfile,"(%s)(%d)\n", tempbuf, EDX);

				}
                                break;
                        case 104 : // sys_setitimer
                                fprintf(logfile,"PID %3d (%16s)[sys_setitim 104]: which %d\n", pid, command, EBX);
                                break;
                        case 105 : // sys_getitimer
                                fprintf(logfile,"PID %3d (%16s)[sys_getitim 105]: which %d\n", pid, command, EBX);
                                break;
                        case 106 : // sys_stat
                                fprintf(logfile,"PID %3d (%16s)[sys_stat    106]: ", pid, command);
                                paddr = cpu_get_phys_page_debug(env, EBX);
                                bzero(tempbuf, 120);
                                cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
                                fprintf(logfile,"%s \n", tempbuf);
                                break;
                        case 107 : // sys_lstat
                                fprintf(logfile,"PID %3d (%16s)[sys_lstat   107]: ", pid, command);
                                paddr = cpu_get_phys_page_debug(env, EBX);
                                bzero(tempbuf, 120);
                                cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
                                fprintf(logfile,"%s \n", tempbuf);
                                break;
                        case 108 : // sys_fstat
                                fprintf(logfile,"PID %3d (%16s)[sys_fstat   108]: fd %d", pid, command, EBX);
                                break;
                        case 109 : // sys_olduname
                                fprintf(logfile,"PID %3d (%16s)[sys_oldunam 109]\n", pid, command);
                                break;
                        case 110 : // sys_iopl
                                fprintf(logfile,"PID %3d (%16s)[sys_iopl    110]: level\n", pid, command, EBX);
                                break;
                        case 111 : // sys_vhangup
                                fprintf(logfile,"PID %3d (%16s)[sys_vhangup 111]\n", pid, command);
                                break;
                        case 112 : // sys_idle
                                fprintf(logfile,"PID %3d (%16s)[sys_idle    112]\n", pid, command);
                                break;
                        case 113 : // sys_vm86old
                                fprintf(logfile,"PID %3d (%16s)[sys_vm86old 113]\n", pid, command);
                                break;
                        case 114 : // sys_wait4
                                fprintf(logfile,"PID %3d (%16s)[sys_wait4   114]: pid %d\n", pid, command, EBX);
                                break;
                        case 115 : // sys_swapoff
                                fprintf(logfile,"PID %3d (%16s)[sys_swapoff 115]: ", pid, command);
                    		paddr = cpu_get_phys_page_debug(env, EBX);
				bzero(tempbuf, 120);
                        	cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				fprintf(logfile,"%s\n", tempbuf);
                                break;
                        case 116 : // sys_sysinfo
                                fprintf(logfile,"PID %3d (%16s)[sys_sysinfo 116]\n", pid, command);
                                break;
                        case 117 : // sys_ipc  
				// call first second third ptr fifth
				//->EBX  ECX  EDX   ESI   EDI  EBP  
                                fprintf(logfile,"PID %3d (%16s)[sys_ipc     117]: ", pid, command);
				switch (EBX){
				case 1: // SEMOP
					fprintf(logfile, "SEMOP sem_id %d, sembuf 0x%08x, nsops %d\n", ECX, EDI, EDX);
					break;
				case 2: // SEMGET
					fprintf(logfile, "SEMGET key %d, nsems %d, semflags %d\n", ECX, EDX, ESI);
					break;
				case 3: // SEMCTL
					fprintf(logfile, "SEMCTL sem_id %d, sem_num %d, cmd %d arg 0x%08x\n", ECX, EDX, ESI, EDI);
					break;
				case 11: // MSGSND
					fprintf(logfile, "MSGSND msg_id %d, msg 0x%08x, size %d, flag %d\n", 
						ECX, EDI, EDX, ESI);
					break;
				default:
					fprintf(logfile, "call %d, first %d, second %d, third %d, ptr 0x%08x, fifth %d\n", 
						EBX, ECX, EDX, ESI, EDI, EBP);
					break;
				}
                                break;
                        case 118 : // sys_fsync
                                fprintf(logfile,"PID %3d (%16s)[sys_fsync   118]: fd %d\n", pid, command, EBX);
                                break;
                        case 119 : // sys_sigreturn
                                fprintf(logfile,"PID %3d (%16s)[sys_sigretu 119]\n", pid, command);
                                break;
                        case 120 : // sys_clone
                                fprintf(logfile,"PID %3d (%16s)[sys_clone   120]\n", pid, command);
                                break;
                        case 121 : // sys_setdomainname
                                fprintf(logfile,"PID %3d (%16s)[sys_setdoma 121]\n", pid, command);
                    		paddr = cpu_get_phys_page_debug(env, EBX);
				bzero(tempbuf, 120);
                        	cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				fprintf(logfile,"%s\n", tempbuf);
                                break;
                        case 122 : // sys_uname
                                fprintf(logfile,"PID %3d (%16s)[sys_uname   122]\n", pid, command);
                                break;
                        case 123 : // sys_modify_ldt
                                fprintf(logfile,"PID %3d (%16s)[sys_modify_ 123]\n", pid, command);
                                break;
                        case 124 : // sys_adjtimex
                                fprintf(logfile,"PID %3d (%16s)[sys_adjtime 124]\n", pid, command);
                                break;
                        case 125 : // sys_mprotect
                                fprintf(logfile,"PID %3d (%16s)[sys_mprotec 125]: addr 0x%08x; len 0x%08x; prot %d\n", 
					pid, command, EBX, ECX, EDX);
                                break;
                        case 126 : // sys_sigprocmask
                                fprintf(logfile,"PID %3d (%16s)[sys_sigproc 126]: how %d; set 0x%08x\n", pid, command, EBX, ECX);
                                break;
                        case 127 : // sys_create_module
                                fprintf(logfile,"PID %3d (%16s)[sys_crt_mod 127]: module ", pid, command);
                    		paddr = cpu_get_phys_page_debug(env, EBX);
				bzero(tempbuf, 120);
                        	cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				fprintf(logfile,"%s\n", tempbuf);
                                break;
                        case 128 : // sys_init_module
                                fprintf(logfile,"PID %3d (%16s)[sys_ini_mod 128]: module ", pid, command);
                    		paddr = cpu_get_phys_page_debug(env, EBX);
				bzero(tempbuf, 120);
                        	cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				fprintf(logfile,"%s\n", tempbuf);
                                break;
                        case 129 : // sys_delete_module
                                fprintf(logfile,"PID %3d (%16s)[sys_del_mod 129]: module", pid, command);
                    		paddr = cpu_get_phys_page_debug(env, EBX);
				bzero(tempbuf, 120);
                        	cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				fprintf(logfile,"%s\n", tempbuf);
                                break;
                        case 130 : // sys_get_kernel_syms
                                fprintf(logfile,"PID %3d (%16s)[sys_getksym 130]\n", pid, command);
                                break;
                        case 131 : // sys_quotactl
                                fprintf(logfile,"PID %3d (%16s)[sys_quotact 131]: cmd %d; id %d\n", pid, command, EBX, EDX);
                                break;
                        case 132 : // sys_getpgid
                                fprintf(logfile,"PID %3d (%16s)[sys_getpgid 132]\n", pid, command);
                                break;
                        case 133 : // sys_fchdir
                                fprintf(logfile,"PID %3d (%16s)[sys_fchdir  133]: fd %d\n", pid, command, EBX);
                                break;
                        case 134 : // sys_bdflush
                                fprintf(logfile,"PID %3d (%16s)[sys_bdflush 134]: func %d\n", pid, command, EBX);
                                break;
                        case 135 : // sys_sysfs
                                fprintf(logfile,"PID %3d (%16s)[sys_sysfs   135]\n", pid, command);
                                break;
                        case 136 : // sys_personality
                                fprintf(logfile,"PID %3d (%16s)[sys_personal136]\n", pid, command);
                                break;
                        case 137 : // sys_afs_syscall
                                fprintf(logfile,"PID %3d (%16s)[sys_afs_sysc137]\n", pid, command);
                                break;
                        case 138 : // sys_setfsuid
                                fprintf(logfile,"PID %3d (%16s)[sys_setfsuid138]: fsuid %d\n", pid, command, EBX);
                                break;
                        case 139 : // sys_setfsgid
                                fprintf(logfile,"PID %3d (%16s)[sys_setfsgid139]: fsgid %d\n", pid, command, EBX);
                                break;
                        case 141 : // sys_getdents
                                fprintf(logfile,"PID %3d (%16s)[sys_getdents141]: fd %d\n", pid, command, EBX);
                                break;
                        case 143 : // sys_flock
                                fprintf(logfile,"PID %3d (%16s)[sys_flock   143]: fd %d; operation %d\n", pid, command, EBX,ECX);
                                break;
                        case 144 : // sys_msync
                                fprintf(logfile,"PID %3d (%16s)[sys_msync   144]: addr 0x%08x len %d flag %d\n", pid, command, EBX, ECX, EDX);
                                break;
                        case 145 : // sys_readv
                                fprintf(logfile,"PID %3d (%16s)[sys_readv   145]\n", pid, command);
                                break;
                        case 146 : // sys_writev
				{ int i, len; struct iovec *iovp; 
                                fprintf(logfile,"PID %3d (%16s)[sys_writev  146]: fd %d;", pid, command, EBX);
				if (EDX>0) {
				    paddr = cpu_get_phys_page_debug(env, ECX);
				    bzero(tempbuf, 120);
				    cpu_physical_memory_read(paddr, tempbuf, EDX*sizeof(struct iovec)); //-> get the iov list
				    iovp = (struct iov*) tempbuf;
				    for(i=0; i<EDX; i++, iovp++) {
				    	fprintf(logfile," iov[%d].base 0x%08x len %d;", i, iovp->iov_base, iovp->iov_len);
				    } // for
				    iovp = (struct iov*) tempbuf;
				    paddr = cpu_get_phys_page_debug(env, iovp->iov_base);
				    if (iovp->iov_len > 30) len = 30;	
				    	else len = iovp->iov_len;
				    bzero(tempbuf, 120); 
				    cpu_physical_memory_read(paddr, tempbuf, len); //-> get the iov list
				    for (i=0; i<30; i++)
					if (iscntrl(tempbuf[i])) tempbuf[i]='.';
				    fprintf(logfile," iov[0] (%s)\n", tempbuf);
				
				} // if
				} // case
                                break;
                        case 147 : // sys_getsid
                                fprintf(logfile,"PID %3d (%16s)[sys_getsid  147]\n", pid, command);
                                break;
                        case 148 : // sys_fdatasync
                                fprintf(logfile,"PID %3d (%16s)[sys_fdatasyn148]: fd %d\n", pid, command, EBX);
                                break;
                        case 150 : // sys_mlock
                                fprintf(logfile,"PID %3d (%16s)[sys_mlock   150]: addr 0x%08x; len %d\n", pid, command, EBX, ECX);
                                break;
                        case 151 : // sys_munlock
                                fprintf(logfile,"PID %3d (%16s)[sys_munlock 151]: addr 0x%08x; len %d\n", pid, command, EBX, ECX);
                                break;
                        case 152 : // sys_mlockall
                                fprintf(logfile,"PID %3d (%16s)[sys_mlockall152]: flag %d\n", pid, command, EBX);
                                break;
                        case 153 : // sys_munlockall
                                fprintf(logfile,"PID %3d (%16s)[sys_munlocka153]\n", pid, command);
                                break;
                        case 154 : // sys_sched_setparam
                                fprintf(logfile,"PID %3d (%16s)[sys_sch_setp154]: pid %d\n", pid, command, EBX);
                                break;
                        case 155 : // sys_sched_getparam
                                fprintf(logfile,"PID %3d (%16s)[sys_sch_getp155]: pid %d\n", pid, command, EBX);
                                break;
                        case 156 : // sys_sched_setscheduler
                                fprintf(logfile,"PID %3d (%16s)[sys_sch_sets156]: pid %d; policy %d\n", pid, command, EBX, ECX);
                                break;
                        case 157 : // sys_sched_getscheduler
                                fprintf(logfile,"PID %3d (%16s)[sys_sch_gets157]: pid %d\n", pid, command, EBX);
                                break;
                        case 158 : // sys_sched_yield
                                fprintf(logfile,"PID %3d (%16s)[sys_sch_yiel158]\n", pid, command);
                                break;
                        case 159 : // sys_sched_get_priority_max
                                fprintf(logfile,"PID %3d (%16s)[sys_sch_gpr 159]: policy %d\n", pid, command, EBX);
                                break;
                        case 160 : // sys_sched_get_priority_min
                                fprintf(logfile,"PID %3d (%16s)[sys_sch_gpr 160]: policy %d\n", pid, command, EBX);
                                break;
                        case 161 : // sys_sched_rr_get_interval
                                fprintf(logfile,"PID %3d (%16s)[sys_sch_rr  161]: pid %d\n", pid, command, EBX);
                                break;
                        case 162 : // sys_nanosleep
                                fprintf(logfile,"PID %3d (%16s)[sys_nanoslee162]\n", pid, command);
                                break;
                        case 163 : // sys_mremap
                                fprintf(logfile,"PID %3d (%16s)[sys_mremap  163]: oldaddr 0x%08x; len %d; new_addr 0x%08x; flags %d\n", 
					pid, command, EBX, ECX, EDX, ESI);
                                break;
                        case 164 : // sys_setresuid
                                fprintf(logfile,"PID %3d (%16s)[sys_setresu 164]: ruid %d; euid %d; suid %d;\n", pid, command, EBX, ECX, EDX);
                                break;
                        case 165 : // sys_getresuid
                                fprintf(logfile,"PID %3d (%16s)[sys_getresu 165]\n", pid, command);
                                break;
                        case 166 : // sys_vm86
                                fprintf(logfile,"PID %3d (%16s)[sys_vm86    166]\n", pid, command);
                                break;
                        case 167 : // sys_query_module
                                fprintf(logfile,"PID %3d (%16s)[sys_query_m 167]: module ", pid, command);
                    		paddr = cpu_get_phys_page_debug(env, EBX);
				bzero(tempbuf, 120);
                        	cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the mod name
				fprintf(logfile,"%s\n", tempbuf);
                                break;
                        case 168 : // sys_poll
				{ int i, len; struct pollfd *iovp; 
                                fprintf(logfile,"PID %3d (%16s)[sys_poll    168]: nfd %d; timeout %d; ", 
					pid, command, ECX, EDX);
				if (ECX>0) {
				    paddr = cpu_get_phys_page_debug(env, EBX);
				    bzero(tempbuf, 120);
				    cpu_physical_memory_read(paddr, tempbuf, ECX*sizeof(struct pollfd)); 
				    iovp = (struct pollfd*) tempbuf;
				    for(i=0; i<ECX; i++, iovp++) {
				    	fprintf(logfile," fds[%d].fd %d (events %d);", i, iovp->fd, iovp->events);
				    } // for
				} // if
				fprintf(logfile,"\n");
				} // case
                                break;
                        case 169 : // sys_nfsservctl
                                fprintf(logfile,"PID %3d (%16s)[sys_nfsserv 169]\n", pid, command);
                                break;
                        case 170 : // sys_setresgid
                                fprintf(logfile,"PID %3d (%16s)[sys_setresg 170]: ruid %d; euid %d; suid %d;\n", pid, command, EBX, ECX, EDX);
                                break;
                        case 171 : // sys_getresgid
                                fprintf(logfile,"PID %3d (%16s)[sys_getresg 171]\n", pid, command);
                                break;
                        case 172 : // sys_prctl
                                fprintf(logfile,"PID %3d (%16s)[sys_prctl   172]\n", pid, command);
                                break;
                        case 173 : // sys_rt_sigreturn
                                fprintf(logfile,"PID %3d (%16s)[sys_rt_sigr 173]\n", pid, command);
                                break;
                        case 174 : // sys_rt_sigaction
                                fprintf(logfile,"PID %3d (%16s)[sys_rt_siga 174]: sig %d; handler 0x%08x\n", 
					pid, command, EBX, ECX);
                                break;
                        case 175 : // sys_rt_sigprocmask
                                fprintf(logfile,"PID %3d (%16s)[sys_rt_mask 175]: how %d; set 0x%08x\n", 
					pid, command, EBX, ECX);
                                break;
                        case 176 : // sys_rt_sigpending
                                fprintf(logfile,"PID %3d (%16s)[sys_rt_pend 176]\n", pid, command);
                                break;
                        case 177 : // sys_rt_sigtimedwait
                                fprintf(logfile,"PID %3d (%16s)[sys_rt_sigt 177]\n", pid, command);
                                break;
                        case 178 : // sys_rt_sigqueueinfo
                                fprintf(logfile,"PID %3d (%16s)[sys_rt_sigq 178]\n", pid, command);
                                break;
                        case 179 : // sys_rt_sigsuspend
                                fprintf(logfile,"PID %3d (%16s)[sys_rt_sigs 179]\n", pid, command);
                                break;
                        case 180 : // sys_pread64
                                fprintf(logfile,"PID %3d (%16s)[sys_pread64 180]: fd %d; count %d; offset %d\n", pid, command, EBX, EDX, ESI);
                                break;
                        case 181 : // sys_pwrite64
                                fprintf(logfile,"PID %3d (%16s)[sys_pwrit64 181]: fd %d; count %d; offset %d\n", pid, command, EBX, EDX, ESI);
                                break;
                        case 182 : // sys_chown
                                fprintf(logfile,"PID %3d (%16s)[sys_chown   182]\n", pid, command);
                    		paddr = cpu_get_phys_page_debug(env, EBX);
				bzero(tempbuf, 120);
                        	cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				fprintf(logfile,"%s (uid %d, gid %d)\n", tempbuf, ECX, EDX);
                                break;
                        case 183 : // sys_getcwd
                                fprintf(logfile,"PID %3d (%16s)[sys_getcwd  183]\n", pid, command);
                                break;
                        case 184 : // sys_capget
                                fprintf(logfile,"PID %3d (%16s)[sys_capget  184]\n", pid, command);
                                break;
                        case 185 : // sys_capset
                                fprintf(logfile,"PID %3d (%16s)[sys_capset  185]\n", pid, command);
                                break;
                        case 186 : // sys_sigaltstack
                                fprintf(logfile,"PID %3d (%16s)[sys_sigalts 186]\n", pid, command);
                                break;
                        case 187 : // sys_sendfile
                                fprintf(logfile,"PID %3d (%16s)[sys_sendfil 187]: out-fd %d; in-fd %d\n", pid, command, EBX, ECX);
                                break;
                        case 188 : // sys_getpmsg
                                fprintf(logfile,"PID %3d (%16s)[sys_getpmsg 188]\n", pid, command);
                                break;
                        case 189 : // sys_putpmsg
                                fprintf(logfile,"PID %3d (%16s)[sys_putpmsg 189]\n", pid, command);
                                break;
                        case 190 : // sys_vfork
                                fprintf(logfile,"PID %3d (%16s)[sys_vfork   190]\n", pid, command);
                                break;
                        case 191 : // sys_getrlimit
                                fprintf(logfile,"PID %3d (%16s)[sys_getrlim 191]: resource %d\n", pid, command, EBX);
                                break;
                        case 192 : // sys_mmap2
                                fprintf(logfile,"PID %3d (%16s)[sys_mmap2   192]: addr 0x%08x, len 0x%08x, prot %d, flags %d, fd %d, offset 0x%08x\n", pid, command, EBX, ECX, EDX, ESI, EDI, EBP);
                                break;
                        case 193 : // sys_truncate64
                                fprintf(logfile,"PID %3d (%16s)[sys_trunc64 193]\n", pid, command);
                    		paddr = cpu_get_phys_page_debug(env, EBX);
				bzero(tempbuf, 120);
                        	cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the link name
				fprintf(logfile,"file %s; length %d\n", tempbuf, ECX);
                                break;
                        case 194 : // sys_ftruncate64
                                fprintf(logfile,"PID %3d (%16s)[sys_ftrun64 194]: fd %d; length %d\n", pid, command, EBX, ECX);
                                break;
                        case 195 : // sys_stat64
                                //fprintf(logfile,"PID %3d (%16s)[sys_stat64  195]\n", pid, command);
                                paddr = cpu_get_phys_page_debug(env, EBX);
                                if (paddr!=-1)  {
                                    fprintf(logfile,"PID %3d (%16s)", pid, command);
                                    fprintf(logfile,"[sys_stat64  195]: ");
                                    bzero(tempbuf, 120);
                                    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
                                    fprintf(logfile,"%s \n", tempbuf);
                                }
                                break;
                        case 196 : // sys_lstat64
                                paddr = cpu_get_phys_page_debug(env, EBX);
                                if (paddr!=-1)  {
                                    fprintf(logfile,"PID %3d (%16s)", pid, command);
                                    fprintf(logfile,"[sys_lstat64 196]: ");
                                    bzero(tempbuf, 120);
                                    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
                                    fprintf(logfile,"%s \n", tempbuf);
                                }
                                break;
                        case 197 : // sys_fstat64
                                //fprintf(logfile,"PID %3d (%16s)[sys_fstat64 197]\n", pid, command);
                                paddr = cpu_get_phys_page_debug(env, EBX);
                                if (paddr!=-1)  {
                                    fprintf(logfile,"PID %3d (%16s)", pid, command);
                                    fprintf(logfile,"[sys_fstat64 108]: ");
                                    bzero(tempbuf, 120);
                                    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
                                    fprintf(logfile,"%s \n", tempbuf);
                                }

                                break;
                        case 198 : // sys_lchown32
                                fprintf(logfile,"PID %3d (%16s)[sys_lchow32 198]\n", pid, command);
                    		paddr = cpu_get_phys_page_debug(env, EBX);
				bzero(tempbuf, 120);
                        	cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				fprintf(logfile,"%s (uid %d, gid %d)\n", tempbuf, ECX, EDX);
                                break;
                        case 199 : // sys_getuid32
                                fprintf(logfile,"PID %3d (%16s)[sys_getuid3 199]\n", pid, command);
                                break;
                        case 200 : // sys_getgid32
                                fprintf(logfile,"PID %3d (%16s)[sys_getgid3 200]\n", pid, command);
                                break;
                        case 201 : // sys_geteuid32
                                fprintf(logfile,"PID %3d (%16s)[sys_geteu32 201]\n", pid, command);
                                break;
                        case 202 : // sys_getegid32
                                fprintf(logfile,"PID %3d (%16s)[sys_geteg32 202]\n", pid, command);
                                break;
                        case 203 : // sys_setreuid32
                                fprintf(logfile,"PID %3d (%16s)[sys_seteu32 203]: ruid %d; euid %d\n", pid, command, EBX, ECX);
                                break;
                        case 204 : // sys_setregid32
                                fprintf(logfile,"PID %3d (%16s)[sys_seteg32 204]: rgid %d; egid %d\n", pid, command, EBX, ECX);
                                break;
                        case 205 : // sys_getgroups32
                                fprintf(logfile,"PID %3d (%16s)[sys_getgr32 205]\n", pid, command);
                                break;
                        case 206 : // sys_setgroups32
                                fprintf(logfile,"PID %3d (%16s)[sys_setgr32 206]\n", pid, command);
                                break;
                        case 207 : // sys_fchown32
                                fprintf(logfile,"PID %3d (%16s)[sys_fchow32 207]: fd %d; uid %d; gid %d\n", pid, command, EBX, ECX, EDX);
                                break;
                        case 208 : // sys_setresuid32
                                fprintf(logfile,"PID %3d (%16s)[sys_setsu32 208]: ruid %d; euid %d; suid %d;\n", pid, command, EBX, ECX, EDX);
                                break;
                        case 209 : // sys_getresuid32
                                fprintf(logfile,"PID %3d (%16s)[sys_getes32 209]\n", pid, command);
                                break;
                        case 210 : // sys_setresgid32
                                fprintf(logfile,"PID %3d (%16s)[sys_setes32 208]: rgid %d; egid %d; sgid %d;\n", pid, command, EBX, ECX, EDX);
                                break;
                        case 211 : // sys_getresgid32
                                fprintf(logfile,"PID %3d (%16s)[sys_getes32 211]\n", pid, command);
                                break;
                        case 212 : // sys_chown32
                                fprintf(logfile,"PID %3d (%16s)[sys_chown32 212]: ", pid, command);
                    		paddr = cpu_get_phys_page_debug(env, EBX);
				bzero(tempbuf, 120);
                        	cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				fprintf(logfile,"%s (uid %d, gid %d)\n", tempbuf, ECX, EDX);
                                break;
                        case 213 : // sys_setuid32
                                fprintf(logfile,"PID %3d (%16s)[sys_setui32 213]: uid %d\n", pid, command, EBX);
                                break;
                        case 214 : // sys_setgid32
                                fprintf(logfile,"PID %3d (%16s)[sys_setgi32 214]: gid %d\n", pid, command, EBX);
                                break;
                        case 215 : // sys_setfsuid32
                                fprintf(logfile,"PID %3d (%16s)[sys_setfs32 215]: fsuid %d\n", pid, command, EBX);
                                break;
                        case 216 : // sys_setfsgid32
                                fprintf(logfile,"PID %3d (%16s)[sys_setfs32 216]: fsgid %d\n", pid, command, EBX);
                                break;
                        case 217 : // sys_pivot_root
                                fprintf(logfile,"PID %3d (%16s)[sys_pivot_r 217]: ", pid, command);
                    		paddr = cpu_get_phys_page_debug(env, EBX);
				bzero(tempbuf, 120);
                        	cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				fprintf(logfile,"%s\n", tempbuf);
                                break;
                        case 218 : // sys_mincore
                                fprintf(logfile,"PID %3d (%16s)[sys_mincore 218]\n", pid, command);
                                break;
                        case 219 : // sys_madvise
                                fprintf(logfile,"PID %3d (%16s)[sys_madvise 219]\n", pid, command);
                                break;
                        case 220 : // sys_getdents64
                                fprintf(logfile,"PID %3d (%16s)[sys_getde64 220]: fd %d\n", pid, command, EBX);
                                break;
                        case 221 : // sys_fcntl64
                                fprintf(logfile,"PID %3d (%16s)[sys_fcntl64 221]: fd %d; cmd %d; arg 0x%08x\n", 
					pid, command, EBX, ECX, EDX);
                                break;
                        case 224 : // sys_gettid
                                fprintf(logfile,"PID %3d (%16s)[sys_gettid  224]\n", pid, command);
                                break;
                        case 225 : // sys_readahead
                                fprintf(logfile,"PID %3d (%16s)[sys_readahe 225]\n", pid, command);
                                break;
                        case 226 : // sys_setxattr
                                fprintf(logfile,"PID %3d (%16s)[sys_setxatt 226]\n", pid, command);
                                break;
                        case 227 : // sys_lsetxattr
                                fprintf(logfile,"PID %3d (%16s)[sys_lsetxat 227]\n", pid, command);
                                break;
                        case 228 : // sys_fsetxattr
                                fprintf(logfile,"PID %3d (%16s)[sys_fsetxat 228]\n", pid, command);
                                break;
                        case 229 : // sys_getxattr
                                fprintf(logfile,"PID %3d (%16s)[sys_getxatt 229]\n", pid, command);
                                break;
                        case 230 : // sys_lgetxattr
                                fprintf(logfile,"PID %3d (%16s)[sys_lgetxat 230]\n", pid, command);
                                break;
                        case 231 : // sys_fgetxattr
                                fprintf(logfile,"PID %3d (%16s)[sys_fgetxat 231]\n", pid, command);
                                break;
                        case 232 : // sys_listxattr
                                fprintf(logfile,"PID %3d (%16s)[sys_listxat 232]\n", pid, command);
                                break;
                        case 233 : // sys_llistxattr
                                fprintf(logfile,"PID %3d (%16s)[sys_llistxa 233]\n", pid, command);
                                break;
                        case 234 : // sys_flistxattr
                                fprintf(logfile,"PID %3d (%16s)[sys_flistxa 234]\n", pid, command);
                                break;
                        case 235 : // sys_removexattr
                                fprintf(logfile,"PID %3d (%16s)[sys_removex 235]\n", pid, command);
                                break;
                        case 236 : // sys_lremovexattr
                                fprintf(logfile,"PID %3d (%16s)[sys_lremove 236]\n", pid, command);
                                break;
                        case 237 : // sys_fremovexattr
                                fprintf(logfile,"PID %3d (%16s)[sys_fremove 237]\n", pid, command);
                                break;
                        case 238 : // sys_tkill
				fprintf(logfile,"PID %3d (%16s)[sys_tkill   238]: pid %d; sig %d\n", pid, 
					command, EBX, ECX);
                                break;
                        case 239 : // sys_sendfile64
                                fprintf(logfile,"PID %3d (%16s)[sys_sendf64 239]\n", pid, command);
                                break;
                        case 240 : // sys_futex
                                fprintf(logfile,"PID %3d (%16s)[sys_futex   240]\n", pid, command);
                                break;
                        case 241 : // sys_sched_setaffinity
                                fprintf(logfile,"PID %3d (%16s)[sys_sch_saf 241]\n", pid, command);
                                break;
                        case 242 : // sys_sched_getaffinity
                                fprintf(logfile,"PID %3d (%16s)[sys_sch_gaf 242]\n", pid, command);
                                break;
                        case 243 : // sys_set_thread_area
                                fprintf(logfile,"PID %3d (%16s)[sys_set_thr 243]\n", pid, command);
                                break;
                        case 244 : // sys_get_thread_area
                                fprintf(logfile,"PID %3d (%16s)[sys_get_thr 244]\n", pid, command);
                                break;
                        case 245 : // sys_io_setup
                                fprintf(logfile,"PID %3d (%16s)[sys_io_setu 245]\n", pid, command);
                                break;
                        case 246 : // sys_io_destroy
                                fprintf(logfile,"PID %3d (%16s)[sys_io_detr 246]\n", pid, command);
                                break;
                        case 247 : // sys_io_getevents
                                fprintf(logfile,"PID %3d (%16s)[sys_io_gete 247]\n", pid, command);
                                break;
                        case 248 : // sys_io_submit
                                fprintf(logfile,"PID %3d (%16s)[sys_io_subm 248]\n", pid, command);
                                break;
                        case 249 : // sys_io_cancel
                                fprintf(logfile,"PID %3d (%16s)[sys_io_canc 249]\n", pid, command);
                                break;
                        case 250 : // sys_fadvise64
                                fprintf(logfile,"PID %3d (%16s)[sys_fadvi64 250]\n", pid, command);
                                break;
                        case 252 : // sys_exit_group
                                fprintf(logfile,"PID %3d (%16s)[sys_exit_gr 252]\n", pid, command);
                                break;
                        case 253 : // sys_lookup_dcookie
                                fprintf(logfile,"PID %3d (%16s)[sys_lkup_dc 253]\n", pid, command);
                                break;
                        case 254 : // sys_epoll_create
                                fprintf(logfile,"PID %3d (%16s)[sys_epollcr 254]\n", pid, command);
                                break;
                        case 255 : // sys_epoll_ctl
                                fprintf(logfile,"PID %3d (%16s)[sys_epollct 255]\n", pid, command);
                                break;
                        case 256 : // sys_epoll_wait
                                fprintf(logfile,"PID %3d (%16s)[sys_epoll_w 256]\n", pid, command);
                                break;
                        case 257 : // sys_remap_file_pages
                                fprintf(logfile,"PID %3d (%16s)[sys_remap_f 257]\n", pid, command);
                                break;
                        case 258 : // sys_set_tid_address
                                fprintf(logfile,"PID %3d (%16s)[sys_set_tid 258]\n", pid, command);
                                break;
                        case 259 : // sys_timer_create
                                fprintf(logfile,"PID %3d (%16s)[sys_timercr 259]\n", pid, command);
                                break;
*/
			default:
                                fprintf(logfile,"PID %3d (%16s)[unknown_syscall %d]\n", pid, command, EAX);
				break;
                    	}  //switch
