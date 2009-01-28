
// Note: 
// system call prototypes
// from  /usr/src/linux-headers-2.6.24-19/include/linux/syscalls.h
// Look in /usr/include/asm/unistd_32.h for the syscall numbers

// Note, from http://www.win.tue.nl/~aeb/linux/lk/lk-4.html
// "The Linux Kernel: Linux System Calls"
// On i386, the parameters of a system call are transported via registers. 
// The system call number goes into %eax, 
// The parameters are (%ebx, %ecx, %edx, %esi, %edi, %ebp)
// More than 6 param?  Not sure what happens.  


#include <string.h>
#include "exec.h"
#include "lookup_table.h"


#include "linux_task_struct_offsets.h"

extern struct CPUX86State *env;


// current_task is pointer to vm physical memory at which linux task structure is
// located. 
// slot_offset is where in task_struct to find this slot.
// slot_size is size of data for that slot, in bytes.
// dest is where to copy that data
void copy_task_struct_slot(char *current_task, uint32_t slot_offset,
                           uint32_t slot_size, char *dest) {
  uint32_t paddr;

  bzero(dest,slot_size);
  paddr = cpu_get_phys_page_debug(env, current_task+slot_offset);
  if (paddr != -1) {
    cpu_physical_memory_read(paddr, &current_task, slot_size);
  }
}

// copy string of length at most len-1
// from physical adddress phsaddr into buffer tempbuf
// null terminate
void copy_string_phys(char *tempbuf, char *physaddr, uint32_t len) {
  bzero(tempbuf, len);
  cpu_physical_memory_read(physaddr,tempbuf,len-1);
}


#define SYSOP(op) glue(INFO_FLOW_OP_SYS_,op)
     
// All syscalls iferret log entries containt this info.
#define IFLS_CORE(op)     \
  IFLW_PUT_OP(SYSOP(op)); \
  IFLW_PUT_STRING(command); \
  IFLW_PUT_UINT32_T(pid); \
  IFLW_PUT_UINT32_T(eip_for_callsite); 

// sys call with no args
#define IFLS(op) \
IF_WRAPPER ( \
  IFLS_CORE(op); \
)


// sys call with one arg -- an int
#define IFLS_I(op,val)	\
IF_WRAPPER ( \
  IFLS_CORE(op);  \
  IFLW_PUT_UINT32_T(val); \
) 

     // sys call with two args -- both ints
#define IFLS_II(op,val1,val2) \
IF_WRAPPER ( \
  IFLS_CORE(op);   \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_UINT32_T(val2); \
) 

     // sys call with three args -- all ints
#define IFLS_III(op,val1,val2,val3)  \
IF_WRAPPER ( \
  IFLS_CORE(op);   \
  IFLW_PUT_UINT32_T(val1); \
  IFLW_PUT_UINT32_T(val2); \
  IFLW_PUT_UINT32_T(val3); \
) 


     // sys call with one arg -- a string 
#define IFLS_S(op,val) \
IF_WRAPPER ( \
  IFLS_CORE(op);	       \
  IFLW_PUT_STRING(val);  \
)


     // sys call with two args -- both strings
#define IFLS_SS(op,val1,val2)			\
IF_WRAPPER ( \
  IFLS_CORE(op);	       \
  IFLW_PUT_STRING(val1);  \
  IFLW_PUT_STRING(val2);  \
)

     // sys call with two args -- a string and an int
#define IFLS_SI(op,val1,val2) \
IF_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_STRING(val1); \
  IFLW_PUT_UINT32_T(val2); \
) 

     // sys call with three args -- a string and two ints
#define IFLS_SII(op,val1,val2,val3)	\
IF_WRAPPER ( \
  IFLS_CORE(op); \
  IFLW_PUT_STRING(val1); \
  IFLW_PUT_UINT32_T(val2); \
  IFLW_PUT_UINT32_T(val3); \
) 


     // we use these over and over. 
#define IFLS_S_SIMP(op,r) \
{ \
  char name[120]; \
  paddr = cpu_get_phys_page_debug(env, r); \
  if (paddr!=-1) { \
    copy_string_phys(name, paddr, 120); \            
  } \
  IFLS_S(op,name); \
} 



#define IFLS_SS_SIMP(op,r1,r2)	\
{ \
  char name1[120], name2[120]; \
  paddr = cpu_get_phys_page_debug(env, r1); \
  if (paddr!=-1)	{ \
    copy_string_phys(name1, paddr, 120); \
    paddr = cpu_get_phys_page_debug(env, r2); \
    if (paddr!=-1)	{ \
      copy_string_phys(name2, paddr, 120);  \
      IFLS_SS(LINK,name1,name2); \
    } \
  } \
} 


#define IFLS_SI_SIMP(op,r1,r2)	\
{ \
  char name[120]; \
  paddr = cpu_get_phys_page_debug(env, r1); \
  if (paddr!=-1)	{ \
    copy_string_phys(name, paddr, 120); \
    IFLS_SI(op,name,r2); \
  }  \
} \


#define IFLS_SII_SIMP(op,r1,r2,r3)   \
{ \
  char name[120]; \
  paddr = cpu_get_phys_page_debug(env, r1); \
  if (paddr!=-1)	{ \
    copy_string_phys(name, paddr, 120);  \
    IFLS_SII(MKNOD,name,r2,r3); \
  } \
}


// write an entry to iferret log to capture
// context (eip & pid), number, and arguments of 
// current system call 
void iferret_log_syscall (uint8_t is_sysenter) {
  
  uint32_t paddr, regs_ebx; 
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
    eip_for_callsite = stack_val ;
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
    // sys_fork is missing from syscalls.h
    IFLS(FORK);
    break;
  case 3 : 
    // ssize_t sys_read(unsigned int fd, char __user *buf,
    //	                size_t count);
    IFLS_I(READ,EBX);
    break;
  case 4 :
    // ssize_t sys_write(unsigned int fd, const char __user *buf,
    //                   size_t count);
    IFLS_II(WRITE,EBX,EDX);
    break;
  case 5 :
    // long sys_open(const char __user *filename,
    //               int flags, int mode);
    IFLS_SII_SIMP(OPEN,EBX,ECX,EDX);
    break;
  case 6 :
    // long sys_close(unsigned int fd);
    IFLS_I(CLOSE,EBX);    
    break;
  case 7 :
    // long sys_waitpid(pid_t pid, int __user *stat_addr, int options);
    IFLS(WAITPID);
    break;
  case 8 : 
    // long sys_creat(const char __user *pathname, int mode);    
    IFLS_SI_SIMP(CREAT,EBX,ECX);
    break;
  case 9: 
    // long sys_link(const char __user *oldname,
    //	             const char __user *newname);
    IFLS_SS_SIMP(LINK,EBX,ECX);
    break;
  case 10 : 
    // long sys_unlink(const char __user *pathname);
    IFLS_S_SIMP(UNLINK,EBX);
    break;
  case 11: 
    // sys_execve is missing from syscalls.h 
    // This one is there, though? 
    // long sys_execve(char *name, char **argv,char **envp, struct pt_regs regs)
    {
      int i,n;
      char name[120], **orig_argvp, **argvp;

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
        IFLS_I(EXECVE,n);
        while (argvp) {
          paddr = cpu_get_phys_page_debug(env, argvp);	
          copy_string_phys(name, paddr, 120); // arg 
          IFLW_PUT_STRING(name);
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
    IFLS_S_SIMP(CHDIR,EBX);
    break;
  case 13 : 
    // long sys_time(time_t __user *tloc);
    IFLS(TIME);
    break;
  case 14 :
    // long sys_mknod(const char __user *filename, int mode,
    //	              unsigned dev);
    IFLS_SII_SIMP(MKNOD,EBX,ECX,EDX);
    break;
  case 15 :
    // long sys_chmod(const char __user *filename, mode_t mode);
    IFLS_SI_SIMP(MKNOD,EBX,ECX);
    break;
  case 16 : 
    // long sys_lchown16(const char __user *filename,
    //                   old_uid_t user, old_gid_t group);
    IFLS_SII_SIMP(LCHOWN16,EBX,ECX,EDX);
    break;
  case 17 : // sys_ni_syscall
    IFLS(NI_SYSCALL_17);
    break;
  case 18 :
    // long sys_stat(char __user *filename,
    //               struct __old_kernel_stat __user *statbuf);
    IFLS_S_SIMP(STAT,EBX);
    break;
  case 19 :
    // off_t sys_lseek(unsigned int fd, off_t offset,
    //                 unsigned int origin);
    IFLS_III(LSEEK,EBX,ECX,EDX);
    break;
  case 20 :
    // long sys_getpid(void);
    IFLS(GETPID);
    break;
  case 21: 
    // long sys_mount(char __user *dev_name, char __user *dir_name,
    //                char __user *type, unsigned long flags,
    //	              void __user *data);
    IFLS_SS_SIMP(MOUNT,EBX,ECX);
    break;
  case 22 :
    // long sys_oldumount(char __user *name);
    IFLS_S_SIMP(OLDUMOUNT,EBX);
    break;
  case 23 : 
    // long sys_setuid16(old_uid_t uid);
    IFLS_I(SETUID16,EBX);
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
    IFLS_III(PTRACE,EBX,ECX,EDX);
    break;
  case 27 :
    // unsigned long sys_alarm(unsigned int seconds);
    IFLS_I(ALARM,EBX);
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
    IFLS_SI_SIMP(UTIME,EBX,ECX);
    break;
  case 31 : // sys_ni_syscall
    IFLS(NI_SYSCALL_31);
    break;
  case 32 : // sys_ni_syscall
    IFLS(NI_SYSCALL_32);
    break;
  case 33 :
    // long sys_access(const char __user *filename, int mode);
    IFLS_SI_SIMP(ACCESS,EBX,ECX);
    break;
  case 34 :
    // long sys_nice(int increment);
    IFLS_I(NICE,EBX);
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
    IFLS_II(KILL,EBX,ECX);
    break;
  case 38: 
    //  long sys_rename(const char __user *oldname,
    //	                const char __user *newname);
    IFLS_SS_SIMP(RENAME,EBX,ECX);
    break;
  case 39: 
    // long sys_mkdir(const char __user *pathname, int mode);
    IFLS_SI_SIMP(MKDIR,EBX,ECX);
    break;
  case 40: 
    //  long sys_rmdir(const char __user *pathname);
    IFLS_S_SIMP(RMDIR,EBX);
    break;
  case 41: 
    //  long sys_dup(unsigned int fildes);
    IFLS_I(DUP,EBX);
    break;
  case 42:
    // sys_pipe is missing from syscalls.h. 
    IFLS_(PIPE);
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
    // sys_break is missing from syscalls.h 
    IFLS(BREAK);
    break;
  case 46 :
    // long sys_setgid16(old_gid_t gid);
    IFLS_I(SETGID16,EBX);
    break;
  case 47 : // sys_getgid16
    // long sys_getgid16(void);
    IFLS(GETGID16);
    break;
  case 48 : 
    // unsigned long sys_signal(int sig, __sighandler_t handler);
    IFLS_II(SIGNAL,EBX,ECX);
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
    IFLS_S_SIMP(ACCT,EBX);
    break;
  case 52 : 
    // sys_umount2 is missing from syscalls.h
    // Xuxian used EBX as a ptr to a string tho...
    IFLS_S(MOUNT2,EBX);
    break;
  case 53 : 
    // sys_lock is missing from syscalls.h
    IFLS(LOCK);
    break;
  case 54 : 
    // long sys_ioctl(unsigned int fd, unsigned int cmd,
    //                unsigned long arg);
    IFLS_III(IOCTL,EBX,ECX,EDX);
    break;
  case 55 : 
    // long sys_fcntl(unsigned int fd, unsigned int cmd, unsigned long arg);
    IFLS_III(FCNTL,EBX,ECX,EDX);
    break;
  case 56 :
    // sys_mpx is missing from syscalls.h
    IFLS(MPX);
    break;
  case 57 : 
    // long sys_setpgid(pid_t pid, pid_t pgid);    
    IFLS_II(SETPGID,EBX,ECX);
    break;
  case 58 : 
    // sys_ulimit is missing from syscalls.h
    IFLS(ULIMIT);
    break;
  case 59 :
    // sys_oldolduname is missing from syscalls.h
    IFLS(OLDOLDUNAME);
    break;
  case 60 :
    // long sys_umask(int mask);
    IFLS_I(UMASK,EBX);
    break;
  case 61 :
    // long sys_chroot(const char __user *filename);
    IFLS_S_SIMP(CHROOT, EBX);
    break;
  case 62 :
    // long sys_ustat(unsigned dev, struct ustat __user *ubuf);
    IFLS(USTAT);
    break;
  case 63 : 
    // long sys_dup2(unsigned int oldfd, unsigned int newfd);
    IFLS_II(DUP2,EBX,ECX);
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
    // sys_sigaction is missing from syscalls.h
    IFLS_I(SIGACTION,EBX);
    break;
  case 68 :
    // long sys_sgetmask(void);
    IFLS(SGETMASK);
    break;
  case 69 : 
    // long sys_ssetmask(int newmask);
    IFLS_I(SSETMASK,EBX);
    break;
  case 70 : 
    // long sys_setreuid(uid_t ruid, uid_t euid);
    IFLS_II(SETREUID,EBX,ECX);
    break;
  case 71 :
    // long sys_setregid(gid_t rgid, gid_t egid);
    IFLS_II(SETREGID,EBX,ECX);
    break;
  case 72 : 
    // sys_sigsuspend is missing from syscalls.h
    IFLS(SIGSUSPEND);
    break;
  case 73 : 
    // long sys_sigpending(old_sigset_t __user *set);
    IFLS(SIGPENDING);
    break;
  case 74 : 
    // long sys_sethostname(char __user *name, int len);
    IFL_SSI_SIMP(SETHOSTNAME,EBX,ECX);
    break;
  case 75 : 
    // long sys_setrlimit(unsigned int resource,
    //                    struct rlimit __user *rlim);
    IFLS_II(SETRLIMIT,EBX,ECX);
    break;
  case 76 : 
    // long sys_getrlimit(unsigned int resource,
    //                    struct rlimit __user *rlim);
    IFLSI(GETRLIMIT,EBX);
    break;
  case 77 : 
    //  long sys_getrusage(int who, struct rusage __user *ru);
    IFLSI(GETRUSAGE,EBX);
    break;
  case 78 : 
    // long sys_gettimeofday(struct timeval __user *tv,
    //                       struct timezone __user *tz);
    IFLS(GETTIMEOFDAY);
    break;
  case 79 : 
    // long sys_settimeofday(struct timeval __user *tv,
    //                       struct timezone __user *tz);
    IFLS(SETTIMEOFDAY);
    break;
  case 80 :
    // long sys_getgroups(int gidsetsize, gid_t __user *grouplist);
    IFLS(GETGROUPS);
    break;
  case 81 : 
    // long sys_setgroups(int gidsetsize, gid_t __user *grouplist);
    IFLS(SETGROUPS);
    break;
  case 82 :
    // long sys_select(int n, fd_set __user *inp, fd_set __user *outp,
    // fd_set __user *exp, struct timeval __user *tvp);
    {
      int fd, *ptr;
      //      fprintf(logfile,"PID %3d (%16s)[sys_select   82]: ", pid, command);
      // write op plus num fds
      IFLS_I(SELECT,EBX);
      if ( EBX > 0 ){
        if (!ECX) {
          paddr = cpu_get_phys_page_debug(env, ECX);
          //	  fprintf(logfile, "readfds");
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, EBX*4); //-> get the fdset 
          i=0; ptr = (int*)tempbuf; fd = *ptr++;
          while (fd ) {
            //	    fprintf(logfile, " %d", fd);
            IFLW_PUT_UINT32_T(fd);
            i++;	
            if ( i<EBX) fd = *ptr++;
            else fd = 0;
          }
        }
        if (!EDX) {
          paddr = cpu_get_phys_page_debug(env, EDX);
          //	  fprintf(logfile, "writefds");
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, EBX*4); //-> get the fdset 
          i=0; ptr = (int*)tempbuf; fd = *ptr++;
          while (fd ) {
            //	    fprintf(logfile, " %d", fd);
            IFLW_PUT_UINT32_T(fd);
            i++;	
            if ( i<EBX) fd = *ptr++;
            else fd = 0;
          }
        }
      }
      //      fprintf(logfile, "\n");
    }
    break;
  case 83 :
    // long sys_symlink(const char __user *old, const char __user *new);
    IFLS_SS_SIMP(SYMLINK,EBX,ECX);
    break;
  case 84 :
    // sys_oldlstat is missing from syscalls.h
    // Xuxian takes EBX to be a ptr to a string.
    IFLS_S_SIMP(OLDLSTAT,EBX);
    break;
  case 85 :
    // long sys_readlink(const char __user *path,
    //                   char __user *buf, int bufsiz);
    IFLS_S_SIMP(READLINK,EBX);
    break;
  case 86 : 
    // long sys_uselib(const char __user *library);
    IFLS_S_SIMP(USELIB,EBX);
    break;
  case 87 : // sys_swapon
    // long sys_swapon(const char __user *specialfile, int swap_flags);
    IFLS_SI_SIMP(SWAPON,EBX,ECX);
    break;
  case 88 :
    // long sys_reboot(int magic1, int magic2, unsigned int cmd,
    //                 void __user *arg);    
    IFLS_III(REBOOT,EBX,ECX,EDX);
    break;
  case 89 : 
    // sys_readdir is missing from syscalls.h
    // Xuxian takes EBX as a ptr to a string.
    IFLS_S_SIMP(READDIR,EBX);
    break;
  case 90 :
    // sys_mmap is missing from syscalls.h
    // This is Xuxian's decoding
    {
      char buf[24];
      int addr, size, fd, *ptr;
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)	{
        copy_string_phys(buf, paddr, 24);
        ptr = (int *)buf;
        addr = *ptr++;
        len = *ptr++;
        ptr++; ptr++;
        fd = *ptr++;
        IFLS_III(MMAP,addr,len,fd);
      }
    }
    break;
  case 91 :
    //  long sys_munmap(unsigned long addr, size_t len);
    IFLS_II(MUNMAP,EBX,ECX);
    break;
  case 92 : 
    // long sys_truncate(const char __user *path,
    //                   unsigned long length);
    IFLS_SI(TRUNCATE,EBX,ECX);
    break;
  case 93 : 
    // long sys_ftruncate(unsigned int fd, unsigned long length);
    IFLS_II(FTRUNCATE,EBX,ECX);
    break;
  case 94 : 
    // long sys_fchmod(unsigned int fd, mode_t mode);
    IFLS_II(FCHMOD,EBX,ECX);
    break;
  case 95 : 
    // long sys_fchown(unsigned int fd, uid_t user, gid_t group);
    IFLS_III(FCHOWN,EBX,ECX,EDX);
    break;
  case 96 : 
    // long sys_getpriority(int which, int who);
    IFLS_II(GETPRIORITY,EBX,ECX);
    break;
  case 97 : 
    // long sys_setpriority(int which, int who, int niceval);
    IFLS_II(SETPRIORITY,EBX,ECX);
    break;
  case 98 :
    // sys_profil is missing from syscalls.h
    IFLS(PROFIL);
    break;
  case 99 : 
    // long sys_statfs(const char __user * path,
    //                 struct statfs __user *buf);
    IFLS_S_SIMP(STATFS,EBX);
    break;
  case 100 :
    // long sys_fstatfs(unsigned int fd, struct statfs __user *buf);
    IFLS_I(FSTATFS,EBX);
    break;
  case 101 : 
    // long sys_ioperm(unsigned long from, unsigned long num, int on);
    IFLS_III(IOPERM,EBX,ECX,EDX);
    break;
  case 102 : 
    // long sys_socketcall(int call, unsigned long __user *args);
    IFLS_I(SOCKETCALL,EBX);
    switch (EBX) {
    case 1: // socket
      { int family, type, protocol, *ptr;
      paddr = cpu_get_phys_page_debug(env, ECX);
      if (paddr!=-1)	{
        bzero(tempbuf, 120);
        cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
        ptr = (int*) tempbuf;
        family = *ptr++; type =*ptr++; protocol = *ptr++;
        IFLW_PUT_UINT32_T(family);
        IFLW_PUT_UINT32_T(type);
        IFLW_PUT_UINT32_T(protocol);
      }
      }
      break;
    case 2: // bind
      { int fd, len, group, *ptr; struct sockaddr_in *sap;
      unsigned char b0, b1, b2, b3, *bptr, b4, b5, b6, b7; 
      unsigned short *sptr;
      unsigned char pkttype, halen, sll_addr[8];
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
        IFLW_PUT_UINT8_T((uint8_t) sap->sin_family);
        IFLW_PUT_UINT32_T(fd);
        if (sap->sin_family == 1 ) { // PF_LOCAL
          IFLW_PUT_STRING(bptr);
          //	    fprintf(logfile,"family  1; fd %d; file %s", fd, bptr);
        }
        else if (sap->sin_family == 2 ) { // PF_INET
          IFLW_PUT_UINT32_T(b4);
          IFLW_PUT_UINT32_T(b5);
          IFLW_PUT_UINT32_T(b6);
          IFLW_PUT_UINT32_T(b7);
          IFLW_PUT_UINT32_T(b2*256+b3);
          //	    fprintf(logfile,"family  2; fd %d; %d.%d.%d.%d:%d", 
          //		    fd, b4, b5, b6, b7, b2*256+b3);
        }
        else if (sap->sin_family == 16 ) {// PF_NETLINK
          IFLW_PUT_UINT32_T(pid);
          IFLW_PUT_UINT32_T(group);
          //	    fprintf(logfile,"family 16; fd %d; pid %d; group %d", 
          //		    fd, pid, group);
          else if (sap->sin_family == 17 )  {// PF_PACKET
            bptr = tempbuf; bptr += 8; sptr = (unsigned short*)bptr;
            bptr +=2; pkttype =*bptr; halen =*bptr++;
            IFLW_PUT_UINT32_T(b2*256+b3);
            IFLW_PUT_UINT32_T(pid);	  	    
            /*
              fprintf(logfile,"family 17; fd %d; protocol 0x%x; ifindex 0x%x",
              fd, b2*256+b3, pid);
            */
            /*
              fprintf(logfile,"family 17; fd %d; protocol %d; ifindex %d; hatype %d; pkttype %d; halen %d; addr %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x", 
              sap->sin_family, fd, b2*256+b3, pid, *sptr, pkttype, halen, 
              tempbuf[12], tempbuf[13], tempbuf[14], tempbuf[15],
              tempbuf[16], tempbuf[17], tempbuf[18], tempbuf[19]);
            */
          } else {
            IFLW_PUT_UINT32_T(b0);
            IFLW_PUT_UINT32_T(len);
            // fprintf(logfile,"family %2d; fd %d; %d.%d.%d.%d-%d.%d.%d.%d(%c%c%c%c%c%c%c%c); len %d", 
            //		    sap->sin_family, fd, b0, b1, b2, b3,b4, b5, b6, 
            //		    b7, b0, b1, b2, b3,b4, b5, b6, b7, len);
          }
        }
      }
      break;
      case 3: // connect
        { int fd, len, *ptr; struct sockaddr_in *sap;
        unsigned char b0, b1, b2, b3, *bptr, b4, b5, b6, b7; 
        //	fprintf(logfile,"PID %3d (%16s)[sys_connect 102]%d: ", pid, command, EBX);
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
          IFLW_PUT_UINT8_T((uint8_t) sap->sin_family);
          IFLW_PUT_UINT32_T(fd);
          if (sap->sin_family == 1) {// PF_LOCAL
            IFLW_PUT_STRING(bptr);
            //	    fprintf(logfile,"socket %d; family  1; file %s", fd, bptr);
          }
          else if (sap->sin_family == 2) { // PF_INET
            IFLW_PUT_UINT32_T(b4);
            IFLW_PUT_UINT32_T(b5);
            IFLW_PUT_UINT32_T(b6);
            IFLW_PUT_UINT32_T(b7);
            IFLW_PUT_UINT32_T(b2*256+b3);
            //	    fprintf(logfile,"socket %d; family  2; %d.%d.%d.%d:%d", 
            //		    fd, b4, b5, b6, b7, b2*256+b3);
          }
          else {
            //	    fprintf(logfile,"socket %d; family %2d;",
            //		    fd, sap->sin_family);
          }
        }
        //	fprintf(logfile,"\n");
        }
        break;
      }
    case 4: // listen
      //      fprintf(logfile,"PID %3d (%16s)[sys_listen  102]%d: ", pid, command, EBX);
      paddr = cpu_get_phys_page_debug(env, ECX);
      if (paddr!=-1)	{
        bzero(tempbuf, 120);
        cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
        //	fprintf(logfile,"socket %d", *(int*) tempbuf);
        IFLW_PUT_UINT32_T(*((int*)tempbuf));
      }
      //      fprintf(logfile,"\n");
      break;
    case 5: // accept
      //      fprintf(logfile,"PID %3d (%16s)[sys_accept  102]%d: ", pid, command, EBX);
      paddr = cpu_get_phys_page_debug(env, ECX);
      if (paddr!=-1)	{
        bzero(tempbuf, 120);
        cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
        //	fprintf(logfile,"socket %d", *(int*) tempbuf);
        IFLW_PUT_UINT32_T(*((int*)tempbuf));
      }
      //      fprintf(logfile,"\n");      
      break;
    case 6: // getsockname
      //      fprintf(logfile,"PID %3d (%16s)[sys_getskna 102]%d: ", pid, command, EBX);
      paddr = cpu_get_phys_page_debug(env, ECX);
      if (paddr!=-1)	{
        bzero(tempbuf, 120);
        cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
        //	fprintf(logfile,"socket %d", *(int*) tempbuf);
        IFLW_PUT_UINT32_T(*((int*)tempbuf));
      }
      //      fprintf(logfile,"\n");      
      break;
    case 7: // getpeername
      //      fprintf(logfile,"PID %3d (%16s)[sys_getpeer 102]%d: ", pid, command, EBX);
      paddr = cpu_get_phys_page_debug(env, ECX);
      if (paddr!=-1)	{
        bzero(tempbuf, 120);
        cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
        //	fprintf(logfile,"socket %d", *(int*) tempbuf);
        IFLW_PUT_UINT32_T(*((int*)tempbuf));
      }
      //      fprintf(logfile,"\n");     
      break;
    case 8: 
      {// socketpair
        int domain, type, protocol, *socket_vector, *ptr;
        //      fprintf(logfile,"PID %3d (%16s)[sys_skpair  102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_page_debug(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          ptr = (int *) tempbuf;
          domain =*ptr++; type=*ptr++; protocol=*ptr++; socket_vector=*ptr++;
          //	fprintf(logfile,"domain %d; type %d; protocol %d\n", *(int*) tempbuf);
          IFLW_PUT_UINT32_T(*((int*)tempbuf));	
        }
      }          
      break;    
    case 9: // send
      {
        int fd, msg, len, *ptr;
	
        //	fprintf(logfile,"PID %3d (%16s)[sys_send    102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_page_debug(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          ptr = (int*) tempbuf;
          fd = *ptr++; msg=*ptr++; len= *ptr++;
          //	  fprintf(logfile,"socket %d ", fd);
	  
          bzero(tempbuf, 120);
          paddr = cpu_get_phys_page_debug(env, msg);
          cpu_physical_memory_read(paddr, tempbuf, 30); 
          for (i=0; i<30; i++)
            if (iscntrl(tempbuf[i])) tempbuf[i]='.';
          //	  fprintf(logfile,"(%s)(%d)", tempbuf, len);
          IFLW_PUT_UINT32_T(fd);
          IFLW_PUT_STRING(tempbuf);
          IFLW_PUT_UINT32_T(len);
        }
        //	fprintf(logfile,"\n");	
      }
      break;
    case 10: // recv
      {
        int fd, len, *ptr;
        unsigned int msg;
	
        //	fprintf(logfile,"PID %3d (%16s)[sys_recv    102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_page_debug(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          ptr = (int*) tempbuf;
          fd = *ptr++; msg=*ptr++; len= *ptr;
          //	  fprintf(logfile,"socket %d, msg 0x%08x\n", fd, msg);
          IFLW_PUT_UINT32_T(fd);
          IFLW_PUT_UINT32_T(msg);
        }
      }
      break;
    case 11: // sendto
      {
        int fd,msg, len, *ptr; struct sockaddr_in *sap;
        unsigned char b0, b1, b2, b3, *bptr, b4, b5, b6, b7;
	
        //	fprintf(logfile,"PID %3d (%16s)[sys_sendto  102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_page_debug(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          ptr = (int*) tempbuf;
          fd = *ptr++; msg=*ptr++; len= *ptr++; ptr++; sap=(struct sockaddr_in *)*ptr;
          //	  fprintf(logfile,"socket %d --> ", fd);
          bzero(tempbuf, 120);
          paddr = cpu_get_phys_page_debug(env, sap);
          cpu_physical_memory_read(paddr, tempbuf, 120); 
          bptr = tempbuf;
          b0=*bptr++; b1=*bptr++; b2=*bptr++; b3=*bptr++;
          b4=*bptr++; b5=*bptr++; b6=*bptr++; b7=*bptr++;
          bptr = tempbuf; bptr+=2;	  
          sap = (struct sockaddr_in*) tempbuf;
          IFLW_PUT_UINT8_T(sap->sin_family);
          IFLW_PUT_UINT32_T(fd);
          if (sap->sin_family == 1 ) {// PF_LOCAL
            IFLW_PUT_STRING(bptr);
            //	    fprintf(logfile,"[dest: family  1; file %s]", bptr);
          }
          else if (sap->sin_family == 2 ) {// PF_INET
            IFLW_PUT_UINT32_T(b4);
            IFLW_PUT_UINT32_T(b5);
            IFLW_PUT_UINT32_T(b6);
            IFLW_PUT_UINT32_T(b7);
            IFLW_PUT_UINT32_T(b2*256+b3);
            //	    fprintf(logfile,"[dest: family  2; %d.%d.%d.%d:%d]", 
            //		    b4, b5, b6, b7, b2*256+b3);
          }
          else {
            //fprintf(logfile,"[dest: family %2d]", sap->sin_family);
          }
          bzero(tempbuf, 120);
          paddr = cpu_get_phys_page_debug(env, msg);
          cpu_physical_memory_read(paddr, tempbuf, 30); 
          for (i=0; i<30; i++)
            if (iscntrl(tempbuf[i])) tempbuf[i]='.';
          IFLW_PUT_STRING(tempbuf);
          IFLW_PUT_UINT32_T(len);
          //	  fprintf(logfile," (%s)(%d)", tempbuf, len);
	  
	  
        }
        //	fprintf(logfile,"\n");	
      }
      break;
    case 12: // recvfrom
      {
        int fd, *ptr;
	
        //	fprintf(logfile,"PID %3d (%16s)[sys_recvfro 102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_page_debug(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          ptr = (int*) tempbuf;
          fd = *ptr++; 
          //	  fprintf(logfile,"socket %d  ", fd);
          IFLW_PUT_UINT32_T(fd);
        }
        //	fprintf(logfile,"\n");	
      }
      break;
    case 13: // shutdown
      {
        int fd, *ptr;
	
        //	fprintf(logfile,"PID %3d (%16s)[sys_shutdow 102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_page_debug(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          ptr = (int*) tempbuf;
          fd = *ptr++; 
          IFLW_PUT_UINT8_T((uint8_t) *ptr);
          IFLW_PUT_UINT32_T(fd);
          /*
            if (*ptr == 0 ) 
            fprintf(logfile,"socket %d (SHUT_RD)",   fd);
            else if (*ptr == 1 )
            fprintf(logfile,"socket %d (SHUT_WR)",   fd);
            else if (*ptr == 2 )
            fprintf(logfile,"socket %d (SHUT_RDWR)", fd);
          */
        }
        //	fprintf(logfile,"\n");
      }
      break;
    case 14: // setsockopt
      {
        int fd, level, option, *ptr;
	
        //	fprintf(logfile,"PID %3d (%16s)[sys_setskop 102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_page_debug(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          ptr = (int*) tempbuf;
          fd = *ptr++; level=*ptr++; option=*ptr++;
          IFLW_PUT_UINT32_T(fd);
          IFLW_PUT_UINT32_T(level);
          IFLW_PUT_UINT32_T(option);
          //	  fprintf(logfile,"socket %d; level %d; option %d", fd, level, option);
        }
        //	fprintf(logfile,"\n");
      }
      break;
    case 15: // getsockopt
      {
        int fd, level, option, *ptr;
	
        //	fprintf(logfile,"PID %3d (%16s)[sys_getskop 102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_page_debug(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          ptr = (int*) tempbuf;
          fd = *ptr++; level=*ptr++; option=*ptr++;
          IFLW_PUT_UINT32_T(fd);
          IFLW_PUT_UINT32_T(level);
          IFLW_PUT_UINT32_T(option);
          //	  fprintf(logfile,"socket %d; level %d; option %d", fd, level, option);
        }
        //	fprintf(logfile,"\n");
      }
      break;
    case 16: // sendmsg
      {
        int fd, *ptr;
	
        //	fprintf(logfile,"PID %3d (%16s)[sys_sendmsg 102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_page_debug(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          ptr = (int*) tempbuf;
          fd = *ptr++; 
          //	  fprintf(logfile,"socket %d", fd);
          IFLW_PUT_UINT32_T(fd);
        }
        //	fprintf(logfile,"\n");
      }
      break;
    case 17: // recvmsg
      {
        int fd, *ptr;
	
        //	fprintf(logfile,"PID %3d (%16s)[sys_recvmsg 102]%d: ", pid, command, EBX);
        paddr = cpu_get_phys_page_debug(env, ECX);
        if (paddr!=-1)	{
          bzero(tempbuf, 120);
          cpu_physical_memory_read(paddr, tempbuf, nargs[EBX] ); //-> get the args
          ptr = (int*) tempbuf;
          fd = *ptr++; 
          //	  fprintf(logfile,"socket %d", fd);
          IFLW_PUT_UINT32_T(fd);
        }
        //	fprintf(logfile,"\n");
      }
      break;
    }
    break;
  case 103 : 
    // long sys_syslog(int type, char __user *buf, int len);
    {
      int size=30;
      paddr = cpu_get_phys_page_debug(env, ECX);
      //      fprintf(logfile,"PID %3d (%16s)[sys_syslog  103]: \n", pid, command);
      bzero(tempbuf, 120);
      if (EDX>0 && EDX <= 30) size = EDX; 
      cpu_physical_memory_read(paddr, tempbuf, size); 
      for (i=0; i<size; i++)
        if (iscntrl(tempbuf[i])) tempbuf[i]='.';
      //      fprintf(logfile,"(%s)(%d)\n", tempbuf, EDX);
      IFLS_SI(SYSLOG,tempbuf,EDX);
    }
    break;
  case 104 : 
    // long sys_setitimer(int which,
    //                    struct itimerval __user *value,
    //                    struct itimerval __user *ovalue);
    IFLS_I(SETITIMER,EBX);
    break;
  case 105 : 
    // long sys_getitimer(int which, struct itimerval __user *value);
    IFLS_I(GETITIMER,EBX);
    break;
  case 106 :
    // long sys_stat(char __user *filename,
    //               struct __old_kernel_stat __user *statbuf);
    IFLS_S_SIMP(STAT,EBX);
    break;
  case 107 :
    // long sys_lstat(char __user *filename,
    //                struct __old_kernel_stat __user *statbuf);
    IFLS_S_SIMP(LSTAT,EBX);
    break;
  case 108 : 
    // long sys_fstat(unsigned int fd,
    //                struct __old_kernel_stat __user *statbuf);
    IFLS_I_SIMP(FSTAT,EBX);
    break;
  case 109 : 
    // sys_olduname is missing from syscalls.h
    IFLS(OLDUNAME);
    break;
  case 110 : 
    // sys_iopl is missing from syscalls.h
    IFLS(IOPL);
    break;
  case 111 : 
    //  long sys_vhangup(void);
    IFLS(VHANGUP);
    break;
  case 112 :
    // sys_idle is missing from syscalls.h
    IFLS(IDLE);
    break;
  case 113 :
    // sys_vm86old is missing from syscalls.h
    IFLS(VM86OLD);
    break;
  case 114 :
    // long sys_wait4(pid_t pid, int __user *stat_addr,
    //                int options, struct rusage __user *ru);
    IFLS_II(WAIT4,EBX,EDX);
    break;
  case 115 :
    // long sys_swapoff(const char __user *specialfile);
    IFLS_S_SIMP(SWAPOFF,EBX);
    break;
  case 116 :
    // long sys_sysinfo(struct sysinfo __user *info);
    IFLS(SYSINFO);
    break;
  case 117 :
    // sys_ipc is missing from syscalls.h
    // Xuxian seems to know what to do with it, though. 

    // call first second third ptr fifth
    //->EBX  ECX  EDX   ESI   EDI  EBP  

    // write op and all regs. 
    IFLS_IIIII(IPC,EBX,ECX,EDX,ESI,EDI);
    //    fprintf(logfile,"PID %3d (%16s)[sys_ipc     117]: ", pid, command);
    /*
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
    */
    break;
  case 118 : 
    // long sys_fsync(unsigned int fd);
    IFLS_I(FSYNC,EBX);
    break;
  case 119 :
    // sys_sigreturn is missing in syscalls.h
    IFLS(SIGRETURN);
    break;
  case 120 : 
    // sys_clone is missing in syscalls.h
    IFLS(CLONE);
    break;
  case 121 : 
    //  long sys_setdomainname(char __user *name, int len);
    IFLS_SI_SIMP(SETDOMAINNAME,EBX,ECX);
    break;
  case 122 :
    // sys_uname is missing in syscalls.h
    IFLS(UNAME);
    break;
  case 123 : 
    // sys_modify_ldt is missing in syscalls.h
    IFLS(MODIFY_LDT);
    break;
  case 124 : 
    // long sys_adjtimex(struct timex __user *txc_p);
    IFLS(ADJTIMEX);
    break;
  case 125 :
    // long sys_mprotect(unsigned long start, size_t len,
    //                   unsigned long prot);
    IFLS_III(MPROTECT,EBX,ECX,EDX);
    break;
  case 126 :
    // long sys_sigprocmask(int how, old_sigset_t __user *set,
    //                      old_sigset_t __user *oset);
    IFLS_II(SIGPROCMASK,EBX,ECX);
    break;
  case 127 : 
    // sys_create_module is missing from syscalls.h
    // Xuxian seems to know what to do with it. 
    IFLS_S_SIMP(CREATE_MODULE,EBX);
    break;
  case 128 : 
    // long sys_init_module(void __user *umod, unsigned long len,
    //                      const char __user *uargs);
    IFLS_SI(INIT_MODULE,ECX,EDX);
    // WTF? How can Xuxian be using EBX as the string ptr? 
    // shouldnt it be EDX? 
    /*
      fprintf(logfile,"PID %3d (%16s)[sys_ini_mod 128]: module ", pid, command);
      paddr = cpu_get_phys_page_debug(env, EBX);
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
      fprintf(logfile,"%s\n", tempbuf);
    */
    break;
  case 129 : 
    // long sys_delete_module(const char __user *name_user,
    //                        unsigned int flags);
    IFLS_S_SIMP(DELETE_MODULE,EBX);
    break;
  case 130 : 
    // sys_get_kernel_syms is missing from syscalls.h
    IFLS(GET_KERNEL_SYMS);
    break;
  case 131 : 
    // long sys_quotactl(unsigned int cmd, const char __user *special,
    //                   qid_t id, void __user *addr);
    IFLS_ISI(QUOTACTL,EBX,ECX,EDX);
    break;
  case 132 :
    // long sys_getpgid(pid_t pid);
    IFLS_I(GETPGID,EBX);
    break;
  case 133 :
    // long sys_fchdir(unsigned int fd);
    IFLS_I(FCHDIR,EBX);
    break;
  case 134 : 
    // long sys_bdflush(int func, long data);
    IFLS_II(BDFLUSH,EBX,EDX);
    break;
  case 135 : 
    // long sys_sysfs(int option,
    //                unsigned long arg1, unsigned long arg2);
    IFLS_III(SYSFS,EBX,ECX,EDX);
    break;
  case 136 : 
    // long sys_personality(u_long personality);
    IFSL_I(PERSONALITY,EBX);
    break;
  case 137 :
    // sys_afs_syscall is missing from syscalls.h
    IFLS(AFS_SYSCALL);
    break;
  case 138 : 
    // long sys_setfsuid(uid_t uid);
    IFLS_I(SETFSUID,EBX);
    break;
  case 139 :
    // long sys_setfsgid(gid_t gid);
    IFSL_I(SETFSGID,EBX);
    break;
  case 141 : 
    // long sys_getdents(unsigned int fd,
    //                   struct linux_dirent __user *dirent,
    //                   unsigned int count);
    IFLS_I(GETDENTS,EBX);
    break;
  case 143 : 
    // long sys_flock(unsigned int fd, unsigned int cmd);
    IFLS_II(FLOCK,EBX,ECX);
    break;
  case 144 :
    // long sys_msync(unsigned long start, size_t len, int flags);
    IFLS_III(MSYNC,EBX,ECX,EDX);
    break;
  case 145 : // sys_readv
    // ssize_t sys_readv(unsigned long fd,
    //                   const struct iovec __user *vec,
    //                   unsigned long vlen);
    IFLS_II(READV,EBX,EDX);
    break;
  case 146 : 
    //  ssize_t sys_writev(unsigned long fd,
    //                     const struct iovec __user *vec,
    //                     unsigned long vlen);
    IFLS_II(WRITEV,EBX,EDX);
    /*
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
    */
    break;
  case 147 : 
    // long sys_getsid(pid_t pid);
    IFLS_I(GETSID,EBX);
    break;
  case 148 : 
    // long sys_fdatasync(unsigned int fd);
    IFLS_I(FDATASYNC,EBX);
    break;
  case 150 :
    // long sys_mlock(unsigned long start, size_t len);
    IFLS_II(MLOCK,EBX,ECX);
    break;
  case 151 : 
    // long sys_munlock(unsigned long start, size_t len);
    IFLS_II(MUNLOCK,EBX,ECX);
    break;
  case 152 : 
    // long sys_mlockall(int flags);
    IFLS_I(MLOCKALL,EBX);
    break;
  case 153 : 
    // long sys_munlockall(void);
    IFLS(MUNLOCKALL);
    break;
  case 154 :
    // long sys_sched_setparam(pid_t pid,
    //                         struct sched_param __user *param);
    IFLS_I(SCHED_SETPARAM,EBX);
    break;
  case 155 : // sys_sched_getparam
    // long sys_sched_getparam(pid_t pid,
    //                         struct sched_param __user *param);
    IFLS_I(SCHED_GETPARAM,EBX);
    break;
  case 156 : 
    // long sys_sched_setscheduler(pid_t pid, int policy,
    //                             struct sched_param __user *param);
    IFLS_II(SCHED_SETSCHEDULER,EBX,ECX);
    break;
  case 157 :
    // long sys_sched_getscheduler(pid_t pid);
    IFLS_I(SCHED_GETSCHEDULER,EBX);
    break;
  case 158 : 
    // long sys_sched_yield(void);
    IFLS(SCHED_YIELD);
    break;
  case 159 :
    // long sys_sched_get_priority_max(int policy);
    IFLS_I(SCHED_GET_PRIORITY_MAX,EBX);
    break;
  case 160 : 
    // long sys_sched_get_priority_min(int policy);
    IFLS_I(SCHED_GET_PRIORITY_MIN,EBX);
    break;
  case 161 : 
    // long sys_sched_rr_get_interval(pid_t pid,
    //                                struct timespec __user *interval);
    IFLS_I(SCHED_RR_GET_INTERVAL,EBX);
    break;
  case 162 :
    // long sys_nanosleep(struct timespec __user *rqtp, struct timespec __user *rmtp);
    IFLS(NANO_SLEEP);
    break;
  case 163 : 
    // unsigned long sys_mremap(unsigned long addr,
    //                          unsigned long old_len, unsigned long new_len,
    //                          unsigned long flags, unsigned long new_addr);
    IFLS_IIII(MREMAP,EBX,ECX,EDX,ESI);
    break;
  case 164 :
    // long sys_setresuid(uid_t ruid, uid_t euid, uid_t suid);
    IFLS_III(SETRESUID,EBX,ECX,EDX);
    break;
  case 165 : 
    // long sys_getresuid16(old_uid_t __user *ruid,
    //                      old_uid_t __user *euid, old_uid_t __user *suid);
    IFLS_III(GETRESUID);
    fprintf(logfile,"PID %3d (%16s)[sys_getresu 165]\n", pid, command);
    break;
  case 166 : 
    // sys_vm86 is missing from syscalls.h
    IFSL(VM86);
    break;
  case 167 : 
    // sys_query_module is missing from syscalls.h    
    IFSL_S_SIMP(QUERY_MODULE,EBX);
    break;
  case 168 :
    // long sys_poll(struct pollfd __user *ufds, unsigned int nfds,
    //               long timeout);
    { int i, len; struct pollfd *iovp; 
    //      fprintf(logfile,"PID %3d (%16s)[sys_poll    168]: nfd %d; timeout %d; ", 
    //	      pid, command, ECX, EDX);
    IFLS_II(POLL,ECX,EDX);
    if (ECX>0) {
      paddr = cpu_get_phys_page_debug(env, EBX);
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, ECX*sizeof(struct pollfd)); 
      iovp = (struct pollfd*) tempbuf;
      for(i=0; i<ECX; i++, iovp++) {
        //	  fprintf(logfile," fds[%d].fd %d (events %d);", i, iovp->fd, iovp->events);
        IFLW_PUT_UINT32_T(iovp->fp);
        IFLW_PUT_UINT32_T(iovp->events);
      } 
    }
    //      fprintf(logfile,"\n");
    } 
    break;
  case 169 :
    // long sys_nfsservctl(int cmd,
    //                     struct nfsctl_arg __user *arg,
    //                     void __user *res);
    IFLS_I(NFSSERVCTL,EBX);
    break;
  case 170 : 
    // sys_setresgid(gid_t rgid, gid_t egid, gid_t sgid);
    IFLS_III(SETRESGID,EBX,ECX,EDX);
    break;
  case 171 : 
    // long sys_getresgid(gid_t __user *rgid, gid_t __user *egid, gid_t __user *sgid);
    IFLS(GETRESGID);
    break;
  case 172 : 
    // long sys_prctl(int option, unsigned long arg2, unsigned long arg3,
    //                unsigned long arg4, unsigned long arg5);
    IFLS_III(PRCTL,EBX,ECX,EDX);
    break;
  case 173 : 
    // sys_rt_sigreturn is missing from syscalls.h
    IFLS(SIGRETURN);
    break;
  case 174 :
    // sys_rt_sigaction is missing from syscalls.h
    // Xuxian seems to know how to decode it. 
    IFLS_II(RT_SIGACTION,EBX,ECX);
    //    fprintf(logfile,"PID %3d (%16s)[sys_rt_siga 174]: sig %d; handler 0x%08x\n", 
    //	    pid, command, EBX, ECX);
    break;
  case 175 : 
    // sys_rt_sigprocmask(int how, sigset_t __user *set,
    //                    sigset_t __user *oset, size_t sigsetsize);
    IFLS_I(RT_SIGPROCMASK,EBX);
    // Why is Xuxian logging the set pointer?
    //    fprintf(logfile,"PID %3d (%16s)[sys_rt_mask 175]: how %d; set 0x%08x\n", 
    //	    pid, command, EBX, ECX);
    break;
  case 176 : 
    // long sys_rt_sigpending(sigset_t __user *set, size_t sigsetsize);
    IFLS(RT_SIGPENDING);
    break;
  case 177 : 
    // long sys_rt_sigtimedwait(const sigset_t __user *uthese,
    //                          siginfo_t __user *uinfo,
    //                          const struct timespec __user *uts,
    //            	        size_t sigsetsize);
    IFLS(RT_SIGTIMEDWAIT);
    break;
  case 178 : 
    // long sys_rt_sigqueueinfo(int pid, int sig, siginfo_t __user *uinfo);
    IFLS_II(RT_SIGQUEUINFO,EBX,ECX);
    break;
  case 179 :
    // sys_rt_sigsuspend is missing from syscalls.h
    IFLS(RT_SIGSUSPEND);
    break;
  case 180 : 
    // ssize_t sys_pread64(unsigned int fd, char __user *buf,
    //                     size_t count, loff_t pos);
    IFLS_III(PREAD64,EBX,EDX,ESI);
    break;
  case 181 : 
    // ssize_t sys_pwrite64(unsigned int fd, const char __user *buf,
    //             	    size_t count, loff_t pos);
    IFLS_III(PWRITE64,EBX,EDX,ESI);
    break;
  case 182 : 
    // long sys_chown(const char __user *filename,
    //                uid_t user, gid_t group);
    IFLS_SII_SIMP(CHOWN,EBX,ECX,EDX);
    break;
  case 183 : 
    // long sys_getcwd(char __user *buf, unsigned long size);
    IFLS_SI_SIMP(GETCWD,EBX,ECX);
    break;
  case 184 : 
    // long sys_capget(cap_user_header_t header,
    //                 cap_user_data_t dataptr);
    IFLS(CAPGET);
    break;
  case 185 : 
    // long sys_capset(cap_user_header_t header,
    //                 const cap_user_data_t data);
    IFLS(CAPSET);
    break;
  case 186 : 
    // sys_sigaltstack is missing from syscalls.h
    IFLS(SIGALTSTACK);
    break;
  case 187 : 
    // ssize_t sys_sendfile(int out_fd, int in_fd,
    //                      off_t __user *offset, size_t count);
    IFLS_II(SENDFILE,EBX,ECX,ESI);
    break;
  case 188 : 
    // sys_getpmsg is missing from syscalls.h
    IFLS(GETPMSG);
    break;
  case 189 : 
    // sys_putpmsg is missing from syscalls.h
    IFLS(PUTPMSG);
    break;
  case 190 :
    // sys_vfork is missing from syscalls.h
    IFLS(VFORK);
    break;
  case 191 : 
    // long sys_getrlimit(unsigned int resource,
    //                    struct rlimit __user *rlim);
    IFLS_I(GETRLIMIT,EBX);
    break;
  case 192 :
    // sys_mmap2 is missing from syscalls.h
    // yet Xuxian knows how to decode it..
    IFLS_IIIIII(MMAP2,EBX,ECX,EDX,ESI,EDI,EBP);
    //    fprintf(logfile,"PID %3d (%16s)[sys_mmap2   192]: addr 0x%08x, len 0x%08x, prot %d, flags %d, fd %d, offset 0x%08x\n", pid, command, EBX, ECX, EDX, ESI, EDI, EBP);
    break;
  case 193 : 
    // long sys_truncate64(const char __user *path, loff_t length);
    IFLS_SI_SIMP(TRUNCATE64,EBX,ECX);
    break;
  case 194 : 
    // long sys_ftruncate64(unsigned int fd, loff_t length);
    IFLS_II(FTRUNCATE64,EBX,ECX);
    break;
  case 195 : 
    // long sys_stat64(char __user *filename,
    //                 struct stat64 __user *statbuf);
    IFLS_S_SIMP(STAT64,EBX);
    break;
  case 196 : 
    // long sys_lstat64(char __user *filename,
    //                  struct stat64 __user *statbuf);
    IFLS_S_SIMP(LSTAT64,EBX);
    break;
  case 197 : 
    // long sys_fstat64(unsigned long fd, struct stat64 __user *statbuf);
    IFLS_I(FSTAT64,EBX);
    //fprintf(logfile,"PID %3d (%16s)[sys_fstat64 197]\n", pid, command);
    // What's up with this.  EBX should be a file descriptor, not a pointer.
    // I think this is just wrong. 
    /*
      paddr = cpu_get_phys_page_debug(env, EBX);
      if (paddr!=-1)  {
      fprintf(logfile,"PID %3d (%16s)", pid, command);
      fprintf(logfile,"[sys_fstat64 108]: ");
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
      fprintf(logfile,"%s \n", tempbuf);
      }
    */
    break;
  case 198 :
    // sys_lchown32 is missing from syscalls.h
    // Xuxian seems to know how to decode it.
    IFLS_SII_SIMP(LCHOWN32,EBX,ECX,EDX);
    /*
      fprintf(logfile,"PID %3d (%16s)[sys_lchow32 198]\n", pid, command);
      paddr = cpu_get_phys_page_debug(env, EBX);
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
      fprintf(logfile,"%s (uid %d, gid %d)\n", tempbuf, ECX, EDX);
    */
    break;
  case 199 : 
    // sys_getuid32 is missing from syscalls.h
    IFLS(GETUID32);
    break;
  case 200 : 
    // sys_getgid32 is missing from syscalls.h
    IFLS(GETGID32);
    break;
  case 201 : 
    // sys_geteuid32 is missing from syscalls.h
    IFLS(GETEUID32);
    break;
  case 202 : 
    // sys_getegid32 is missing from syscalls.h
    IFLS(GETEGID32);
    break;
  case 203 : 
    // sys_setreuid32 is missing from syscalls.h
    // Xuxian knows how to decode.
    IFLS_II(SETREUID32,EBX,ECX);
    //    fprintf(logfile,"PID %3d (%16s)[sys_seteu32 203]: ruid %d; euid %d\n", pid, command, EBX, ECX);
    break;
  case 204 : 
    // sys_setregid32 is missing from syscalls.h
    // Xuxian knows how to decode.
    IFLS_II(SETREGID32,EBX,ECX);
    //    fprintf(logfile,"PID %3d (%16s)[sys_seteg32 204]: rgid %d; egid %d\n", pid, command, EBX, ECX);
    break;
  case 205 : 
    // sys_getgroups32 is missing from syscalls.h
    IFLS(GETGROUPS32);
    //    fprintf(logfile,"PID %3d (%16s)[sys_getgr32 205]\n", pid, command);
    break;
  case 206 : 
    // sys_setgroups32 is missing from syscalls.h
    IFLS(SETGROUPS32);
    break;
  case 207 : 
    // sys_fchown32 is missing from syscalls.h
    // Xuxian knows how to decode.
    IFLS_III(FCHOWN32,EBX,ECX,EDX);
    //    fprintf(logfile,"PID %3d (%16s)[sys_fchow32 207]: fd %d; uid %d; gid %d\n", pid, command, EBX, ECX, EDX);
    break;
  case 208 : 
    // sys_setresuid32 is missing from syscalls.h
    // Xuxian knows how to decode
    IFLS_III(SETRESUID32,EBX,ECX,EDX);
    //    fprintf(logfile,"PID %3d (%16s)[sys_setsu32 208]: ruid %d; euid %d; suid %d;\n", pid, command, EBX, ECX, EDX);
    break;
  case 209 : 
    // sys_getresuid32 is missing from syscalls.h
    IFLS(GETRESUID32);
    break;
  case 210 :
    // sys_setresgid32 is missing from syscalls.h
    // Xuxian knows how to decode.
    IFLS_III(SETRESGID32,EBX,ECX,EDX);
    //    fprintf(logfile,"PID %3d (%16s)[sys_setes32 208]: rgid %d; egid %d; sgid %d;\n", pid, command, EBX, ECX, EDX);
    break;
  case 211 : 
    // sys_getresgid32 is missing from syscalls.h
    IFLS(GETRESGID32);
    break;
  case 212 : 
    // sys_chown32 is missing from syscalls.h
    // Xuxian knows how to decode.
    IFLS_SII_SIMP(EBX,ECX,EDX);
    /*
      fprintf(logfile,"PID %3d (%16s)[sys_chown32 212]: ", pid, command);
      paddr = cpu_get_phys_page_debug(env, EBX);
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
      fprintf(logfile,"%s (uid %d, gid %d)\n", tempbuf, ECX, EDX);
    */
    break;
  case 213 :
    // sys_setuid32 is missing from syscalls.h
    // Xuxian knows how to decode.
    IFLS_I(SETUID32,EBX);
    //    fprintf(logfile,"PID %3d (%16s)[sys_setui32 213]: uid %d\n", pid, command, EBX);
    break;
  case 214 : 
    // sys_setgid32 is missing from syscalls.h
    // Xuxian knows how to decode.
    IFLS_I(SETGID32,EBX);
    //    fprintf(logfile,"PID %3d (%16s)[sys_setgi32 214]: gid %d\n", pid, command, EBX);
    break;
  case 215 : 
    // sys_setfsuid32 is missing from syscalls.h
    // Xuxian knows how to decode.
    IFLS_I(SETFSUID32,EBX);
    //    fprintf(logfile,"PID %3d (%16s)[sys_setfs32 215]: fsuid %d\n", pid, command, EBX);
    break;
  case 216 : 
    // sys_setfsgid32 is missing from syscalls.h
    // Xuxian knows how to decode.
    IFLS_I(SETFSGID32,EBX);
    //    fprintf(logfile,"PID %3d (%16s)[sys_setfs32 216]: fsgid %d\n", pid, command, EBX);
    break;
  case 217 :
    // long sys_pivot_root(const char __user *new_root,
    //                     const char __user *put_old);
    IFLS_SS_SIMP(PIVOT_ROOT,EBX,ECX);
    // Why not log 2nd string?
    /*
      fprintf(logfile,"PID %3d (%16s)[sys_pivot_r 217]: ", pid, command);
      paddr = cpu_get_phys_page_debug(env, EBX);
      bzero(tempbuf, 120);
      cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
      fprintf(logfile,"%s\n", tempbuf);
    */
    break;
  case 218 : 
    // long sys_mincore(unsigned long start, size_t len,
    //                  unsigned char __user * vec);
    IFLS_II(MINCORE,EBX,ECX);
    break;
  case 219 : 
    // long sys_madvise(unsigned long start, size_t len, int behavior);
    IFLS_III(MADVISE,EBX,ECX,EDX);
    break;
  case 220 : 
    // long sys_getdents64(unsigned int fd,
    //                     struct linux_dirent64 __user *dirent,
    //                     unsigned int count);
    IFLS_II(GETDETS64,EBX,EDX);
    break;
  case 221 : 
    // long sys_fcntl64(unsigned int fd,
    //	                unsigned int cmd, unsigned long arg);
    IFLS_III(FCNTL64,EBX,ECX,EDX);
    break;
  case 224 : 
    //  long sys_gettid(void);
    IFLS(GETTID);
    break;
  case 225 : 
    // ssize_t sys_readahead(int fd, loff_t offset, size_t count);
    IFLS_III(READAHEAD,EBX,ECX,EDX);
    break;
  case 226 : 
    // long sys_setxattr(const char __user *path, const char __user *name,
    //                   const void __user *value, size_t size, int flags);
    IFLS_SSII(SETXATTR,EBX,ECX,ESI,EDI);
    break;
  case 227 : 
    // long sys_lsetxattr(const char __user *path, const char __user *name,
    //                    const void __user *value, size_t size, int flags);
    IFLS_SSII(LSETXATTR,EBX,ECX,ESI,EDI);
    break;
  case 228 : 
    //  long sys_fsetxattr(int fd, const char __user *name,
    //                     const void __user *value, size_t size, int flags);
    IFLS_ISII(FSETXATTR,EBX,ECX,ESI,EDI);
    break;
  case 229 : 
    // ssize_t sys_getxattr(const char __user *path, const char __user *name,
    //              	    void __user *value, size_t size);
    IFLS_SSI(GETXATTR,EBX,ECX,ESI);
    break;
  case 230 : 
    // ssize_t sys_lgetxattr(const char __user *path, const char __user *name,
    //                       void __user *value, size_t size);
    IFLS_SSI(LGETXATTR,EBX,ECX,ESI);
    break;
  case 231 : // sys_fgetxattr
    // ssize_t sys_fgetxattr(int fd, const char __user *name,
    //                       void __user *value, size_t size);
    IFLS_ISI(FGETXATTR,EBX,ECX,ESI);
    break;
  case 232 : 
    // ssize_t sys_listxattr(const char __user *path, char __user *list,
    //	                     size_t size);
    IFLS_SI(LISTXATTR,EBX,EDX);
    break;
  case 233 :
    // ssize_t sys_llistxattr(const char __user *path, char __user *list,
    //                        size_t size);
    IFLS_SI(LLISTXATTR,EBX,EDX);
    break;
  case 234 : 
    // ssize_t sys_flistxattr(int fd, char __user *list, size_t size);
    IFLS_II(FLISTXATTR,EBX,EDX);
    break;
  case 235 : 
    // long sys_removexattr(const char __user *path,
    //                      const char __user *name);
    IFLS_SS_SIMP(REMOVEXATTR,EBX,ECX);
    break;
  case 236 :     
    // long sys_lremovexattr(const char __user *path,
    //                       const char __user *name);
    IFLS_SS_SIMP(LREMOVEXATTR,EBX,ECX);
    break;
  case 237 : 
    // long sys_fremovexattr(int fd, const char __user *name);
    IFLS_IS(FREMOVEXATTR,EBX,ECX);
    break;
  case 238 : 
    // long sys_tkill(int pid, int sig);
    IFLS_II(TKILL,EBX,ECX);
    break;
  case 239 : 
    //  ssize_t sys_sendfile64(int out_fd, int in_fd,
    //                         loff_t __user *offset, size_t count);
    IFLS_III(SENDFILE64,EBX,ECX,ESI);
    break;
  case 240 : 
    // long sys_futex(u32 __user *uaddr, int op, u32 val,
    //                struct timespec __user *utime, u32 __user *uaddr2,
    //                u32 val3);
    IFLS_I(FUTEX,ECX);
    break;
  case 241 : 
    // long sys_sched_setaffinity(pid_t pid, unsigned int len,
    //                            unsigned long __user *user_mask_ptr);
    IFLS_II(SCHED_SETAFFINITY,EBX,ECX);
    break;
  case 242 : 
    // long sys_sched_getaffinity(pid_t pid, unsigned int len,
    //                            unsigned long __user *user_mask_ptr);
    IFLS_II(SCHED_GETAFFINITY,EBX,ECX);
    break;
  case 243 :
    // sys_set_thread_area is missing from syscalls.h    
    IFLS(SET_THREAD_AREA);
    break;
  case 244 :
    // sys_get_thread_area is missing from syscall.h
    IFLS(GET_THREAD_AREA);
    break;
  case 245 : 
    //  sys_io_setup(unsigned nr_reqs, aio_context_t __user *ctx);
    IFLS(IO_SETUP);
    break;
  case 246 : 
    // long sys_io_destroy(aio_context_t ctx);
    IFLS(IO_DESTROY);
    break;
  case 247 : 
    //  long sys_io_getevents(aio_context_t ctx_id,
    //                        long min_nr,
    //                        long nr,
    //                        struct io_event __user *events,
    //                        struct timespec __user *timeout);
    IFLS(IO_GETEVENTS);
    break;
  case 248 :
    // long sys_io_submit(aio_context_t, long,
    //                    struct iocb __user * __user *);
    IFLS(IO_SUBMIT);
    break;
  case 249 : 
    // long sys_io_cancel(aio_context_t ctx_id, struct iocb __user *iocb,
    //                    struct io_event __user *result);
    IFLS(IO_CANCEL);
    break;
  case 250 : 
    // sys_fadvise64(int fd, loff_t offset, size_t len, int advice);
    IFLS_IIII(FADVISE64,EBX,ECX,EDX,ESI);
    break;
  case 252 : 
    // void sys_exit_group(int error_code);
    IFLS_I(EXIT_GROUP,EBX);
    break;
  case 253 : 
    //  long sys_lookup_dcookie(u64 cookie64, char __user *buf, size_t len);
    IFLS(LOOKUP_DCOOKIE);
    break;
  case 254 :
    // long sys_epoll_create(int size);
    IFLS_I(EPOLL_CREATE,EBX);
    break;
  case 255 :
    //  long sys_epoll_ctl(int epfd, int op, int fd,
    //                     struct epoll_event __user *event);
    IFLS_III(EPOLL_CTL,EBX,ECX,EDX);
    break;
  case 256 : 
    // ong sys_epoll_wait(int epfd, struct epoll_event __user *events,
    //                    int maxevents, int timeout);
    IFLS_III(EPOLL_WAIT,EBX,EDX,ESI);
    break;
  case 257 : 
    // long sys_remap_file_pages(unsigned long start, unsigned long size,
    //                           unsigned long prot, unsigned long pgoff,
    //                           unsigned long flags);
    IFLS_IIIII(REMAP_FILE_PAGES,EBX,ECX,EDX,ESI,EDI);
    break;
  case 258 :
    // long sys_set_tid_address(int __user *tidptr);
    IFLS(SET_TID_ADDRESS);
    break;
  case 259 : 
    // long sys_timer_create(clockid_t which_clock,
    //                       struct sigevent __user *timer_event_spec,
    //                       timer_t __user * created_timer_id);
    IFLS_I(TIMER_CREATE,EBX);
    break;
  default:
    IFLS_IIIIIII(UNKNOWN,EAX,EBX,ECX,EDX,ESI,EDI,EBP);
    break;
  }
}




// log syscall originating in an interrupt
void iferret_log_syscall_interrupt(void) {
  iferret_log_syscall(0);
}

// log syscall originating in a sysenter
void iferret_log_syscall_sysenter(void) {
  iferret_log_syscall(1);
}
