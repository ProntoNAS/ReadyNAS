#!/usr/bin/perl

# Main
use Getopt::Std;
 
getopt('f:v');

if ($opt_f eq "nor")
{
	$img_opts   = "";
	$flash_name = "nor";
	$img_type   = "flash";
}
elsif  ($opt_f eq "nand")
{
	$img_opts   = "-P 4096 -L 128 -N MLC";
	$flash_name = "nand";
	$img_type   = "nand";
}
else
{
	$flash_name = "spi";
}


$fail = system("./build.pl  -f $flash_name -v $opt_v -b armada_xp_dbgp -i nand:nor:spi -c -o ");
if($fail){
	print  "\n *** Error: Build u-boot bot from $flash_name failed\n\n";
	exit;
}

$fail = system("cp u-boot-axp-$opt_v-$flash_name-dbgp.bin /tftpboot/u-boot.bin");
if($fail){
	print  "\n *** Error: copy u-boot-dbgp boot from $flash_name failed\n\n";
	exit;
}
	print  "====> copied cp u-boot-axp-$opt_v-$flash_name.bin /tftpboot/u-boot.bin \n\n";

