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

# read in all the syscall prototypes
open F, "./syscall-prototypes";
my $maxSyscallNum = 0;
my %syscall;
while (my $line = <F>) {
    chomp $line;
    my @foo = split ' ', $line;
    my $num = shift @foo;
    $syscall{$num} = {};
    $syscall{$num}{proto} = $line;
    if ($num > $maxSyscallNum) {
	$maxSyscallNum = $num;
    }
    my $rest = join ' ', @foo;
    my $rettype = shift @foo;
    if ($rettype eq "missing") {
	# we have no prototype for this syscall
	my $name = shift @foo;
	$syscall{$num}{name} = $name;
	$syscall{$num}{noargs} = 1;
    }
    elsif ($rest =~ /(.*)\s(.*)\((.*)\)/) {
	# we have a prototype.  examine the args
	my $type = $1;
	my $name = $2;
	my $inside = $3;
	$syscall{$num}{name} = $name;
	$syscall{$num}{args} = ();
	if ($inside eq "void") {
	    $syscall{$num}{noargs} = 1;
	}
	else {
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
		    push @{$syscall{$num}{args}}, "string";
		}
		else {
		    push @{$syscall{$num}{args}}, "int";
		}
	    }
	}
    }
}
my $numSyscalls = scalar keys %syscall;
print "$numSyscalls unique syscalls logged\n";


my $iferretDir = "/home/tleek/hg/iferret-logging-new";



sub create_op_format() {
    my ($opname, $raArgs) = @_;
    my $fmt = "";
    foreach my $arg (@{$raArgs}) {
	$arg =~ s/^\s+//g;
	$arg =~ s/\s+$//g;
	if ($arg =~ /^str/
	    || $arg =~ /label$/
	    ) {
	    # in order for this script to recognize an op logfile entry
	    # arg as a string, it must be called "str..."
	    $fmt .= "s";
	}
	elsif ($arg =~ /^REGNUM/ 
	       || $arg =~ /MEMWRITE/ 
	       || $arg =~ /MEMSUFFIXNUM/ 
	       || $arg =~ /SHIFT/
	       || $arg =~ /cpl/) {
	    # a register number -- only one byte necessary
	    $fmt .= "1"; 
	}
	elsif ($arg =~ /current_pid/ 
	       || $arg =~ /current_uid/
	       || $arg =~ /EAX/
	       || $arg =~ /EBX/
	       || $arg =~ /ECX/
	       || $arg =~ /EDX/
	       || $arg =~ /ESI/
	       || $arg =~ /EDI/
	       || $arg =~ /EBP/
	       || $arg =~ /ESP/
	       || $arg =~ /T0/
	       || $arg =~ /T1/
	       || $arg =~ /^[0-9]+$/
	    ) {
	    $fmt .= "4";
	}
	elsif ($arg =~ /A0/
	       || $arg =~ /ADDR/
	       || $opname =~ /HD_TRANSFER/
	    ) {
	    $fmt .= "8";
	}
	else {
	    print "Unrecognized arg: $arg. Using fmt of 4\n";
	    $fmt .= "4";
	}
    }
#    print "$opname ";
#    print (scalar @{$raArgs});
#    print " args (". (join ', ', @{$raArgs}) .") --> fmt is [$fmt]\n";
    return $fmt;
}



sub update_op_format() {
    my ($opname, $fmt, $raArgs) = @_;

    my $newFmt = &create_op_format($opname, $raArgs);
    if (length $fmt != length $newFmt) {
	die "$opname $fmt and $newFmt are of different length?\n"
    }
    my $mergedFmt = "";
    for (my $i=0; $i<length $fmt; $i++) {
	my $f = substr($fmt,$i,1);
	my $newF = substr($newFmt,$i,1);
	if ($f eq $newF) {
	    $mergedFmt .= $f;
	}
	elsif (($f =~ /^[0-9]$/)
	       && ($newF =~ /^[0-9]$/)) {
	    $f *= 1;
	    $newF *= 1;
	    if ($f > $newF) {
		$mergedFmt .= $f;
	    }
	    else {
		$mergedFmt .= $newF;
	    }
	}
	else {
	    print "dont know how to merge $f and $newF.\n";
	    die;
	}
    }
    return ($mergedFmt);
}


# examine all the source files looking for info_flow_log_op_write calls

my @lines = `cd $iferretDir; find -H . -name \"*.[ch]\" -print -exec grep \"info_flow_log_op_write\" \'{}\' \\;`;

my %ops;
my $filename;
my $numOps = 0;
foreach my $line (@lines) {
    chomp $line;
    if ($line =~ /info_flow_log_op_write/) {
	if ($line =~ /info_flow_log_op_write\((.*)\)\;/) {
	    my $args = $1;
	    my @rest = split ',', $args;
	    my $opname = shift @rest;
	    # remember all the files we saw this in.
 	    $ops{$opname}{files}{$filename} = 1; 
	    # create or update format string for this op by examining args.
	    if (!exists $ops{$opname}{format}) {
		$ops{$opname}{format} = &create_op_format($opname, \@rest);
	    }
	    else {
		$ops{$opname}{format} = &update_op_format($opname, $ops{$opname}{format}, \@rest);
	    }
	}
	else {
	    unless ($line =~ /\)(\s*)\{/) {
		die "can't parse $line\n";
	    }
	}
    }
    else {
	$filename = $line;
    }
}
my $numOps = scalar keys %ops;
print "$numOps info flow ops logged\n";



# create the big case stmt for writing syscall log items
open F, ">iferret_syscall_switch.h";
print F "// NB: This code is auto-generated by make_iferret_code.pl. \n";
print F "// It's job is to generate iferret log entries for system calls.\n";
print F "// It should be included in iferret-syscalls.c\n";
print F "switch (EAX) {\n";
for (my $i=0; $i<=$maxSyscallNum; $i++) {
    if (exists $syscall{$i}) {
	print F "case $i: // $syscall{$i}{proto}\n";
	my $numStrings = 1;
	unless (exists $syscall{$i}{noargs}) {
	    my $n = scalar @{$syscall{$i}{args}};
	    for (my $j=0; $j<$n; $j++) {
		my $t = $syscall{$i}{args}[$j];
		if ($t eq "string") {
		    print F "if (copy_string(str$numStrings, $syscallRegArgs[$j])) { \n";
		    $numStrings ++;
		}
	    }
	}
	print F "info_flow_log_syscall_write(scp";
	unless (exists $syscall{$i}{noargs}) {
	    my $n = scalar @{$syscall{$i}{args}};
	    my $numStrings = 1;
	    for (my $j=0; $j<$n; $j++) {
		my $t = $syscall{$i}{args}[$j];
		if ($t eq "string") {
		    print F ",str$numStrings";
		    $numStrings ++;
		}
		else {
		    print F ",$syscallRegArgs[$j]";
		}
	    }
	}
	print F ");\n";
	for (my $j=0; $j<$numStrings-1; $j++) {
	    print F "}\n";
	}
	print F "break; \n";
    }
}
print F "}\n";
close F;


# create the big array of strings specifying log item formats
open F, ">iferret_log_arg_fmt.h";
print F "// NB: This code is auto-generated by make_iferret_code.pl. \n";
print F "// It's job is to generate the big array of strings specifying the \n";
print F "// format of iferret log entries for system calls.\n";
print F "\n";
print F "char *iferret_log_format[] = {\n";

open F2, ">iferret_ops.h";
print F2 "// NB: This code is auto-generated by make_iferret_code.pl. \n";
print F2 "// It contains the big enum type of all info-flow ops.\n";
print F2 "\ntypedef enum iferret_log_op_enum {\n";

my $ii = 0;

# first all the ops
foreach my $opname (sort keys %ops) {
    
    print F2 "  $opname // $ii \n";
#    foreach my $file (sort keys %{$ops{$opname}{files}}) {
#	print F2 "$file ";
#    }
    print F2 "\n";
    
    print F "\"$ops{$opname}{format}\"";

    if ($ii < $numOps) {
	print F ",";
	print F2 ",";
    }
    print F " // $ii $opname \n";
    print F2 " // $ii $opname \n";
    $ii ++;
    
}


# second all the syscalls
for (my $i=0; $i<=$maxSyscallNum; $i++) {
    if (! (exists $syscall{$i})) {
	next;
    }
    my $name = $syscall{$i}{name};
#    print "$name\n";
    my $opname = $name;
    $opname =~ tr/a-z/A-Z/;
#    print "$name $opname\n";
    print F2 "  $opname";
    if (exists $syscall{$i}) {
	print F "  \"";
	if (exists $syscall{$i}{args}) {
	    foreach my $arg (@{$syscall{$i}{args}}) {
		if ($arg eq "int") {
		    print F "4";  # four bytes
		}
		if ($arg eq "string") {
		    print F "s";
		}
	    }
	}
	print F "\"";
	if ($i < $maxSyscallNum) {
	    printf F ",";
	    printf F2 ",";
	}
	print F " // $ii -- syscall # $i\n";
	print F2 " // $ii -- syscall # $i\n";
    }
    $ii++;
}
print F "}\n";
print F2 "}\n";

close F;
