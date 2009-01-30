#ifndef _IFERRET_SYSCALL_H_
#define _IFERRET_SYSCALL_H_

void iferret_log_syscall_interrupt(void);

void iferret_log_syscall_sysenter(void);

void iferret_log_syscall_ret_sysexit(int callsite_esp);

void iferret_log_syscall_ret_iret(int callsite_esp, uint32_t callsite_eip);


#endif
