#!/usr/bin/perl -w

use strict;



# This script examines all the *.c and *.h files in target-i386 and pulls out every
# info-flow op.  Then it synthesizes the typedef enum for info_flow.h as well as the
# array used to map from those enums to strings.  

my %ops;
my $i=0;
foreach my $rop (`grep IFLW *.[ch] */*.[ch]`) {
    if ($rop =~ /#define/) {
	next;
    }
    my @foo = split ' ', $rop;
    if (scalar @foo != 2) {
	next;
    }
    my $rop1 = $foo[1];
    my $op;
    if ($rop1 =~ /^IFLW\((.*)$/) {
	$op = $1;
    }
    elsif ($rop1 =~ /IFLW_ADDR\((.*)$/) {
	$op = "ADDR_$1";
    }
    elsif ($rop1 =~ /IFLW_REG\((.*)$/) {
	$op = "REG_$1";
    }
    elsif ($rop1 =~ /IFLW_SHIFT\((.*)$/) {
	$op = "SHIFT_$1";
    }
    elsif ($rop1 =~ /IFLW_MEMSUFFIX_ADDR\((.*)$/) {
	$op = "MEMSUFFIX_ADDR_$1";
    }
    elsif ($rop1 =~ /IFLW_SHIFT_MEM_WRITE\((.*)$/) {
	$op = "SHIFT_MEM_WRITE_$1";
    }
    else {
	next;
    }

    $op =~ s/[\)\;\,]//g;
    $ops{$op} = 1;
    
    print "$i $rop $op\n";
    $i++;
}

# add this one by hand. yucky.
$ops{"INB"} = 1;


print "\n\n\nPaste this bit into info_flow.h\n\n";
print "enum info_flow_op_enum_t {\n";
foreach my $op (sort keys %ops) {
    print "INFO_FLOW_OP_$op,\n";
}
print "}\n";


print "\n\n\nPaste this bit into info_flow.c\n\n";
print "char *if_op_str(if_op_t if_op) {\n";
print "  char *ifs = NULL;\n";
print "\n  switch (if_op) {\n";
foreach my $op (sort keys %ops) {
    my $lop = $op;
    $lop =~ tr/A-Z/a-z/;
    print "  case INFO_FLOW_OP_$op:\n";
    print "    ifs = \"$lop\";\n";
    print "    break;\n";
}
print "  default:\n";
print "    ifs =\"WFT_MATE\";\n";
print "  }\n";
print "  return ifs;\n";
print "}\n";

