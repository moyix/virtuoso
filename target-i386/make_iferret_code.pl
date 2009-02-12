#!/usr/bin/perl -w 

use strict;

# set of types used to represent a string as a syscall arg.
my %syscallStringTypes = ('char __user *',
			  'const char *',
			  'const char __user *',
			  'unsigned char __user *');

# arg 1 of a syscall is EBX, arg 2 is ECX, etc.  
# after 6, it gets complicated.
my @syscallRegArgs = ("EBX", "ECX", "EDX", "ESI", "EDI", "EBP");




# main
{

# read in all the syscall prototypes,
# parse lightly.
    print "parsing syscall prototypes.\n";
    open F, "./syscall-prototypes";
    my $maxSyscallNum = 0;
    my @syscall;
    my $num = 0; 
    while (my $line = <F>) {
	chomp $line;
	print "line = $line\n";
	my @foo = split ' ', $line;
	my $call_num = shift @foo;
	my $first = shift @foo;
	my $rest = join ' ', @foo;
	my $name;
	$syscall[$num] = {};
	$syscall[$num]{call_num} = $call_num;
	$syscall[$num]{proto} = $line;
	if (($call_num eq "102") && ($first =~ /^[0-9]+$/)) {
	    # this is a socketcall -- fmt is specified directly
	    $syscall[$num]{name} = shift @foo;
	    $syscall[$num]{fmt} = shift @foo;
	    print "syscall $num is a socketcall...\n";
	}
	elsif ($first eq "missing") {
	    # we have no prototype for this syscall
	    my $name = shift @foo;
	    $syscall[$num]{name} = $name;
	    $syscall[$num]{noargs} = 1;
	    print "syscall $num is missing\n";
	}
	elsif ($rest =~ /(.*)\((.*)\);/) {
	    # we have a prototype 
	    my $nameandtype = $1;
	    my @foo = split ' ', $nameandtype;
	    my $name = pop @foo;
	    my $inside = $2;
	    $syscall[$num]{name} = $name;
	    # examine the protoype args
	    if ($inside eq "void") {
		$syscall[$num]{noargs} = 1;
		print "syscall $num has no args\n";
	    }
	    else {
		$syscall[$num]{args} = ();
		$inside =~ s/\*/\* /g;
		my @args = split ',', $inside;
		my $n = scalar @args;
		if ($n > 6) {
		    $n = 6;
		}
		for (my $i=0; $i<$n; $i++) {
		    my $arg = $args[$i];
		    my @argparts = split ' ', $arg;	    
		    # NB: assume prototypes have named args
		    my $varname = pop @argparts;
		    my $argType = join ' ', @argparts;
		    if (exists $syscallStringTypes{$argType}) {
			# its a string
			push @{$syscall[$num]{args}}, "s";
		    }
		    else {
			push @{$syscall[$num]{args}}, "4";
		    }
		}
	    }
	}
	else {
	    print "$line\n";
	    die "Can't understand this one?\n";
	}
	$num ++;
	if ($num > $maxSyscallNum) {
	    $maxSyscallNum = $num;
	}    
    }
    my $numSyscalls = scalar @syscall;
    print "$numSyscalls unique syscalls\n";


    my $iferretDir = "/home/tleek/hg/iferret-logging-new";


    
# examine all the source files looking for info_flow_log_op_write calls
    my @lines = `cd $iferretDir; find -H /home/tleek/hg/iferret-logging-new -name \"*.[ch]\" -print -exec grep \"info_flow_log_op_write\" \'{}\' \\;`;
    
    my $skip = 0;
    my %ops;
    my $filename = "none";
    foreach my $line (@lines) {

	chomp $line;
	if ($line =~ /IFLO_HD_TRANSFER/) {
	    print "foo\n";
	}
	if ($line =~ /info_flow_log_op_write/) {	
	    if ($skip == 1) {
		next;
	    }
	    if ($line =~ /info_flow_log_op_write_([01248s]+)\((.*)\)\;/) {
		# format is specified by the suffix of the write function name. 
		my $fmt = $1;
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
			|| $line =~ /info_flow_log_op_write_prologue/) {
		    die "can't parse $line\n";
		}
	    }
	}
	else {
	    $filename = $line;
	    if ($filename eq "/home/tleek/hg/iferret-logging-new/target-i386/iferret_log_arg_fmt.h"
		|| $filename eq "/home/tleek/hg/iferret-logging-new/target-i386/iferret_ops.h") {
		$skip = 1;
	    }
	    else {
		$skip = 0;
	    }
	}
    }
    my $numOps = scalar keys %ops;
    print "$numOps unique info flow ops\n";
    

# examine iferret-socketcall.c to find out how to parse socket syscalls. 
    open F, "/home/tleek/hg/iferret-logging-new/target-i386/iferret-socketcall.c";
    my ($ebx,$name);
    my %socketcalls;
    while (my $line = <F>) {
#    if ($line =~ /case ([0-9]+): \/\/ ([a-z]+)\s*$/) {
#	$ebx = $1;
#	$name = $2;
#    }
	if ($line =~ /^\s+\/\//) {
	    next;
	}
	if ($line =~ /info_flow_log_socketcall_write/) {
	    if ($line =~ /info_flow_log_socketcall_write\s*\(scp,([^,]+),(.*)\)\;/) {
		my $name = $1;
		my $args = $2;
		&add_socketcall(\%socketcalls, $name, $args, $line);	    
	    }
	    else {
		print "$line\n";
		die "Cant parse?\n";
	    }
	}
    }
    my $numSocketcalls = scalar keys %socketcalls;
    print "$numSocketcalls unique socket calls\n";





# create the big case stmt for writing syscall log items
    open SW, ">iferret_syscall_switch.h";
    print SW "// NB: This code is auto-generated by make_iferret_code.pl. \n";
    print SW "// It's job is to generate iferret log entries for system calls.\n";
    print SW "// It should be included in iferret-syscalls.c\n";
    print SW "switch (EAX) {\n";
    for (my $i=0; $i<=$maxSyscallNum; $i++) {
	if (exists $syscall[$i]) {
	    my $call_num = $syscall[$i]{call_num};
	    if ($call_num == 102) {
		# don't bother with socketcall
		next;
	    }
	    print SW "case $call_num: // $i $syscall[$i]{proto}\n";
	    my $numStrings = 1;
	    unless (exists $syscall[$i]{noargs}) {
		my $n = scalar @{$syscall[$i]{args}};
		for (my $j=0; $j<$n; $j++) {
		    my $t = $syscall[$i]{args}[$j];
		    if ($t eq "s") {
			print SW "if (copy_string(str$numStrings, $syscallRegArgs[$j])) { \n";
			$numStrings ++;
		    }
		}
	    }
	    print SW "info_flow_log_syscall_write(scp";
	    unless (exists $syscall[$i]{noargs}) {
		my $n = scalar @{$syscall[$i]{args}};
		my $numStrings = 1;
		for (my $j=0; $j<$n; $j++) {
		    my $t = $syscall[$i]{args}[$j];
		    if ($t eq "s") {
			print SW ",str$numStrings";
			$numStrings ++;
		    }
		    else {
			print SW ",$syscallRegArgs[$j]";
		    }
		}
	    }
	    print SW ");\n";
	    for (my $j=0; $j<$numStrings-1; $j++) {
		print SW "}\n";
	    }
	    print SW "break; \n";
	}
    }
    print SW "}\n";
    close SW;



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
		$comment .= "// --> $line\n";
	    }
	}
	$enum[$ii]{comment} = $comment;
	$enum[$ii]{format} = $ops{$opname}{format};
	$ii++;
    }

# second, all the syscalls (except socketcall)
    for (my $i=0; $i<=$maxSyscallNum; $i++) {
	if (! (exists $syscall[$i])) {
	    next;
	}
	my $name = $syscall[$i]{name};
	my $opname = $name;
	$opname =~ tr/a-z/A-Z/;
	$opname = "IFLO_SYS_" . $opname;
	print "enum $ii syscall $opname\n";
	$enum[$ii]{opname} = $opname;
	my $comment = "// $ii \n";
	$comment .= "// syscall # $syscall[$i]{call_num}\n";
	$comment .= "// $syscall[$i]{proto}";
	$enum[$ii]{comment} = $comment;
	my $format = "";
	if (exists $syscall[$i]{args}) {
	    if ((scalar @{$syscall[$i]{args}}) == 0) {
		$format = "0";
	    }
	    else {
		foreach my $arg (@{$syscall[$i]{args}}) {
		    if ($arg eq "4" || $arg eq "s") {
			$format .= $arg;
		    }
		}
	    }
	} 
	else {
	    $format = "0";
	}
	$enum[$ii]{format} = $format;    
	$ii++;
    }

# third, all the socketcalls 
    foreach my $opname (sort keys %socketcalls) {
	$enum[$ii]{opname} = $opname;
	print "enum=$ii socketcall $opname\n";
	$enum[$ii]{comment} = $socketcalls{$opname}{comment};
	$enum[$ii]{format} = $socketcalls{$opname}{format};
	$ii++;
    }


# okay, now we have an ordering for log ops.
# create the file with the enum type

# write the file containing the big enum type
    open OPS, ">iferret_ops.h";
    print OPS "// NB: This code is auto-generated by make_iferret_code.pl. \n";
    print OPS "// It contains the big enum type of all info-flow ops.\n";
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
    print OPS "}\n";
    close OPS;

# create the big array of strings specifying log item formats
    open FMT, ">iferret_log_arg_fmt.h";
    print FMT "// NB: This code is auto-generated by make_iferret_code.pl. \n";
    print FMT "// It's job is to generate the big array of strings specifying the \n";
    print FMT "// format of iferret log entries for system calls.\n";
    print FMT "\n";
    print FMT "char *iferret_log_format[] = {\n";

    for (my $i=0; $i<scalar @enum; $i++) {
	my $opname = $enum[$i]{opname};
	print FMT "// $i \n";
	print FMT "$enum[$i]{comment}";
	print FMT "$enum[$i]{format}";
	if ($i < (scalar @enum)-1) {
	    print FMT ",";
	}
	print FMT "\n";
    }
    print FMT "}\n";
    close FMT;

} # main











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
	    || $arg eq "if_keyboard_label"
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
	    ) {
	    $fmt .= "4";
	    print "--> 4-byte int\n";
	}
	elsif ($arg eq "A0"
	       || $arg eq "ADDR"
	       || $arg eq "ptr"
	       || $arg eq "addr"
	       || $arg eq "RYANS_MAGIC_NUMBER_2"
	       || $opname eq "HD_TRANSFER"
	    ) {
	    $fmt .= "8";
	    print "--> 8-byte int\n";
	}
	else {
	    print "Unrecognized arg: $arg. Using fmt of 4\n";
	    $fmt .= "4";
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
    $rhOps->{$opname}{files}{$filename} = ();
    # and foreach, collect all lines used to write this op
    push @{$rhOps->{$opname}{files}{$filename}}, $line;
    
    # create or update format string for this op by examining args.
#    print "$opname $filename\n";
    my $newFmt;   
    if (!exists $rhOps->{$opname}{format}) {
	$newFmt = &create_op_format($opname, \@rest);
    }
    else {
	$newFmt = &update_op_format($opname, $rhOps->{$opname}{format}, \@rest);
    }

    print "2 op is $opname. \n";
    print "$filename\n";
    print "$line\n";
    $rhOps->{$opname}{format} = &merge_formats ($fmt, $newFmt);
}



sub add_socketcall() {
    my ($rhOps, $opname, $args, $line) = @_;

    my @rest = split ',', $args;
    # create or update format string for this op by examining args.
    $rhOps->{$opname}{format} = &create_op_format($opname, \@rest);
    $rhOps->{$opname}{comment} = $line;
}


