make distclean

cd target-i386
./make_iferret_code.pl

cd ..
 ./configure --disable-kqemu --prefix=/home/tleek/hg/iferret-logging-new/install --target-list=i386-softmmu  --disable-linux-user --disable-darwin-user \
 --extra-cflags="-DIFERRET_SYSCALL"
# --extra-cflags="-DIFERRET_SYSCALL -DIFERRET_PUID"

# --extra-cflags="-DIFERRET_SYSCALL -DIFERRET_INFO_FLOW -DIFERRET_PUID -DIFERRET_PHYS_EIP" 


# --extra-cflags="-DIFERRET_SYSCALL -DIFERRET_INFO_FLOW" 
# --extra-cflags="-DIFERRET_SYSCALL -DIFERRET_INFO_FLOW -DIFERRET_PUID" 

# -DIFERRET_SYSCALL   log system calls
# -DIFERRET_PUID      log when detect process change.  or spawn (in syscall stuff which also needs to be on for this to work.) 
# -DIFERRET_PUID_EVERY_TB   log when detect process
# -DIFERRET_SET_CPL : log changes of priv level

 
