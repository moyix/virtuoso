/*
#define FC3
#ifndef TS_SIZE
#ifdef RH80
#define TS_SIZE  1440
#endif
#ifdef FC3
#define TS_SIZE  1360
#endif
#endif
*/
#ifndef TS_SIZE
#define TS_SIZE  1712
#endif

			current_task = ESP & (~8191UL);
			paddr = cpu_get_phys_page_debug(env, current_task); 
			if (paddr!=-1) {
				cpu_physical_memory_read(paddr, &current_task, 4);
			}
			pid = 0;
			//paddr = cpu_get_phys_page_debug(env, current_task+0x7c); // rh8.0
			//paddr = cpu_get_phys_page_debug(env, current_task+140); // rh9.0
			//paddr = cpu_get_phys_page_debug(env, current_task+136);  //fc2
			//paddr = cpu_get_phys_page_debug(env, current_task+156);  //fc5
			paddr = cpu_get_phys_page_debug(env, current_task+204); // Ubuntu 2.6.24-19-generic 
			if (paddr!=-1) {
				cpu_physical_memory_read(paddr, &pid, 4);
			}
			bzero(command, 120);
			//paddr = cpu_get_phys_page_debug(env, current_task+0x23e); // task_struct->comm redhat 8.0
			//paddr = cpu_get_phys_page_debug(env, current_task+766); // task_struct->comm redhat 9.0
			//paddr = cpu_get_phys_page_debug(env, current_task+610); // fc2
			//paddr = cpu_get_phys_page_debug(env, current_task+432); // fc5
			paddr = cpu_get_phys_page_debug(env, current_task+461); // task_struct->comm ubuntu 2.6.24-19-generic
			if (paddr!=-1) {
				 cpu_physical_memory_read(paddr, command, 16);
				 // fprintf(logfile,"PID %3d (%16s)", pid, command);
			}
//		paddr = cpu_get_phys_page_debug(env, current_task); 
//		 cpu_physical_memory_read(paddr, tsbuf, 1000);
//		fprintf(logfile,"\n task_struct: 0x%08x \n",*(int*)tsbuf);
/*		for(i=0;i<1000;i++){
			if(!(i%16))
				fprintf(logfile,"\n%d: ", i);
			fprintf(logfile,"%c", tsbuf[i]);
		}
		fprintf(logfile,"\n");
*/

		if(!table_initialized){
			init_table();
			table_initialized = 1;	
		}
		//printf("adding element pid=%d, EAX=%d\n",pid,EAX);
/* old way
		#ifdef IS_SYSENTER
		 paddr = cpu_get_phys_page_debug(env, saved_esp+4*3);
                 if (paddr!=-1) {
                       cpu_physical_memory_read(paddr, &stack_val, 4);
                 }
		old_syscall_num = add_element(pid,stack_val,EAX);
		fprintf(logfile,"Storing EIP:0x%08x\n", stack_val);

		old_syscall_num = add_element(pid,EIP+11,EAX);
		fprintf(logfile,"Storing EIP:0x%08x\n", EIP+11);
		
		#else
		add_element(pid,old_eip,EAX);
		fprintf(logfile,"Storing EIP:0x%08x\n", old_eip);
		#endif
		if(old_syscall_num != 119 && old_syscall_num != 11)
			fprintf(logfile,"PID %3d (%16s)[unmatched syscall %d]\n", pid, command,old_syscall_num);
*/
	
		#ifdef IS_SYSENTER
		 paddr = cpu_get_phys_page_debug(env, saved_esp+4*3);
                 if (paddr!=-1) {
                       cpu_physical_memory_read(paddr, &stack_val, 4);
                 }else{
			printf("paddr is -1, oops!\n");	
			exit(1);
		}
		add_element(pid,stack_val,EAX);
		fprintf(logfile,"sys_enter Storing EIP:0x%08x\n", stack_val);
		#else
		if(EAX != 11 && EAX != 119){
			add_element(pid,old_eip,EAX);
			fprintf(logfile,"int Storing EIP:0x%08x\n", old_eip);
		}
		#endif
		fprintf(logfile, "PID: %d, stack size:%d\n",pid,get_stack_size(pid));
//pid = 1337;
//command = "W3 |33 31337!";
	     	    switch (EAX) {	
/*                    	case 0 : // sys_ni_syscall
				fprintf(logfile,"PID %3d (%16s)[sys_ni_sysc   0]\n", pid, command);
				break;
                    	case 1 : // sys_exit
				fprintf(logfile,"PID %3d (%16s)[sys_exit      1]\n", pid, command);
				break;
                    	case 2 : // sys_fork
				fprintf(logfile,"PID %3d (%16s)", pid, command);
				fprintf(logfile,"[sys_fork      2]: \n");
				break;
                    	case 3 : // sys_read
				fprintf(logfile,"PID %3d (%16s)", pid, command);
				fprintf(logfile,"[sys_read      3]: %d\n", EBX);
				break;
*/
                    	case 4 : // sys_write
				{
				int size=30;
                    		paddr = cpu_get_phys_page_debug(env, ECX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_write     4]: %d ", EBX);

				    bzero(tempbuf, 120);
				    if (EDX>0 && EDX <= 30) size = EDX; 
                        	    cpu_physical_memory_read(paddr, tempbuf, size); 
 	                            for (i=0; i<size; i++)
                                        if (iscntrl(tempbuf[i])) tempbuf[i]='.';
				    fprintf(logfile,"(%s)(%d)\n", tempbuf, EDX);
		    		} 
				}
				break;
                    	case 5 : // sys_open
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_open      5]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 119); //-> get the file name
				    fprintf(logfile,"%s; flags %d\n", tempbuf, ECX);
		    		} 
				break;
                    	case 6 : // sys_close
				fprintf(logfile,"PID %3d (%16s)", pid, command);
				fprintf(logfile,"[sys_close     6]: %d\n", EBX);
				break;
/*                    	case 7 : // sys_waitpid
				fprintf(logfile,"PID %3d (%16s)[sys_waitpid   7]\n", pid, command);
				break;
                    	case 8 : // sys_creat
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_creat     8]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				    fprintf(logfile,"%s (mode %d)\n", tempbuf, ECX);
		    		} 
				break;
                    	case 9: // sys_link
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_link      9]: ");
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
                    	case 10 : // sys_unlink
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_unlink   10]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				    fprintf(logfile,"%s\n", tempbuf);
				}
				break;
                    	case 11: // sys_execve

                    		paddr = cpu_get_phys_page_debug(env, ECX);
                        	cpu_physical_memory_read(paddr, &argvp, 4);  // The first argument

				if (argvp) {
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_execve   11]: ");
				}

				pid = 0;
				while (argvp) {
				    bzero(command, 120);
                    		    paddr = cpu_get_phys_page_debug(env, argvp);
                        	    cpu_physical_memory_read(paddr, command, 120);  // The first argument
				    fprintf(logfile,"%s ", command);
				    pid++;
				    if (pid >12) break;
                    		    //paddr = cpu_get_phys_page_debug(env, argvp);
                    		    paddr = cpu_get_phys_page_debug(env, ECX+pid*4);
                        	    cpu_physical_memory_read(paddr, &argvp, 4);  // The first argument
				}
				//fprintf(logfile,"(size %d)\n", pid);
				if (pid) fprintf(logfile,"\n");
				break;
                    	case 12 : // sys_chdir
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_chdir    12]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
				    fprintf(logfile,"%s\n", tempbuf);
		    		} 
				break;
                    	case 13 : // sys_time
				fprintf(logfile,"PID %3d (%16s)[sys_time     13]\n", pid, command);
				break;
                    	case 14 : // sys_mknod
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_mknod    14]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
				    fprintf(logfile,"%s (mode %d, dev_t %d)\n", tempbuf, ECX, EDX);
		    		} 
				break;
                    	case 15 : // sys_chmod
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_chmod    15]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
				    fprintf(logfile,"%s (mode %d)\n", tempbuf, ECX);
		    		} 
				break;
                    	case 16 : // sys_lchown16
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_lchown   16]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
				    fprintf(logfile,"%s (uid %d, gid %d)\n", tempbuf, ECX, EDX);
		    		} 
				break;
                    	case 17 : // sys_ni_syscall
				fprintf(logfile,"PID %3d (%16s)[sys_ni_sysc  17]\n", pid, command);
				break;
                    	case 18 : // sys_stat
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_stat     18]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
				    fprintf(logfile,"%s\n", tempbuf);
		    		} 
				break;
                    	case 19 : // sys_lseek
				fprintf(logfile,"PID %3d (%16s)", pid, command);
				fprintf(logfile,"[sys_lseek    19]: fd %d; offset %d; origin %d\n", EBX, ECX, EDX);
				break;
                    	case 20 : // sys_getpid
				fprintf(logfile,"PID %3d (%16s)[sys_getpid   20]\n", pid, command);
				break;
                    	case 21: // sys_mount
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_mount    21]: ");
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
                    	case 22 : // sys_oldumount
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_oldumoun 22]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
				    fprintf(logfile,"%s\n", tempbuf);
		    		} 
				break;
                    	case 23 : // sys_setuid16
				fprintf(logfile,"PID %3d (%16s)[sys_setuid16 23] %d\n", pid, command, EBX);
				break;
                    	case 24 : // sys_getuid16
				fprintf(logfile,"PID %3d (%16s)[sys_getuid16 24]\n", pid, command);
				break;
                    	case 25 : // sys_stime
				fprintf(logfile,"PID %3d (%16s)[sys_stime    25]\n", pid, command);
				break;
                    	case 26 : // sys_ptrace
				fprintf(logfile,"PID %3d (%16s)[sys_ptrace   26]: ", pid, command);
				fprintf(logfile,"request %d; pid %d; addr 0x%08x\n", EBX, ECX, EDX);
				break;
                    	case 27 : // sys_alarm
				fprintf(logfile,"PID %3d (%16s)[sys_alarm    27]\n", pid, command);
				break;
                    	case 28 : // sys_fstat
				//fprintf(logfile,"PID %3d (%16s)[sys_fstat    28]: %d\n", pid, command, EBX);
                                paddr = cpu_get_phys_page_debug(env, EBX);
                                if (paddr!=-1)  {
                                    fprintf(logfile,"PID %3d (%16s)", pid, command);
                                    fprintf(logfile,"[sys_fsta     28]: ");
                                    bzero(tempbuf, 120);
                                    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
                                    fprintf(logfile,"%s\n", tempbuf);
                                }

				break;
                    	case 29 : // sys_pause
				fprintf(logfile,"PID %3d (%16s)[sys_pause    29]\n", pid, command);
				break;
                    	case 30 : // sys_utime
				break;
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_utime    30]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
				    fprintf(logfile,"%s (times  0x%08x)\n", tempbuf, ECX);
		    		} 
                    	case 31 : // sys_ni_syscall
				fprintf(logfile,"PID %3d (%16s)[sys_ni_sysc  31]\n", pid, command);
				break;
                    	case 32 : // sys_ni_syscall
				fprintf(logfile,"PID %3d (%16s)[sys_ni_sysc  32]\n", pid, command);
				break;
                    	case 33 : // sys_access
				break;
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_access    33]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
				    fprintf(logfile,"%s (mode %d)\n", tempbuf, ECX);
		    		} 
                    	case 34 : // sys_nice
				fprintf(logfile,"PID %3d (%16s)[sys_nice     34]: %d\n", pid, command, EBX);
				break;
                    	case 35 : // sys_ni_syscall
				fprintf(logfile,"PID %3d (%16s)[sys_ni_sysc  35]\n", pid, command);
				break;
                    	case 36 : // sys_sync
				fprintf(logfile,"PID %3d (%16s)[sys_sysc     36]\n", pid, command);
				break;
                    	case 37 : // sys_kill
				fprintf(logfile,"PID %3d (%16s)[sys_kill     37]: pid %d; sig %d\n", pid, 
					command, EBX, ECX);
				break;
                    	case 38: // sys_rename
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_rename   38]: ");
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
                    	case 39: // sys_mkdir
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_mkdir    39]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				    fprintf(logfile,"%s (mode %d)\n", tempbuf, ECX);

		    		} 
				break;
                    	case 40: // sys_rmdir
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_rmdir    40]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				    fprintf(logfile,"%s\n", tempbuf);

		    		} 
				break;
                    	case 41: // sys_dup
				fprintf(logfile,"PID %3d (%16s)", pid, command);
				fprintf(logfile,"[sys_dup      41]: fildes %d\n", EBX);
				break;
                    	case 42: // sys_pipe
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_pipe     42]\n");

		    		} 
				break;
                    	case 43 : // sys_times
				fprintf(logfile,"PID %3d (%16s)[sys_times    43]\n", pid, command);
				break;
                    	case 44 : // sys_ni_syscall
				fprintf(logfile,"PID %3d (%16s)[sys_ni_sysc  44]\n", pid, command);
				break;
                    	case 45 : // sys_break
				fprintf(logfile,"PID %3d (%16s)[sys_break    45]: 0x%08x\n", pid, command, EBX);
				break;
                    	case 46 : // sys_setgid16
				fprintf(logfile,"PID %3d (%16s)[sys_setgid16 46] %d\n", pid, command, EBX);
				break;
                    	case 47 : // sys_getgid16
				fprintf(logfile,"PID %3d (%16s)[sys_getgid16 47]\n", pid, command);
				break;
                    	case 48 : // sys_signal
				fprintf(logfile,"PID %3d (%16s)[sys_signal   48]: sig %d; handler 0x%08x\n", pid, command, EBX, ECX);
				break;
                    	case 49 : // sys_geteuid16
				fprintf(logfile,"PID %3d (%16s)[sys_geteuid1 49] %d\n", pid, command, EBX);
				break;
                    	case 50 : // sys_getegid16
				fprintf(logfile,"PID %3d (%16s)[sys_getegid1 50]\n", pid, command);
				break;
                    	case 51 : // sys_acct
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_acct     51]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the file name
				    fprintf(logfile,"%s\n", tempbuf);

		    		} 
				break;
                        case 52 : // sys_umount2
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_umount2  52]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
				    fprintf(logfile,"%s\n", tempbuf);

		    		} 
                                break;
                        case 53 : // sys_lock
				fprintf(logfile,"PID %3d (%16s)[sys_ni_sysc  53]\n", pid, command);
                                break;
                        case 54 : // sys_ioctl
                                fprintf(logfile,"PID %3d (%16s)[sys_ioctl    54]: fd %d; cmd %d; arg 0x%08x\n", 
					pid, command, EBX, ECX, EDX);
                                break;
                        case 55 : // sys_fcntl
                                fprintf(logfile,"PID %3d (%16s)[sys_fcntl    55]: fd %d; cmd %d; arg 0x%08x\n", 
					pid, command, EBX, ECX, EDX);
                                break;
                        case 56 : // sys_mpx
				fprintf(logfile,"PID %3d (%16s)[sys_ni_sysc  56]\n", pid, command);
                                break;
                        case 57 : // sys_setpgid
                                fprintf(logfile,"PID %3d (%16s)[sys_setpgid  57]: pid %d; pgid %d\n", pid, command, EBX, ECX);
                                break;
                        case 58 : // sys_ulimit
				fprintf(logfile,"PID %3d (%16s)[sys_ni_sysc  58]\n", pid, command);
                                break;
                        case 59 : // sys_oldolduname
                                fprintf(logfile,"PID %3d (%16s)[sys_oldoldun 59]\n", pid, command);
                                break;
                        case 60 : // sys_umask
                                fprintf(logfile,"PID %3d (%16s)[sys_umask    60]: mask %d(0x%x)\n", pid, command, EBX,EBX);
                                break;
                        case 61 : // sys_chroot
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_chroot   61]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
				    fprintf(logfile,"%s\n", tempbuf);
				}
                                break;
                        case 62 : // sys_ustat
                                fprintf(logfile,"PID %3d (%16s)[sys_ustat    62]\n", pid, command);
                                break;
                        case 63 : // sys_dup2
                                fprintf(logfile,"PID %3d (%16s)[sys_dup2     63]: oldfd %d; newfd %d\n", pid, command, EBX, ECX);
                                break;
                        case 64 : // sys_getppid
                                fprintf(logfile,"PID %3d (%16s)[sys_getppid  64]\n", pid, command);
                                break;
                        case 65 : // sys_getpgrp
                                fprintf(logfile,"PID %3d (%16s)[sys_getpgrp  65]\n", pid, command);
                                break;
                        case 66 : // sys_setsid
                                fprintf(logfile,"PID %3d (%16s)[sys_setsid   66]\n", pid, command);
                                break;
                        case 67 : // sys_sigaction
                                fprintf(logfile,"PID %3d (%16s)[sys_sigactio 67]: sig %d\n", pid, command, EBX);
                                break;
                        case 68 : // sys_sgetmask
                                fprintf(logfile,"PID %3d (%16s)[sys_sgetmask 68]\n", pid, command);
                                break;
                        case 69 : // sys_ssetmask
                                fprintf(logfile,"PID %3d (%16s)[sys_ssetmask 69]: newmask %d\n", pid, command, EBX);
                                break;
                        case 70 : // sys_setreuid
                                fprintf(logfile,"PID %3d (%16s)[sys_setreuid 70]: ruid %d; euid %d\n", pid, command, EBX, ECX);
                                break;
                        case 71 : // sys_setregid
                                fprintf(logfile,"PID %3d (%16s)[sys_setregid 71]: rgid %d; egid %d\n", pid, command, EBX, ECX);
                                break;
                        case 72 : // sys_sigsuspend
                                fprintf(logfile,"PID %3d (%16s)[sys_sigsuspe 72]\n", pid, command);
                                break;
                        case 73 : // sys_sigpending
                                fprintf(logfile,"PID %3d (%16s)[sys_sigpendi 73]\n", pid, command);
                                break;
                        case 74 : // sys_sethostname
                    		paddr = cpu_get_phys_page_debug(env, EBX);
		        	if (paddr!=-1)	{
				    fprintf(logfile,"PID %3d (%16s)", pid, command);
				    fprintf(logfile,"[sys_sethostn 74]: ");
				    bzero(tempbuf, 120);
                        	    cpu_physical_memory_read(paddr, tempbuf, 120); //-> get the dir name
				    fprintf(logfile,"%s\n", tempbuf);
				}
                                break;
                        case 75 : // sys_setrlimit
                                fprintf(logfile,"PID %3d (%16s)[sys_setrlimi 75]: resource %d; rlimit 0x%08x\n", 
					pid, command, EBX, ECX);
                                break;
                        case 76 : // sys_getrlimit
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
				case 2: // bindb
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
