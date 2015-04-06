#ifndef TS_SIZE
#define TS_SIZE  1712
#endif

#ifdef IS_IRET
			current_task = old_esp & (~8191UL);
#else
			current_task = ESP & (~8191UL);
#endif
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
		fprintf(logfile, "returning from PID=%d EAX:%d\n",pid,EAX);
		
		#ifdef IS_IRET
		//fprintf(logfile,"new_eip: 0x%08x\n", new_eip);
	 	paddr = cpu_get_phys_page_debug(env, ESP+4*3);
                if (paddr!=-1) {
                      cpu_physical_memory_read(paddr, &stack_val, 4);
                }
		
		offset = 0; 
		do{
			syscall_element = find_element(pid,offset);
			offset++;
		}while(syscall_element.eip != -1 && syscall_element.eip != new_eip && syscall_element.eip != stack_val);
		if(syscall_element.eip != -1){
			syscall_num = syscall_element.syscall_num;
			//printf("iret\n");	
			//printf("offset %d syscall_element.eip 0x%08x new_eip 0x%08x stack_val 0x%08x\n", offset, syscall_element.eip, new_eip, stack_val);
			fprintf(logfile,"matched EIP:0x%08x Num:%d\n",syscall_element.eip, syscall_element.syscall_num);
			del_element(pid,offset-1);
		}else{
			syscall_num = -1;
		}
		#else //sysexit method
	 	paddr = cpu_get_phys_page_debug(env, ECX+4*3);
                 if (paddr!=-1) {
                       cpu_physical_memory_read(paddr, &stack_val, 4);
                 }
		
		offset = 0;
                do{
                        syscall_element = find_element(pid,offset);
                        offset++;
                }while(syscall_element.eip != -1 && syscall_element.eip != stack_val);	
		
		syscall_num = syscall_element.syscall_num;
		if(syscall_num != -1){
			//printf("sysexit\n");
			del_element(pid,offset-1);
			fprintf(logfile,"matched EIP:0x%08x Num:%d\n",syscall_element.eip, syscall_element.syscall_num);
		}else{
			printf("PID %3d (%16s)", pid, command);
			printf("Error, unmatched sysexit, AFAIK not possible, stack_val:0x%08x\n", stack_val);
			//exit(1);	
		}
   		#endif	
	
//pid = 1337;
//command = "W3 |33 31337!";
	     	    switch (syscall_num) {	
			case -1: //do nothing, not a system call
			   // fprintf(logfile,"PID %3d (%16s)", pid, command);
			   // fprintf(logfile,"[unmatched syscall_return -1]: %d\n", EAX);
			    break;
                    	case 4 : // sys_write
			    fprintf(logfile,"PID %3d (%16s)", pid, command);
			    fprintf(logfile,"[sys_write return  4]: %d\n", EAX);
			    break;
			case 5 : //sys_open
			    fprintf(logfile,"PID %3d (%16s)", pid, command);
			    fprintf(logfile,"[sys_open return  5]: %d\n", EAX);
			    break;
                    	case 6 : // sys_close
				fprintf(logfile,"PID %3d (%16s)", pid, command);
				fprintf(logfile,"[sys_close return  6]: %d\n", EAX);
				break;
			default:
			    fprintf(logfile,"PID %3d (%16s)", pid, command);
			    fprintf(logfile,"[unknown syscall return  %d]: %d\n", syscall_num, EAX);
}
