#ifndef _IFERRET_SYSCALL_H_
#define _IFERRET_SYSCALL_H_


void iferret_log_syscall_enter (uint8_t is_sysenter, uint32_t callsite_eip);

void iferret_log_syscall_ret_sysexit(int callsite_esp);

void iferret_log_syscall_ret_iret(int callsite_esp, uint32_t callsite_eip);


#endif
