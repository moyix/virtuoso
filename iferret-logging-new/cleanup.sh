make distclean

export IFERRET_DIR=`pwd`

cd target-i386
./make_iferret_code.pl

cd ..
./configure --disable-kqemu --prefix=$IFERRET_DIR/install --target-list=i386-softmmu  --disable-linux-user --disable-darwin-user
#-DIFERRET_INFO_FLOW" 



# --extra-cflags="-DIFERRET_SYSCALL -DIFERRET_PUID"

# --extra-cflags="-DIFERRET_SYSCALL -DIFERRET_INFO_FLOW -DIFERRET_PUID -DIFERRET_PHYS_EIP" 


# --extra-cflags="-DIFERRET_SYSCALL -DIFERRET_INFO_FLOW" 
# --extra-cflags="-DIFERRET_SYSCALL -DIFERRET_INFO_FLOW -DIFERRET_PUID" 

# -DIFERRET_SYSCALL   log system calls
# -DIFERRET_PUID      log when detect process change.  or spawn (in syscall stuff which also needs to be on for this to work.) 
# -DIFERRET_PUID_EVERY_TB   log when detect process
# -DIFERRET_SET_CPL : log changes of priv level

 
