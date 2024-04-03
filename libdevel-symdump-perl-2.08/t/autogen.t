#!/usr/bin/perl -w

BEGIN { unshift @INC, '.' ;}

require Devel::Symdump;

print "1..8\n";

@p = qw(
scalars arrays hashes functions
unknowns filehandles dirhandles packages);

$i=0;
for (@p){
    @x1 = Devel::Symdump->new->$_();
    @x2 = Devel::Symdump->$_();
    unless ("@x1" eq "@x2"){
	print "x1 [@x1] x2 [@x2]\n";
	print "not ";
    }
    print "ok ", ++$i, "\n";
}

