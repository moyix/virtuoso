#define MAGIC_IN  0xdeadbeef
#define MAGIC_OUT 0xdeadf00d

#define vm_mark_buf_in(buf, len) \
  {\
  void *b = buf; \
  int n = len; \
  \
  __asm { \
    __asm PUSH EAX \
    __asm PUSH EBX \
    __asm PUSH ECX \
    __asm PUSH EDX \
    __asm MOV EAX, 0 \
    __asm MOV EBX, MAGIC_IN \
    __asm MOV ECX, b \
    __asm MOV EDX, n \
    __asm CPUID \
    __asm POP EDX \
    __asm POP ECX \
    __asm POP EBX \
    __asm POP EAX \
  };\
  }

#define vm_mark_buf_out(buf, len) \
  {\
  void *b = buf; \
  int n = len; \
  \
  __asm { \
    __asm PUSH EAX \
    __asm PUSH EBX \
    __asm PUSH ECX \
    __asm PUSH EDX \
    __asm MOV EAX, 0 \
    __asm MOV EBX, MAGIC_OUT \
    __asm MOV ECX, b \
    __asm MOV EDX, n \
    __asm CPUID \
    __asm POP EDX \
    __asm POP ECX \
    __asm POP EBX \
    __asm POP EAX \
  };\
  }
