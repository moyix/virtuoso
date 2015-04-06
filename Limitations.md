# Limitations #

  * Virtuoso relies on iFerret for tracing, which is based on QEMU 0.9.1. This version of QEMU is getting quite old at this point, and is unable to run many modern operating systems, including Windows Vista. Unfortunately, the translation engine changed starting with QEMU 0.10, and so porting Virtuoso to a more recent version of QEMU would take significant effort.
  * Virtuoso makes use of libdasm to disassemble instructions (see `iferret-logging-new/target-i386/translate.c` for details). Libdasm is missing support for some instructions, and this will cause tracing to stop and QEMU to shut down. The disassembly Virtuoso does is mainly for debugging, and so these lines can be commented out if they cause trouble. I would like to switch to a more reliable disassembler in the future, however.
  * All code you want to extract must run in a single process, no task switches can occur while tracing is in progress. In most operating systems, you can set the process priority so that this is the case:
    * In Windows, launch the process using `start /realtime [prog.exe]`
    * In Linux, launch the process using `chrt -f 99 [prog]`
    * In Haiku, use the spawn\_thread API and pass B\_REAL\_TIME\_PRIORITY for the priority (an example of this can be found in the [Walkthrough](Walkthrough.md).
  * There may be QEMU operations that are not supported by the slicing and translation code. If Virtuoso encounters one, it will report the name of the operation and quit. New operations can be added to the slicing and translation code by editing `dynslicer/qemu_data.py` and `dynslicer/translate_uop.py`; if you do add any, please contribute the changes back so everyone can benefit!
  * Self-modifying or dynamically generated code is not supported by the emulator, and so code that relies on self-modification or runtime code generation cannot be extracted by Virtuoso.
  * Code which uses device I/O cannot be extracted by Virtuoso.