#!/usr/bin/perl -w

use strict;

open F, "/home/tleek/hg/iferret-logging-new/info-flow-ops";

my $prefix = "INFO_FLOW_OP_";

my @lines;
while (my $line = <F>) {
    if ($line =~ /^\/\//) {
	# skip comments
	next;
    }
    chomp $line;
    push @lines, $line;
}

print "// Put this in info_flow.h\n";
print "// \n";
print "typedef enum info_flow_op_enum {\n";
{
my $i = 0;
foreach my $line (@lines) {
    print "  $prefix$line, // $i\n";
    $i ++;
}
print "  DUMMY_LAST_OP\n";
print "} info_flow_op_enum_t;\n";
}

print "\n\n//Put this in info_flow.c\n";
print "// \n";
print "char *if_op_str(if_op_t if_op) {\n";
print "  char *ifs = NULL;\n";
print "\n";
print "  switch (if_op) {\n";
{
my $i = 0;
foreach my $line (@lines) {
    my $lcline = $line;
    $lcline =~ tr/A-Z/a-z/;
    print "  case $prefix$line:\n";
    print "    ifs = \"$lcline\";\n";
    print "    break;\n";
}
}
print "  default:\n";
print "    ifs =\"WTF_MATE\";\n";
print "  }\n";
print "  return ifs;\n";
print "}\n";


