#!/usr/bin/perl -w 


use strict;

unless (exists $ENV{'IFERRET_DIR'}) {
    die "IFERRET_DIR env variable not available.\n";
}

# Windows system call parameter types
my %win_types = (
    "ACCESS_MASK" => 1, "ATOM_INFORMATION_CLASS" => 1, "BOOLEAN" => 1, "BOOT_ENTRY" => 1,
    "CLIENT_ID" => 1, "CONTEXT" => 1, "DBGUI_WAIT_STATE_CHANGE" => 1, "DEBUGOBJECTINFOCLASS" => 1,
    "EVENT_INFORMATION_CLASS" => 1, "EVENT_TRACE_HEADER" => 1, "EVENT_TYPE" => 1,
    "EXCEPTION_RECORD" => 1, "FILE_BASIC_INFORMATION" => 1, "FILE_INFORMATION_CLASS" => 1,
    "FILE_NETWORK_OPEN_INFORMATION" => 1, "FILE_PATH" => 1, "FILE_SEGMENT_ELEMENT" => 1,
    "FS_INFORMATION_CLASS" => 1, "GENERIC_MAPPING" => 1, "HANDLE" => 1, "INITIAL_TEB" => 1,
    "IO_APC_ROUTINE" => 1, "IO_COMPLETION_INFORMATION_CLASS" => 1, "IO_STATUS_BLOCK" => 1,
    "JOBOBJECTINFOCLASS" => 1, "JOB_SET_ARRAY" => 1, "KAFFINITY" => 1, "KEY_INFORMATION_CLASS" => 1,
    "KEY_SET_INFORMATION_CLASS" => 1, "KEY_VALUE_ENTRY" => 1,
    "KEY_VALUE_INFORMATION_CLASS" => 1, "KNORMAL_ROUTINE" => 1, "KPROFILE_SOURCE" => 1,
    "LANGID" => 1, "LARGE_INTEGER" => 1, "LCID" => 1, "LDT_ENTRY" => 1, "LONG" => 1, "LPGUID" => 1, "LUID" => 1,
    "MEMORY_INFORMATION_CLASS" => 1, "MUTANT_INFORMATION_CLASS" => 1, "NTSTATUS" => 1,
    "OBJECT_ATTRIBUTES" => 1, "OBJECT_INFORMATION_CLASS" => 1, "OBJECT_TYPE_LIST" => 1,
    "PLUGPLAY_EVENT_BLOCK" => 1, "PLUGPLAY_CONTROL_CLASS" => 1, "PORT_INFORMATION_CLASS" => 1, "PORT_MESSAGE" => 1,
    "PORT_VIEW" => 1, "POWER_ACTION" => 1, "POWER_INFORMATION_LEVEL" => 1, "PRIVILEGE_SET" => 1,
    "PVOID" => 1, "REMOTE_PORT_VIEW" => 1, "RTL_ATOM" => 1, "SECTION_INFORMATION_CLASS" => 1,
    "SECTION_INHERIT" => 1, "SECURITY_DESCRIPTOR" => 1, "SECURITY_INFORMATION" => 1,
    "SECURITY_QUALITY_OF_SERVICE" => 1, "SEMAPHORE_INFORMATION_CLASS" => 1,
    "SHUTDOWN_ACTION" => 1, "SID" => 1, "SIZE_T" => 1, "SYSDBG_COMMAND" => 1,
    "SYSTEM_INFORMATION_CLASS" => 1, "SYSTEM_POWER_STATE" => 1, "THREADINFOCLASS" => 1,
    "TIMER_APC_ROUTINE" => 1, "TIMER_INFORMATION_CLASS" => 1, "TIMER_TYPE" => 1,
    "TOKEN_DEFAULT_DACL" => 1, "TOKEN_GROUPS" => 1, "TOKEN_INFORMATION_CLASS" => 1,
    "TOKEN_OWNER" => 1, "TOKEN_PRIMARY_GROUP" => 1, "TOKEN_PRIVILEGES" => 1, "TOKEN_SOURCE" => 1,
    "TOKEN_TYPE" => 1, "TOKEN_USER" => 1, "UCHAR" => 1, "ULARGE_INTEGER" => 1, "ULONG" => 1, "ULONG_PTR" => 1,
    "UNICODE_STRING" => 1, "USHORT" => 1, "WAIT_TYPE" => 1, "WSTR" => 1, "PROCESSINFOCLASS" => 1
);



# Windows types that are just 4-byte ints
my %win_int_types = (
    "ACCESS_MASK" => 1, "ATOM_INFORMATION_CLASS" => 1, "BOOLEAN" => 1, "DEBUGOBJECTINFOCLASS" => 1,
    "EVENT_INFORMATION_CLASS" => 1, "EVENT_TYPE" => 1, "FILE_INFORMATION_CLASS" => 1,
    "FILE_SEGMENT_ELEMENT" => 1, "FS_INFORMATION_CLASS" => 1, "HANDLE" => 1,
    "IO_COMPLETION_INFORMATION_CLASS" => 1, "JOBOBJECTINFOCLASS" => 1, "KAFFINITY" => 1,
    "KEY_INFORMATION_CLASS" => 1, "KEY_SET_INFORMATION_CLASS" => 1,
    "KEY_VALUE_INFORMATION_CLASS" => 1, "KPROFILE_SOURCE" => 1, "LANGID" => 1, "LCID" => 1,
    "LDT_ENTRY" => 1, "LONG" => 1, "LPGUID" => 1, "MEMORY_INFORMATION_CLASS" => 1,
    "MUTANT_INFORMATION_CLASS" => 1, "NTSTATUS" => 1, "OBJECT_INFORMATION_CLASS" => 1,
    "PLUGPLAY_CONTROL_CLASS" => 1, "PORT_INFORMATION_CLASS" => 1, "POWER_ACTION" => 1,
    "POWER_INFORMATION_LEVEL" => 1, "PROCESSINFOCLASS" => 1, "RTL_ATOM" => 1,
    "SECTION_INFORMATION_CLASS" => 1, "SECTION_INHERIT" => 1, "SECURITY_INFORMATION" => 1,
    "SEMAPHORE_INFORMATION_CLASS" => 1, "SHUTDOWN_ACTION" => 1, "SIZE_T" => 1,
    "SYSDBG_COMMAND" => 1, "SYSTEM_INFORMATION_CLASS" => 1, "SYSTEM_POWER_STATE" => 1,
    "THREADINFOCLASS" => 1, "TIMER_INFORMATION_CLASS" => 1, "TIMER_TYPE" => 1,
    "TOKEN_INFORMATION_CLASS" => 1, "TOKEN_TYPE" => 1, "ULONG" => 1, "ULONG_PTR" => 1, "USHORT" => 1,
    "WAIT_TYPE" => 1, "PVOID" => 1
);

# Windows types that are 8-bytes of data 
my % win_int64_types = ( "IO_STATUS_BLOCK" => 1 );


# Handlers we have defined for specific types
# Maps between the type name and the pair (function, format_specifier)
my %win_type_handlers = (
    "UNICODE_STRING" => ["iferret_get_unicode_string", "s"],
    "OBJECT_ATTRIBUTES" => ["iferret_get_object_attributes", "s"],
    "LARGE_INTEGER" => ["iferret_get_large_integer", "8"],
    "LUID" => ["iferret_get_large_integer", "8"],
    "ULARGE_INTEGER" => ["iferret_get_large_integer", "8"],
    "UCHAR" => ["iferret_get_uchar", "1"],
);

# set of types used to represent a string as a syscall arg.
my %syscallStringTypes = ('char *' => 1,
                          'char __user *' => 1,
                          'const char *' => 1,
                          'const char __user *' => 1,
                          'unsigned char __user *' => 1);

# arg 1 of a syscall is EBX, arg 2 is ECX, etc.  
# after 6, it gets complicated.
my @syscallRegArgs = ("EBX", "ECX", "EDX", "ESI", "EDI", "EBP");

my @v = ("a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9", "a10", "a11", "a12", "a13", "a14", "a15", "a16", "a17", "a18", "a19", "a20", "a21", "a22", "a23", "a24", "a25", "a26", "a27", "a28", "a29");

my %iferret_fmts;

#mz this will get called by File::Find()
use File::Find();

my %ops;

# $_ filename
# $File::Find::name is complete path
sub handle_file {
    #mz we only care about .c and .h files
    if (/^.*\.[ch]\z/s) {
        my $filename = $_;
        if ($filename =~    /iferret_log_arg_fmt.h/
            || $filename =~ /iferret_ops.h/
            || $filename =~ /iferret_log.c/
            || $filename =~ /iferret.c/
            || $filename =~ /iferret_op_str.c/
            || $filename =~ /iferret_syscall_switch/
            || $filename =~ /iferret_socketcall.c/
            || $filename =~ /iferret_log_simp.h/
            ) {
            print "Skipping $filename.\n";
            return;
        }

#        if ($filename =~ /softmmu_header.h/) {
#            print "foo\n";
#        }
        my @lines;
        open F, "$File::Find::name";
        while (<F>) {
            if (/iferret_log_info_flow_op_write/
                || /iferret_log_syscall_op_write/
                || /iferret_log_sysret_op_write/
                || /iferret_log_op_write/) {
                push @lines, $_;
            }
        }
        close F;

        #@lines = `cd $iferretDir; grep \"iferret_log_info_flow_op_write\" $filename`;
        #push @lines, `cd $iferretDir; grep \"iferret_log_syscall_op_write\" $filename`; 
        #push @lines, `cd $iferretDir; grep \"iferret_log_sysret_op_write\" $filename`; 
        #push @lines, `cd $iferretDir; grep \"iferret_log_op_write\" $filename`; 

        if (scalar @lines > 0) {
            print "examining file $filename\n";
        }

#       my $filename = "none";
        foreach my $line (@lines) {
            
#            if ($line =~ /IFLO_HD_TRANSFER_PART2/) {
#                print "foo\n";
#            }

            chomp $line;
            # skip commented lines (c++ style)
            if ($line =~ /^(\s*)\/\//) {
                next;
            }

#           if ($line =~ /IFLO_HD_TRANSFER/) {
#               print "foo\n";
#           }
            if ($line =~ /iferret_log_info_flow_op_write/
                || $line =~ /iferret_log_syscall_op_write/
                || $line =~ /iferret_log_sysret_op_write/
                || $line =~ /iferret_log_op_write/
                ) {     
#               if ($skip == 1) {
#                   next;
#               }
                if ($line =~ /iferret_log_info_flow_op_write_([01248s]+)\((.*)\)\;/
                    || $line =~ /iferret_log_syscall_op_write_([01248s]+)\((.*)\)\;/ 
                    || $line =~ /iferret_log_sysret_op_write_([01248s]+)\((.*)\)\;/ 
                    || $line =~ /iferret_log_op_write_([01248s]+)\((.*)\)\;/
                    ) {
                    # format is specified by the suffix of the write function name. 
                    my $fmt = $1;
                    $iferret_fmts{$fmt} = 1;
                    my $inside = $2;
                    if ($inside =~ /^glue\((.*),CSUFFIX\),(.*)$/) {
                        # yuck.  hate that glue shit.  
                        # produce all permutations of this. CSUFFIX={Q,L,W,B} 
                        my $name_prefix = $1;
                        my $args = $2;
                        &add_op(\%ops, "$name_prefix"."Q", $args, $filename, $line, $fmt);
                        &add_op(\%ops, "$name_prefix"."L", $args, $filename, $line, $fmt);
                        &add_op(\%ops, "$name_prefix"."W", $args, $filename, $line, $fmt);
                        &add_op(\%ops, "$name_prefix"."B", $args, $filename, $line, $fmt);
                    }
                    else {
                        my ($opname, @rest) = split ',', $inside;
                        my $args = join ',', @rest;
                        print "$opname, $args\n";
                        &add_op(\%ops, $opname, $args, $filename, $line, $fmt);
                    }
                }
                else {
                    unless ($line =~ /\)(\s*)\{/ 
                            || $line =~ /iferret_log_op_write_prologue/) {
                        die "can't parse $line\n";
                    }
                }
            }
#           else {
#           $filename = $line;
#           if ($filename eq "/home/tleek/hg/iferret-light/iferret-logging-new/target-i386/iferret_log_arg_fmt.h"
#               || $filename eq "/home/tleek/hg/iferret-light/iferret-logging-new/target-i386/iferret_ops.h"
#               || $filename eq "/home/tleek/hg/iferret-light/iferret-logging-new/target-i386/iferret_op_str.c"
#               
#               ) {
#               $skip = 1;
#           }
#           else {
#               $skip = 0;
#           }
        }
    }
}

# main
{    
    # read in all the syscall prototypes,
    # parse lightly.
    print "parsing syscall prototypes.\n";
    open F, "./syscall-prototypes";
    my $maxSyscallNum = 0;
    my @syscall;
    while (my $line = <F>) {
        # a single line, i..e a single prototype
        chomp $line;
        print "line = $line\n";
        my @foo = split ' ', $line;
        my ($call_num, $first, @rest) = @foo;
        if ($call_num > $maxSyscallNum) {
            $maxSyscallNum = $call_num;
        }
        my $rest = join ' ', @rest;
        my $name;
        $syscall[$call_num] = {};
        $syscall[$call_num]{proto} = $line;
        if ($first eq "missing") {
            # we have no prototype for this syscall
            my $name = shift @rest;
            $syscall[$call_num]{name} = $name;
            $syscall[$call_num]{noargs} = 1;
            $syscall[$call_num]{format} = "0";
            print "syscall $call_num is missing\n";
            next;
        }
        if ($rest =~ /([^\(\) ]+)\s*\((.+)\);/) {
            # we have a prototype 
            my $nameandtype = $1;
            my $inside = $2;
            # if return type is pointer, get rid of that crap
            $nameandtype =~ s/\*/ /g;
            my @foo = split ' ', $nameandtype;
            my $name = pop @foo;
            print "we have a prototype for $name. args=[$inside]\n";
            $syscall[$call_num]{name} = $name;
            # examine the protoype args
            if ($inside eq "void") {
                # no args
                $syscall[$call_num]{noargs} = 1;
                print "syscall $call_num has no args\n";
                $syscall[$call_num]{format} = "0";
            }
            else {
                # one or more args
                $syscall[$call_num]{args} = ();
                $syscall[$call_num]{argnames} = ();
                $inside =~ s/\*/\* /g;
                my @args = split ',', $inside;
                my $n = scalar @args;
                if ($n > 6) {
                    $n = 6;
                }
                my $format = "";
                if ($n == 0) {
                    $format = "0";
                }
                for (my $i=0; $i<$n; $i++) {
                    my $arg = $args[$i];
                    print "arg = [$arg]\n";
                    $arg =~ s/\*/* /g;
                    my @argparts = split ' ', $arg;         
                    # NB: assume prototypes have named args
                    my $varname = pop @argparts;
                    my $argType = join ' ', @argparts;
                    print "--> var=[$varname] type=[$argType] ";
                    if (($argType =~ /char.*\*/) 
                        && !($varname eq "buf")
                        && !($varname eq "...")
                        && $varname !~ /\[\]$/
                        ) {
#                   if (exists $syscallStringTypes{$argType}) {
                        # it's a string
                        push @{$syscall[$call_num]{args}}, "s";
                        $format .= "s";
                        print "$arg is a string\n";
                    }
                    elsif ($argType =~ /\*/) {
                        # some kind of pointer. 
                        push @{$syscall[$call_num]{args}}, "p";
                        $format .= "p";
                        print "$arg is a pointer of some kind\n";       
                    }           
                    else {
                        push @{$syscall[$call_num]{args}}, "4";
                        $format .= "4";
                        print "$arg is a 4-byte int\n";
                    }
                    # Keep track of the variable names for later
                    push @{$syscall[$call_num]{argnames}}, $varname;
                }
                print "format is $format\n";
                $syscall[$call_num]{format} = $format;
            }
            $iferret_fmts{$syscall[$call_num]{format}} = 1;
        }
        else {
            print "$line\n";
            die "Can't understand this one?\n";
        }
    }
    my $numSyscalls = scalar @syscall;
    print "$numSyscalls unique syscalls\n";

# WINDOWS SYSTEM CALLS
    print "parsing windows syscall prototypes.\n";
    open F, "./syscall-prototypes-xpsp2";
    my $maxSyscallNum_win = 0;
    my @syscall_win;
    while (my $line = <F>) {
        # a single line, i..e a single prototype
        chomp $line;
        print "line = $line\n";
        my @foo = split ' ', $line;
        my ($call_num, $first, @rest) = @foo;
        if ($call_num > $maxSyscallNum_win) {
            $maxSyscallNum_win = $call_num;
        }
        my $rest = join ' ', @rest;
        my $name;
        $syscall_win[$call_num] = {};
        $syscall_win[$call_num]{proto} = $line;
        if ($first eq "missing") {
            # we have no prototype for this syscall
            my $name = shift @rest;
            $syscall_win[$call_num]{name} = $name;
            $syscall_win[$call_num]{noargs} = 1;
            $syscall_win[$call_num]{format} = "0";
            print "syscall $call_num is missing\n";
            next;
        }
        if ($rest =~ /([^\(\) ]+)\s*\((.+)\);/) {
            # we have a prototype 
            my $nameandtype = $1;
            my $inside = $2;
            # if return type is pointer, get rid of that crap
            $nameandtype =~ s/\*/ /g;
            my @foo = split ' ', $nameandtype;
            my $name = pop @foo;
            print "we have a prototype for $name. args=[$inside]\n";
            $syscall_win[$call_num]{name} = $name;
            # examine the protoype args
            if ($inside =~ /^void$/i) {
                # no args
                $syscall_win[$call_num]{noargs} = 1;
                print "syscall $call_num has no args\n";
                $syscall_win[$call_num]{format} = "0";
            }
            else {
                # one or more args
                $syscall_win[$call_num]{args} = ();
                $syscall_win[$call_num]{argnames} = ();
                $inside =~ s/\*/\* /g;
                my @args = split ',', $inside;
                my $n = scalar @args;
                my $format = "";
                if ($n == 0) {
                    $format = "0";
                }
                for (my $i=0; $i<$n; $i++) {
                    my $arg = $args[$i];
                    print "arg = [$arg]\n";
                    $arg =~ s/\*/* /g;
                    my @argparts = split ' ', $arg;         
                    # NB: assume prototypes have named args
                    my $varname = pop @argparts;
                    my $argType = join ' ', @argparts;
                    print "--> var=[$varname] type=[$argType] \n";
                    # Unlike Linux, we have to defer the conversion to a format
                    # until later, so just push the arg itself on now.
                    my $fmt = get_win_arg_fmt($argType);
                    push @{$syscall_win[$call_num]{args}}, $argType;
                    push @{$syscall_win[$call_num]{fmt_list}}, $fmt;
                    $format .= $fmt;
                    push @{$syscall_win[$call_num]{argnames}}, $varname;
                }
                print "format is $format\n";
                $syscall_win[$call_num]{format} = $format;
            }
            $iferret_fmts{$syscall_win[$call_num]{format}} = 1;
        }
        else {
            print "$line\n";
            die "Can't understand this one?\n";
        }
    }
    my $numSyscalls_win = scalar @syscall_win;
    print "$numSyscalls_win unique syscalls\n";


    my $iferretDir = $ENV{'IFERRET_DIR'};


    print "iferret dir is $iferretDir\n";
    
# examine all the source files looking for iferret_log_info_flow_op_write calls
    
    #mz TODO write this code in PERL!
    #my @files = `cd $iferretDir; find . -name \"*.[ch]\"`;
    File::Find::find({wanted => \&handle_file, follow => 1 }, $iferretDir);

    my $numOps = scalar keys %ops;
    print "$numOps unique info flow ops\n";
    

# examine iferret_socketcall.c to find out how to parse socket syscalls. 
    open F, "$iferretDir/target-i386/iferret_socketcall.c";
    my ($ebx,$name);
    my @socketcalls;
    while (my $line = <F>) {
#    if ($line =~ /case ([0-9]+): \/\/ ([a-z]+)\s*$/) {
#       $ebx = $1;
#       $name = $2;
#    }
        if ($line =~ /^\s+\/\//) {
            next;
        }
        if ($line =~ /iferret_log_socketcall_op_write_/) {
            if ($line =~ /iferret_log_socketcall_op_write_([01248s]+)\s*\(scp,([^,]+),(.*)\)\;/) {
                my $fmt = $1;
                $iferret_fmts{$fmt} = 1;
                my $name = $2;
                my $args = $3;
                &add_socketcall(\@socketcalls, $name, $args, $line, $fmt);          
            }
            else {
                print "$line\n";
                die "Cant parse?\n";
            }
        }
    }
    my $numSocketcalls = scalar @socketcalls;
    print "$numSocketcalls unique socket calls\n";





# create the big case stmt for writing syscall log items
    open SW, ">iferret_syscall_switch_linux.c";
    print SW <<EOF;
// NB: This code is auto-generated by make_iferret_code.pl. 
// It's job is to generate iferret log entries for system calls.
// It should be included in iferret_syscalls.c
#include "exec.h"
#include "iferret_syscall.h"

int copy_string(char *str, uint32_t vaddr);

void iferret_write_syscall_params_linux(iferret_syscall_t *scp) {
char str1[MAX_STRING_LEN], str2[MAX_STRING_LEN], str3[MAX_STRING_LEN];

EOF

    print SW "switch (scp->eax) {\n";
    for (my $i=0; $i<=$maxSyscallNum; $i++) {
        if (exists $syscall[$i]) {

            if ($i == 102) {
                # don't bother with socketcall
                next;
            }

#            if ($i == 4) {
#                print "foobar\n";
#            }

            print "case $i: // $i $syscall[$i]{proto}\n";
            print SW "case $i: // $i $syscall[$i]{proto}\n";
            my $numStrings = 1;
            unless (exists $syscall[$i]{noargs}) {
                my $n = scalar @{$syscall[$i]{args}};
                for (my $j=0; $j<$n; $j++) {
                    my $t = $syscall[$i]{args}[$j];
                    if ($t eq "s") {
                        print "if (copy_string(str$numStrings, $syscallRegArgs[$j])) { \n";
                        print SW "if (copy_string(str$numStrings, $syscallRegArgs[$j])) { \n";
                        $numStrings ++;
                    }
                }
            }
            print "iferret_log_syscall_op_write_$syscall[$i]{format}(scp";
            print SW "iferret_log_syscall_op_write_$syscall[$i]{format}(scp";
            unless (exists $syscall[$i]{noargs}) {
                my $n = scalar @{$syscall[$i]{args}};
                my $numStrings = 1;
                for (my $j=0; $j<$n; $j++) {
                    my $t = $syscall[$i]{args}[$j];
                    if ($t eq "s") {
                        print ",str$numStrings";
                        print SW ",str$numStrings";
                        $numStrings ++;
                    }
                    elsif ($t eq "p") {
                        print ",ram_addr($syscallRegArgs[$j])";
                        print SW ",ram_addr($syscallRegArgs[$j])";
                    }
                    else {
                        print ",$syscallRegArgs[$j]";
                        print SW ",$syscallRegArgs[$j]";
                    }
                }
            }
            print ");\n";
            print SW ");\n";
            for (my $j=0; $j<$numStrings-1; $j++) {
                print "}\n";
                print SW "}\n";
            }
            print "break; \n";
            print SW "break; \n";
        }
    }
    print SW "}\n}\n";
    close SW;

# Switch statement for syscall enter in Windows
    open SW, ">iferret_syscall_switch_xpsp2.c";
    print SW <<EOF;
// NB: This code is auto-generated by make_iferret_code.pl. 
// Its job is to generate iferret log entries for system calls.
// It should be included in iferret_syscalls.c
#include "exec.h"
#include "iferret_syscall.h"

target_ulong iferret_get_arg_win(int i, int is_enter);


EOF

    print SW "void iferret_write_syscall_params_xpsp2(iferret_syscall_t *scp) {\n";
    print "void iferret_write_syscall_params_xpsp2(iferret_syscall_t *scp) {\n";
    print SW "switch (scp->eax) {\n";
    print "switch (scp->eax) {\n";
    for (my $i=0; $i<=$maxSyscallNum_win; $i++) {

            if ($i == 183) {
               print "foo\n";
            }

        if (exists $syscall_win[$i]) {
            print SW "case $i: // $i $syscall_win[$i]{proto}\n";
            print "case $i: // $i $syscall_win[$i]{proto}\n";
            
            # Get a new scope
            print SW "{\n";
            print "{\n";
            if (exists $syscall_win[$i]{noargs}) {
                print SW "iferret_log_syscall_op_write_$syscall_win[$i]{format}(scp);\n";
                print "iferret_log_syscall_op_write_$syscall_win[$i]{format}(scp);\n";
            }
            else {
                # Temporary holder for argument, may be needed
                # in pointer derefs
                print SW "uint32_t arg_temp;\n";
                print "uint32_t arg_temp;\n";
                # Define slots for the argument values
                my $n = scalar @{$syscall_win[$i]{args}};
                for (my $j = 0; $j < $n; $j++) {
                    my $fmt = $syscall_win[$i]{fmt_list}[$j];
                    if ($fmt eq "4") {
                        print SW "uint32_t arg$j;\n";
                        print "uint32_t arg$j;\n";
                    }
                    elsif ($fmt eq "1") {
                        print SW "uint8_t arg$j;\n";
                        print "uint8_t arg$j;\n";
                    }
                    elsif ($fmt eq "8") {
                        print SW "uint64_t arg$j;\n";
                        print "uint64_t arg$j;\n";
                    }
                    elsif ($fmt eq "s") {
                        print SW "char * arg$j;\n";
                        print "char * arg$j;\n";
                    }
                }
                print SW "\n";
                print "\n";
                
                # Write the extraction code
                for (my $j = 0; $j < $n; $j++) {
                    my $fmt = $syscall_win[$i]{fmt_list}[$j];
                    my $varName = "&arg$j";
                    print SW "arg_temp = iferret_get_arg_win($j,scp->is_enter);\n";
                    print "arg_temp = iferret_get_arg_win($j,scp->is_enter);\n";
                    my ($stmt, $pointer_depth) = build_win_arg_extractor($syscall_win[$i]{args}[$j], $varName, 0);
                    for(my $k = 0; $k < $pointer_depth; $k++) {
                        print SW "cpu_virtual_memory_read(env, arg_temp, (char *) &arg_temp, 4);\n";
                        print "cpu_virtual_memory_read(env, arg_temp, (char *) &arg_temp, 4);\n";
                    }
                    print SW $stmt;
                    print $stmt;
                }

                # Write the call to the logger
                print "iferret_log_syscall_op_write_$syscall_win[$i]{format}(scp";
                print SW "iferret_log_syscall_op_write_$syscall_win[$i]{format}(scp";
                for (my $j = 0; $j < $n; $j++) {
                    print SW ",arg$j";
                    print ",arg$j";
                }
                print SW ");\n";
                print ");\n";

                # Extractors that return strings allocate the strings themselves,
                # so we free them here
                for (my $j = 0; $j < $n; $j++) {
                    my $fmt = $syscall_win[$i]{fmt_list}[$j];
                    if ($fmt eq "s") {
                        print SW "my_free(arg$j, strlen(arg$j)+1, poolid_syscall);\n";
                        print "my_free(arg$j, strlen(arg$j)+1, poolid_syscall);\n";
                    }
                }
            }
            # Close out of the scope
            print SW "}\n";
            print "}\n";
            print "break; \n";
            print SW "break; \n";
        }
    }
    print SW "}\n";
    print "}\n";
    print SW "}\n";
    print "}\n";
    close SW;

#
    my @enum;

    my $ii=0;
    print "Accumulating enum type for info flow ops.\n";
# first, all the ops
    foreach my $opname (sort keys %ops) {
        print "enum $ii op $opname\n";
        $enum[$ii] = {};
        $enum[$ii]{opname} = $opname;
        my $comment = "";
        foreach my $filename (sort keys %{$ops{$opname}{files}}) {
            $comment .= "// $filename\n";
            foreach my $line (sort @{$ops{$opname}{files}{$filename}}) {
		$line =~ s/\\//g;
                $comment .= "// --> $line\n";
            }
        }
        $enum[$ii]{comment} = $comment;
        $enum[$ii]{format} = $ops{$opname}{format};
        $enum[$ii]{args} = ();
        $ii++;
    }
    
    $enum[$ii]{opname} = "IFLO_SYS_CALLS_START";
    $enum[$ii]{comment} = "//This is just a separator\n";
    $enum[$ii]{format} = "0";
    $enum[$ii]{args} = ();
    $ii++;

# second, all the syscalls (except socketcall)
    for (my $i=0; $i<=$maxSyscallNum; $i++) {
        if (! (exists $syscall[$i])) {
            $enum[$ii]{opname} = "IFLO_UNUSED_$ii";
            $enum[$ii]{format} = "0";
            $enum[$ii]{comment} = "";
            $ii++;
            next;
        }
        my $name = $syscall[$i]{name};
        my $opname = $name;
        $opname =~ tr/a-z/A-Z/;
        $opname = "IFLO_SYS_" . $opname;
        print "enum $ii syscall $opname\n";
        $enum[$ii]{opname} = $opname;
#       my $comment = "// $ii \n";
        my $comment .= "// syscall # $i";
        $comment .= "// $syscall[$i]{proto}\n";
        $enum[$ii]{comment} = $comment;
        $enum[$ii]{args} = $syscall[$i]{argnames};
#       my $format = "";
#       if (exists $syscall[$i]{args}) {
#           if ((scalar @{$syscall[$i]{args}}) == 0) {
#               $format = "0";
#           }
#           else {
#               foreach my $arg (@{$syscall[$i]{args}}) {
#                   if ($arg eq "4" || $arg eq "s") {
#                       $format .= $arg;
#                   }
#               }
#           }
#       } 
#       else {
#           $format = "0";
#       }
        $enum[$ii]{format} = $syscall[$i]{format};
        $ii++;
    }

    $enum[$ii]{opname} = "IFLO_SYS_SOCKETCALLS_START";
    $enum[$ii]{comment} = "//This is just a separator\n";
    $enum[$ii]{format} = "0";
    $enum[$ii]{args} = ();
    $ii++;

# third, all the socketcalls 
    foreach my $rh (@socketcalls) {
        $enum[$ii]{opname} = $rh->{name};
        print "enum=$ii socketcall $rh->{name}\n";
        $enum[$ii]{comment} = $rh->{comment};
        $enum[$ii]{format} = $rh->{format};
        $enum[$ii]{args} = ();
        $ii++;
    }

    $enum[$ii]{opname} = "IFLO_SYS_WINCALLS_START";
    $enum[$ii]{comment} = "//This is just a separator\n";
    $enum[$ii]{format} = "0";
    $enum[$ii]{args} = ();
    $ii++;

my %win_call_names = ();
# fourth, Windows system calls
    for (my $i=0; $i<=$maxSyscallNum_win; $i++) {
        if (! (exists $syscall_win[$i])) {
            $enum[$ii]{opname} = "IFLO_UNUSED_$ii";
            $enum[$ii]{format} = "0";
            $enum[$ii]{comment} = "";
            $enum[$ii]{args} = ();
            $ii++;
            next;
        }
        # Note: some windows functions have multiple ordinals
        my $name = $syscall_win[$i]{name};
        if (exists $win_call_names{$name}) {
            $win_call_names{$name}++;
            $name .= $win_call_names{$name};
        }
        else {
            $win_call_names{$name} = 1;
        }

        my $opname = $name;
        $opname =~ tr/a-z/A-Z/;
        $opname = "IFLO_SYS_" . $opname;
        print "enum $ii syscall $opname\n";
        $enum[$ii]{opname} = $opname;
        my $comment .= "// Windows syscall # $i";
        $comment .= "// $syscall_win[$i]{proto}\n";
        $enum[$ii]{comment} = $comment;
        $enum[$ii]{format} = $syscall_win[$i]{format};
        $enum[$ii]{args} = $syscall_win[$i]{argnames};
        $ii++;
    }

# a dummy end op

    $enum[$ii]{opname} = "IFLO_DUMMY_LAST";
    $enum[$ii]{comment} = "";
    $enum[$ii]{format} = "0";
    $enum[$ii]{args} = ();



# okay, now we have an ordering for log ops.
# create the file with the enum type

# write the file containing the big enum type
    open OPS, ">iferret_ops.h";
    print OPS "// NB: This code is auto-generated by make_iferret_code.pl. \n";
    print OPS "// It contains the big enum type of all info-flow ops.\n";
    print OPS "\#ifndef __IFERRET_OPS_H_ \n";
    print OPS "\#define __IFERRET_OPS_H_ \n";
    print OPS "\ntypedef enum iferret_log_op_enum {\n";
    for (my $i=0; $i<scalar @enum; $i++) {
        my $opname = $enum[$i]{opname};
        print "$i $opname\n";
        print OPS "// $i \n";
        print OPS "$enum[$i]{comment}";
        print OPS "$opname";
        if ($i < (scalar @enum)-1) {
            print OPS ",";
        }
        print OPS "\n";
    }
    print OPS "} iferret_log_op_enum_t;\n";
    print OPS "\#endif\n";
    close OPS;

# create the big array of strings specifying log item formats
    open FMT, ">iferret_log_arg_fmt.h";
    print FMT "// NB: This code is auto-generated by make_iferret_code.pl. \n";
    print FMT "// It's job is to generate the big array of strings specifying the \n";
    print FMT "// format of iferret log entries for system calls.\n";
    print FMT "\#ifndef __IFERRET_LOG_ARG_FMT_H_ \n";
    print FMT "\#define __IFERRET_LOG_ARG_FMT_H_ \n";
    print FMT "\n";
    print FMT "typedef struct {\n";
    print FMT "  char *fmt;\n";
    print FMT "  char *args[IFERRET_OP_MAX_NUM_ARGS];\n";
    print FMT "} iferret_arg_fmt_t;\n";
    print FMT "\n";
    print FMT "iferret_arg_fmt_t iferret_log_arg_format[] = {\n";

    for (my $i=0; $i<scalar @enum; $i++) {
        my $opname = $enum[$i]{opname};
        print "$opname\n";
        if (! (exists $enum[$i]{format})) {
            print "no format for $i $opname?\n";
            die;
        }
        print FMT "  // $i \n";
        print FMT "$enum[$i]{comment}";
        #if (! defined @{$enum[$i]{args}}) {
        #    print "DEBUG: args array for $opname [$i] not defined!\n";
        #    die;
        #}
        my @quoted_args = ();
        
        foreach my $arg (@{$enum[$i]{args}}) {
            push @quoted_args, "\"$arg\"";
        }
        print FMT " {\"$enum[$i]{format}\", {" .  (join ", ", @quoted_args) . "}}";
        if ($i < (scalar @enum)-1) {
            print FMT ",";
        }
        print FMT "\n";
    }
    print FMT "};\n";
    print FMT "\#endif\n";
    close FMT;



    

# Also create fns to returns strings for each op
    open STR, ">iferret_op_str.c";
    print STR "// NB: This code is auto-generated by make_iferret_code.pl. \n";
    print STR "// It contains a large number of functions that take an info-flow\n";
    print STR "// op as input and return a pointer to a string representing it. \n";
    print STR "#include <stdio.h>\n";
    print STR "#include \"iferret_ops.h\"\n";
    print STR "\n";   
    print STR "char *iferret_op_num_to_str(iferret_log_op_enum_t op_num) {\n";
    print STR "  char *rv=NULL;\n";
    print STR "  switch (op_num) { \n";
    for (my $i=0; $i<scalar @enum; $i++) {
        my $opname = $enum[$i]{opname}; 
        print STR "  case $opname:\n";
        print STR "    rv = \"$opname\";\n";
        print STR "    break;\n";
    }
    print STR "  }\n";
    print STR "  return rv;\n";
    print STR "}\n";
    close STR;
    

# for each format we actually saw in code, create the various required functions

    my $fnsfh = do {local *FNS} ;
    open $fnsfh, ">iferret_log_simp.h";
    print $fnsfh "// NB: This code is auto-generated by make_iferret_code.pl. \n";
    print $fnsfh "// It contains a large number of functions that write an op plus\n";
    print $fnsfh "// a standard argument list.\n";
    print $fnsfh "\#ifndef __IFERRET_LOG_SIMP_H_\n";
    print $fnsfh "\#define __IFERRET_LOG_SIMP_H_\n";
    print $fnsfh "\#define TRUE 1\n";
    print $fnsfh "extern uint8_t iferret_info_flow;\n";

    foreach my $fmt (sort keys %iferret_fmts) {
        
        # The base function
        print $fnsfh "static inline void iferret_log_op_write_$fmt(\n";
        print $fnsfh "  iferret_log_op_enum_t op_num";
        &write_formals($fnsfh, $fmt);
        print $fnsfh ")\n{\n";
        print $fnsfh "  iferret_log_op_write_prologue(op_num);\n";
        &write_log_calls($fnsfh, $fmt);
        print $fnsfh "}\n\n";

        # The info-flow version
        print $fnsfh "static inline void iferret_log_info_flow_op_write_$fmt(\n";
        print $fnsfh "  iferret_log_op_enum_t op_num";
        &write_formals($fnsfh, $fmt);
        print $fnsfh ")\n{\n";
        print $fnsfh "\#ifdef IFERRET_LOGTHING_ON\n";
        #print $fnsfh "  if (iferret_info_flow == TRUE) {\n";
        print $fnsfh "  iferret_log_op_write_prologue(op_num);\n";
        &write_log_calls($fnsfh, $fmt);
        #print $fnsfh "  }\n";
        print $fnsfh "\#endif\n";
        print $fnsfh "}\n\n";

        # The socketcall version
        print $fnsfh "static inline void iferret_log_socketcall_op_write_$fmt(\n";
        print $fnsfh "  iferret_syscall_t *sc,\n";
        print $fnsfh "  iferret_log_op_enum_t op_num";
        &write_formals($fnsfh, $fmt);
        print $fnsfh ")\n{\n";
        print $fnsfh "\#ifdef IFERRET_SYSCALL \n";
        print $fnsfh "  iferret_log_op_write_prologue(op_num);\n";
        #print $fnsfh "  iferret_log_syscall_commoner(sc);\n";
        &write_log_calls($fnsfh, $fmt);
        print $fnsfh "\#endif\n";
        print $fnsfh "}\n\n";

        # The syscall version
        print $fnsfh "static inline void iferret_log_syscall_op_write_$fmt(\n";
        print $fnsfh "  iferret_syscall_t *sc";
        if ($fmt eq "1") {
            print "foo\n";
        }
        if (!($fmt eq "0")) {
            &write_formals($fnsfh, $fmt);
        }
        print $fnsfh ")\n{\n";
        print $fnsfh "\#ifdef IFERRET_SYSCALL \n";
        print $fnsfh "  iferret_log_op_write_prologue(sc->op_num);\n";
        #print $fnsfh "  iferret_log_syscall_commoner(sc);\n";
        &write_log_calls($fnsfh, $fmt);
        print $fnsfh "\#endif\n";
        print $fnsfh "}\n\n";

        # The sysret version
        print $fnsfh "static inline void iferret_log_sysret_op_write_$fmt(\n";
        print $fnsfh "  iferret_log_op_enum_t op_num";
        &write_formals($fnsfh, $fmt);
        print $fnsfh ")\n{\n";
        print $fnsfh "\#ifdef IFERRET_SYSCALL \n";
        print $fnsfh "  iferret_log_op_write_prologue(op_num);\n";
        &write_log_calls($fnsfh, $fmt);
        print $fnsfh "\#endif\n";
        print $fnsfh "}\n\n";
        
    }
    print $fnsfh "\#endif\n";
    close $fnsfh;
} # main




sub write_formals() {
    my ($fnsfh, $fmt) = @_;
    
    my $l = length $fmt;
    for (my $i=0; $i<$l; $i++) {
        my $f = substr($fmt,$i,1);
        if ($f eq "0") { 
            last; 
        }
        print $fnsfh ",\n";
        if ($f eq "1") {
            print $fnsfh "  uint8_t $v[$i]"
        }
        if ($f eq "2") {
            print $fnsfh "  uint16_t $v[$i]"
        }
        if ($f eq "4") {
            print $fnsfh "  uint32_t $v[$i]"
        }
        if ($f eq "8") {
            print $fnsfh "  uint64_t $v[$i]"
        }
        if ($f eq "s") {
            print $fnsfh "  char * $v[$i]"
        }
        if ($f eq "p") {
            print $fnsfh "  uint32_t $v[$i]"
        }
    }
}


sub write_log_calls() {
    my ($fnsfh, $fmt) = @_;
    
    my $l = length $fmt;
    for (my $i=0; $i<$l; $i++) {
        my $f = substr($fmt,$i,1);
        if ($f eq "0") { 
            last; 
        }
        if ($f eq "p") {
            print $fnsfh "  iferret_log_write_4($v[$i]);\n";
        }
        else {
            print $fnsfh "  iferret_log_write_$f($v[$i]);\n";
        }
    }
}


sub write_args() {
    my ($fnsfh, $fmt) = @_;
    
    my $l = length $fmt;
    for (my $i=0; $i<$l; $i++) {
        my $f = substr($fmt,$i,1);
        print $fnsfh ", ";
        if ($f eq "0") { 
            last; 
        }
        print $fnsfh "$v[$i]";
    }
}














sub create_op_format() {
    my ($opname, $raArgs) = @_;
    my $fmt = "";
    print "$opname -- creating op format\n";
    foreach my $arg (@{$raArgs}) {
        $arg =~ s/^\s+//g;
        $arg =~ s/\s+$//g;
        print "arg is $arg\n";
        if ($arg =~ /^str[0-5]$/
            || $arg eq "label"
            || $arg eq "iferret_keyboard_label"
            || $arg eq "iferret_network_label"
            || $arg eq "tempbuf"
            ) {
            # in order for this script to recognize an op logfile entry
            # arg as a string, it must be called "str..."
            $fmt .= "s";
            print "--> string\n";
        }
        elsif ($arg eq "REGNUM"
               || $arg eq "MEMWRITE"
               || $arg eq "MEMSUFFIXNUM"
               || $arg eq "SHIFT"
               || $arg eq "cpl"
               || $arg =~ /tempbuf\[[0-9]+\]/
            ) {
            # a register number -- only one byte necessary
            $fmt .= "1"; 
            print "--> 1-byte int\n";
        }
        elsif ($arg eq "current_pid" 
               || $arg eq "current_uid"
               || $arg eq "EIP"
               || $arg eq "EAX"
               || $arg eq "EBX"
               || $arg eq "ECX"
               || $arg eq "EDX"
               || $arg eq "ESI"
               || $arg eq "EDI"
               || $arg eq "EBP"
               || $arg eq "ESP"
               || $arg eq "T0"
               || $arg eq "T1"
               || $arg =~ /^[0-9]+$/
               || $arg eq "val"
               || $arg eq "mmu_idx"
               || $arg eq "rv"
               || $arg eq "v"
               || $arg eq "family"
               || $arg eq "type"
               || $arg eq "protocol"
               || $arg =~ /^b[0-9]$/
               || $arg eq "sap->sin_family"
               || $arg eq "level"
               || $arg eq "option"
               || $arg eq "fd"
               || $arg eq "pktype"
               || $arg eq "halen"
               || $arg eq "domain"
               || $arg eq "len"
               || $arg eq "msg"
               || $arg =~ /\*\(int\*\)/
            ) {
            $fmt .= "4";
            print "--> 4-byte int\n";
        }
        elsif ($arg eq "A0"
               || $arg eq "physaddr"
               || $arg eq "ADDR"
               || $arg eq "ptr"
               || $arg eq "addr"
               || $arg eq "addr1"
               || $arg eq "RYANS_MAGIC_NUMBER_2"
               || $opname eq "HD_TRANSFER"
               || $arg =~ /IFRBA/
            ) {
            $fmt .= "8";
            print "--> 8-byte int\n";
        }
        else {
            print "Unrecognized arg: $arg. Using fmt of 8\n";
            $fmt .= "8";
        }
    }
    if ($fmt =~ /^$/) {
        $fmt = "0";
    }
    return $fmt;
}


sub merge_formats() {
    my ($format1, $format2) = @_;

    print "merging $format1 and $format2\n";
    if (length $format1 != length $format2) {
        print "different lengths?\n";
        die;
    }
    my $mergedfmt = "";
    for (my $i=0; $i<length $format1; $i++) {
        my $f = substr($format1,$i,1);
        my $newf = substr($format2,$i,1);
        if ($f eq $newf) {
            $mergedfmt .= $f;
        }
        elsif (($f =~ /^[0-9]$/)
               && ($newf =~ /^[0-9]$/)) {
            $f *= 1;
            $newf *= 1;
            if ($f > $newf) {
                $mergedfmt .= $f;
            }
            else {
                $mergedfmt .= $newf;
            }
        }
        else {
            print "dont know how to merge $f and $newf.\n";
            die;
        }
    }
    print "--> $mergedfmt\n";
    return ($mergedfmt);
}


sub update_op_format() {
    my ($opname, $fmt, $raArgs) = @_;

    my $newfmt = &create_op_format($opname, $raArgs);
    print "1 op is $opname\n";
    return &merge_formats($fmt, $newfmt);
}




sub add_op() {
    my ($rhOps, $opname, $args, $filename, $line, $fmt) = @_;

    my @rest = split ',', $args;
    # remember all the files we saw this in.
    unless (exists     $rhOps->{$opname}{files}{$filename}) {
        $rhOps->{$opname}{files}{$filename} = ();
    }
    # and foreach, collect all lines used to write this op
    push @{$rhOps->{$opname}{files}{$filename}}, $line;
    
    # create or update format string for this op by examining args.
    print "$opname $filename $line $fmt\n";
    if (exists $rhOps->{$opname}{format}) {
        my $fmt2 = $rhOps->{$opname}{format};
        if ($fmt ne $fmt2) {
            print "Disagreeing formats for $opname $fmt $fmt2\n";
            print "This: $fmt from $filename $line\n";
            print "others:\n";
            foreach my $filename2 (keys %{$rhOps->{$opname}{files}}) {
                print "  file: $filename2: \n";
                    foreach my $line2 (@{$rhOps->{$opname}{files}{$filename2}}) {
                        print "    line: $line2\n";
                }
            }
            die;
        }
        else {
            print "$fmt same as $fmt2\n";
        }
    }

    

#    if (!exists $rhOps->{$opname}{format}) {
#       $newFmt = &create_op_format($opname, \@rest);
#    }
#    else {
#       $newFmt = &update_op_format($opname, $rhOps->{$opname}{format}, \@rest);
#    }

    print "2 op is $opname. \n";
    print "$filename\n";
    print "$line\n";


    $rhOps->{$opname}{format} = $fmt;
}



sub add_socketcall() {
    my ($raSocketcalls, $opname, $args, $line, $fmt) = @_;

    my @rest = split ',', $args;
    # create or update format string for this op by examining args.
    my $i = scalar @{$raSocketcalls};
    my %h;
    $h{name} = $opname;
    $h{format} = $fmt;
    $h{comment} = "// $line";
    $raSocketcalls->[$i] = \%h;
#    $raSocketcalls->[$i]{name} = $opname;
#    $raSocketcalls->[$i]{format} = $fmt;
#    $raSocketcalls->[$i]{comment} = "// $line";
}

sub get_win_arg_fmt {
    my $argType = $_[0];

    $argType =~ s/^ *(IN|OUT) +//;
    $argType =~ s/^ *(IN|OUT) +//;
    print "get_win_arg_fmt: Base Type: [$argType]\n";
    if (exists $win_types{$argType}) {
        if (exists $win_int_types{$argType}) {
            return "4";
        }
        elsif (exists $win_int64_types{$argType}) {
            return "8";
        }
        elsif (exists $win_type_handlers{$argType}) {
            return $win_type_handlers{$argType}[1];
        }
        else {
            print "get_win_arg_fmt: No handling defined for $argType, assuming int\n";
            return "4";
        }
    }
    if ($argType =~ m/\*$/) {
        $argType =~ s/ *\*$//;
        return get_win_arg_fmt($argType);
    }
    elsif ($argType =~ m/^P/) {
        $argType =~ s/^P//;
        return get_win_arg_fmt($argType);
    }
    else {
        print "get_win_arg_fmt: ERROR: No matching basic type!\n";
        die;
    }
}

sub build_win_arg_extractor {
    my ($argType, $varName, $depth) = @_;

    $argType =~ s/^ *(IN|OUT) +//;
    $argType =~ s/^ *(IN|OUT) +//;
    print "build_win_arg_extractor: Base Type: [$argType]\n";
    if (exists $win_types{$argType}) {
        if (exists $win_int_types{$argType}) {
            return ("cpu_virtual_memory_read(env, arg_temp, (char *) $varName, 4);\n", $depth);
        }
        elsif (exists $win_int64_types{$argType}) {
            return ("cpu_virtual_memory_read(env, arg_temp, (char *) $varName, 8);\n", $depth);
        }
        elsif (exists $win_type_handlers{$argType}) {
            print "build_win_arg_extractor: using handler " . $win_type_handlers{$argType}[0] . "\n";
            return ($win_type_handlers{$argType}[0] . "(arg_temp, $varName);\n", $depth);
        }
        else {
            print "build_win_arg_extractor: No handling defined for $argType, assuming int\n";
            return ("cpu_virtual_memory_read(env, arg_temp, (char *) $varName, 4);\n", $depth);
        }
    }
    if ($argType =~ m/\*$/) {
        $argType =~ s/ *\*$//;
        return build_win_arg_extractor($argType, $varName, $depth + 1);
    }
    elsif ($argType =~ m/^P/) {
        $argType =~ s/^P//;
        return build_win_arg_extractor($argType, $varName, $depth + 1);
    }
    else {
        print "build_win_arg_extractor: ERROR: No matching basic type!\n";
        die;
    }
}
