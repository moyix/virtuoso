/* ©2011 Massachusetts Institute of Technology */
#define vm_mark_buf_in(x,y) \
    { \
    void *b = x; \
    int n = y; \
    asm __volatile__ ("push %%eax\n\t" \
         "push %%ebx\n\t" \
         "push %%ecx\n\t" \
         "push %%edx\n\t" \
         "mov $0, %%eax\n\t" \
         "mov $0xdeadbeef, %%ebx\n\t" \
         "mov %0, %%ecx\n\t" \
         "mov %1, %%edx\n\t" \
         "cpuid\n\t" \
         "pop %%edx\n\t" \
         "pop %%ecx\n\t" \
         "pop %%ebx\n\t" \
         "pop %%eax\n\t" \
         : \
         : "r" (b), \
           "r" (n) \
         : "%eax", "%ebx", "%ecx", "%edx" \
    ); \
    }

#define vm_mark_buf_out(x,y) \
    { \
    void *b = x; \
    int n = y; \
    asm __volatile__ ("push %%eax\n\t" \
         "push %%ebx\n\t" \
         "push %%ecx\n\t" \
         "push %%edx\n\t" \
         "mov $0, %%eax\n\t" \
         "mov $0xdeadf00d, %%ebx\n\t" \
         "mov %0, %%ecx\n\t" \
         "mov %1, %%edx\n\t" \
         "cpuid\n\t" \
         "pop %%edx\n\t" \
         "pop %%ecx\n\t" \
         "pop %%ebx\n\t" \
         "pop %%eax\n\t" \
         : \
         : "r" (b), \
           "r" (n) \
         : "%eax", "%ebx", "%ecx", "%edx" \
    ); \
    }
