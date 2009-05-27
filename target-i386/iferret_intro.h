#ifndef __INTROSPECTION_H_
#define __INTROSPECTION_H_

#include <stddef.h>
#include <stdint.h>

#define OS_LINUX    1
#define OS_WINXPSP2 2

#include "iferret_intro_xpsp2.h"

// BDG 05/19/2009
// This is some magic: the use of sizeof here
// is of the form sizeof [expresssion]. Although
// it looks like we're dereferencing NULL, this code
// never actually makes it into the compiled program--
// it just fools sizeof into getting the size of a
// structure member.
#define membsize(x,y) sizeof ((x *)NULL)->y
#define read_member(env,addr,strct,memb,dest) cpu_virtual_memory_read(env, addr+offsetof(strct,memb), (char *) dest, membsize(strct,memb))

#endif
