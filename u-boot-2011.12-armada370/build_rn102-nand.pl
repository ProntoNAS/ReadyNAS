#!/usr/bin/perl

# Main
use Getopt::Std;
 
getopt('v:h:d:');

if(defined $opt_h) {
	print  "\nUsage  : ./usi-build_rn102-nand.pl -v X.X.X \n";
	exit;
}

if(!defined $opt_v) {
	print  " *** Error: Please set option -v, see USI-README for help\n";
	exit;
}

if(!defined $opt_d) {
	print  " *** Error: Please set option -d, to embed DTB\n";
	exit;
}

$fail = system("./build.pl -p usi_rn102 -f nand -v $opt_v -D $opt_d -b armada_370_db  -i nand -c -o");
if($fail){
	print  "\n *** Error: Build u-boot boot from NAND failed\n\n";
	exit;
}

$fail = system("mv u-boot-a370-$opt_v-nand-db.bin u-boot-rn102-nand.bin");
if($fail){
	print  "\n *** Error: Build u-boot bot from nand failed\n\n";
	exit;
}
	print  "====> moved mv u-boot-a370-$opt_v-nand-db.bin u-boot-rn102-nand.bin \n\n";

