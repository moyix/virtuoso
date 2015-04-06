# Virtuoso Walkthrough #

This document describes how to use Virtuoso to generate memory analysis plugins automatically, based on live analysis programs. We'll do this by walking through the generation of a process listing plugin for the Haiku OS ([R1](https://code.google.com/p/virtuoso/source/detail?r=1) alpha2). This walkthrough assumes that you've already compiled Virtuoso and installed any necessary dependencies; if you haven't, check out the [installation guide](Installation.md).

## Training Programs ##

Training programs are small programs, written for the OS that you want to analyze, that calculate whatever it is you'd like your generated Volatility plugin to calculate. For example, if you wanted to generate a Volatility plugin to get a list of running process IDs (PIDs), you could write an introspection program like this:

```
#define __WIN32_LEAN_AND_MEAN__
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#include <stdio.h>
#include "vmnotify.h"

int main(int argc, char **argv) {
  DWORD *pids, outcb;
  pids = (DWORD *) malloc(256);
  vm_mark_buf_in(&pids, 4);
  EnumProcesses(pids, 256, &outcb);
  vm_mark_buf_out(pids, 256);
  return 0;
}
```

This uses the [EnumProcesses](http://msdn.microsoft.com/en-us/library/ms682629(v=vs.85).aspx) command to get a list of PIDs running on the system, and places it into the pids buffer. This is a very simple example, but it is all we need to produce a working Volatility plugin to list PIDs in a Windows memory image!

There are a few things about this code snippet that are specific to Virtuoso. The first is the extra include file:

```
#include "vmnotify.h"
```

This is a file included in the Virtuoso distribution under the `introprog` directory (there's one for each OS -- or more accurately, there are two versions, one for gcc and one for Microsoft's compiler). It provides definitions for the two macros Virtuoso needs:

```
vm_mark_buf_in(input buffer, size);
vm_mark_buf_out(output buffer, size);
```

These two macros are used to let **iFerret**, the tracing portion of Virtuoso, know where the inputs and outputs to your program are located, and how big they are. Currently, multiple inputs are supported, but multiple outputs are not. If you need multiple outputs, one workaround is to create an output buffer big enough to hold all your outputs, and then copy each output into it.

## A Haiku Process Lister Training Program ##

For this walkthrough, we'll be creating a process listing program for Haiku. This is conceptually the same as the training program we just saw for Windows, but uses the Haiku APIs.

The process lister is included in `introprog/haiku/enumprocs.c`; I have reproduced it here for reference:

```
#include <kernel/OS.h>
#include <stdio.h>
#include "vmnotify.h"

int32 real_list_procs(int32 *pids) {
    team_info info;
    int32 i = 0;
    int32 cookie = 0;
    while (get_next_team_info(&cookie, &info) == B_OK) {
        pids[i] = info.team;
        i++;
    }
    return i;
}

int32 list_procs(int32 *pids) {
    int i;
    vm_mark_buf_in(&pids, 4);
    i = real_list_procs(pids);
    vm_mark_buf_out(pids, 1024);
    return i;
}

int main(int argc, char **argv) {
    int32 count, i;
    thread_id foo;
    int32 *pids = (int32 *)malloc(1024);
    foo = spawn_thread(list_procs, "list_procs", B_REAL_TIME_PRIORITY, pids);
    resume_thread(foo);
    wait_for_thread(foo, &count);
    for(i = 0; i < count; i++) {
        printf("%d\n", pids[i]);
    }
    return 0;
}
```

The core of this program is the call to [get\_next\_team\_info](http://haiku-os.org/legacy-docs/bebook/TheKernelKit_ThreadsAndTeams.html#get_next_team_info). As the Haiku documentation says, this function can be called multiple times to retrieve information about each running "team" (Haiku's equivalent of a process). Inside the `list_procs` function, we can see that `vm_mark_buf_in` and `vm_mark_buf_out` are used to tell Virtuoso where the inputs and outputs to the program are.

There are a few other features of this program that are worth noting, as they point out some limitations of Virtuoso. First, you'll notice that the calls to `get_next_team_info` are encapsulated in their own function, `real_list_procs`, rather than just being placed in `list_procs`. This is because Virtuoso only records things that happen in between the first `vm_mark_buf_in` and the last `vm_mark_buf_out`. Because `get_next_team_info` needs the `cookie` variable, its declaration and initialization must be part of the code we record. However, since C doesn't permit variable declarations in the middle of functions, we just put the whole thing into its own function.

Second, in the `main` function, we don't call `list_procs` directly. Instead, we spawn a new thread and give it realtime priority; that thread then executes `list_procs`. This is due to a limitation in Virtuoso: all of the code that is recorded must run within the same process. This means that if the OS switches away to another process in between the two `vm_mark_buf` markers, we won't be able to generate a plugin successfully. To get around this, we give our thread the highest priority possible. To accomplish the same thing on Windows, you can start the program using `start /realtime [program]`; on Linux, you can use `chrt -f 99 [program]` as root.

## Running the Training Program ##

Once you've got a training program, you'll want to run it inside **iFerret**. To do that, you'll need to have a copy of the OS you intend to analyze running inside **iFerret** (which, you recall, is just a modified version of QEMU). For this walkthrough, you can download a [premade virtual machine](http://amnesia.gtisc.gatech.edu/~moyix/haiku-r1alpha2-anyboot.qcow2.bz2) of Haiku [R1](https://code.google.com/p/virtuoso/source/detail?r=1) Alpha 2. The virtual machine comes with a snapshot named `introprog` that has a few training programs already loaded and compiled.

Because **iFerret** is a modified version of QEMU, the usage is essentially the same. There are a couple extra options needed that are specific to Virtuoso, though. Here's the command line you'd use to boot the image linked above, load the `introprog` snapshot, and save traces to a logfile named `walkthrough`:

```
install/bin/qemu -m 256 \
    -hda haiku-r1alpha2-anyboot.qcow2 \
    -usbdevice tablet -loadvm introprog \
    -monitor stdio -k en-us \
    -iferret_log walkthrough
```

The last argument (`-iferret_log`) is the only one that is specific to Virtuoso; the rest are documented in the QEMU manual.

If you're not working off the provided virtual machine image, you'll have to transfer the source code to the training program into the VM and compile it. To transfer the code into the VM, you can either put it somewhere network accessible and then download it inside the VM, or create an ISO image containing the code and then use QEMU's `-cdrom` argument to make QEMU load the ISO as a CDROM drive. You'll also have to compile the source code; luckily, Haiku comes with gcc, so this is relatively simple: `gcc enumprocs.c -o enumprocs`.

Now, you can run the training program inside **iFerret** to produce the instruction traces from which we'll generate our Volatility program:

```
./enumprocs
```

Each time you run the command, you'll see the emulator freeze for a second while it runs the program and generates the log. Once it's done, you should find the following files:

  * `walkthrough.0-1234-0`
  * `walkthrough.0.env`
  * `walkthrough.0.mem`

If you run the command multiple times, you'll get multiple output files, for example `walkthrough.1-1234-0`. Running the command multiple times can sometimes be necessary if there are multiple paths through the program that we need to capture, but for now we'll just run it once and move on and analyze the logfile using **dynslicer**. The `.env` and `.mem` files produced are a snapshot of the CPU and memory state at the time the program was run; this can be handy for testing and debugging the generated programs.

## Analyzing the Logfile with Dynslicer ##

Not every instruction we just recorded was actually necessary to computing the list of running PIDs. On most operating systems, there will be a number of other things going on, and so some unnecessary stuff will be recorded along with the operations we want to capture. Dynslicer is designed to use data flow analysis to figure out precisely what parts of the log file need to be kept in order to compute the data we marked in the output buffer with `vm_mark_buf_out`.

To perform this analysis and produce the Volatility plugin, go to the `dynslicer` directory and run:

```
./newslice.py -o haiku ../iferret-logging-new/walkthrough.0-1234
```

The `-o haiku` tells Virtuoso what OS is in use. In theory this shouldn't be necessary -- by design, Virtuoso is completely agnostic to the operating system used. However, some operating systems use some features in their memory allocation functions that Virtuoso doesn't support yet, so the OS is used to figure out where the memory allocation functions are and replace them with versions that are simpler for Virtuoso to analyze. Also note that we omit the final `-0` on the log file name -- this is in case we have a log file that was too large to fit in a single file; in this case, the logfiles will be given suffixes `-0`, `-1`, etc., and Virtuoso will recombine them.

The command given above produces Python code that we can use with Volatility. These files are given the same name as the logfile by default, with the extension `.pkl`. To change the output file name, you can use the `-O` option with `newslice.py`.

## Running the Generated Plugin on a Memory Image ##

Finally, we can run our plugin on a Haiku memory image. For reference, I've put up one on my web site:

  * [haiku.mem](http://amnesia.gtisc.gatech.edu/~moyix/haiku.mem) - The memory image
  * [haiku.env](http://amnesia.gtisc.gatech.edu/~moyix/haiku.env) - The corresponding CPU state

The `.env` file is necessary because the code we've generated is essentially a direct translation of the original x86 binary code; this means it needs a CPU context in which to execute. The `.env` file provides such a context.

To run the newly generated plugin against the memory image, go into the `Volatility-1.3_Beta` directory and run:

```
./volatility newmicrodo \
    -f haiku.mem \
    -e haiku.env \
    -m ../iferret-logging-new/walkthrough.pkl \
    -n '[ mem.alloc(1024) ]'
```

The `-f` and `-e` options tell Volatility where to find the memory and CPU state, respectively. The `-m` option tells Volatility where to find the plugin we just generated. Finally, the `-n` option fills in the inputs to the program -- in this case, a buffer where the PIDs will be placed. Virtuoso can allocate temporary buffers if necessary using the `mem.alloc` command. You should get output like:

```
Time taken: 629.324198 ms

Output: 010000002b0000003200000033000000340000003e000000440000004700000052000000590000005a0000005d0000005e0000005f0000006300000072000000b6000000ba000000ef000000
```

It seems to have worked, but the output is a little ugly -- we're just getting a raw hex dump of the output buffer. We can clean it up a little by passing an _interpretation function_ using the `-i` option:

```
./volatility newmicrodo \
    -f haiku.mem \
    -e haiku.env \
    -m ../iferret-logging-new/walkthrough.pkl \
    -n '[ mem.alloc(1024) ]' \
    -i 'def f(x): print unpack("<%dI" % (len(x)/4),x)'
```

This option tells `newmicrodo` that it should parse the data using a function that uses Python's [struct.unpack](http://docs.python.org/library/struct.html) function to decode the data as integers. When we do this, we get much nicer output:

```
Decoded data:
(1, 43, 50, 51, 52, 62, 68, 71, 82, 89, 90, 93, 94, 95, 99, 114, 182, 186, 239)
```

## Next Steps ##

So where do you go from here? Well, go off and write your own training programs and generate your own plugins! There are some [limitations and caveats](Limitations.md) (this is, after all, a research project), but hopefully you'll find it useful. Good luck!