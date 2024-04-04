#!/usr/bin/perl

# Main
use Getopt::Std;
 
getopt('f:v:n');

if ($opt_f eq "nor")
{
	$img_opts   = "";
	$flash_name = "nor";
#	$flash_name2 = "nand";
	$img_type   = "flash";
}
elsif  ($opt_f eq "nand")
{
	$img_opts   = "-P 4096 -L 128 -N MLC";
	$flash_name = "nand";
#	$flash_name2 = "nand";
	$img_type   = "nand";
}
else
{
	$flash_name = "spi";
#	$flash_name2 = "nor";
#	$flash_name2 = "nand";
}


if(defined $opt_n) {
   $fail = system("./build.pl  -f $flash_name -v $opt_v  -n $opt_n -b armada_370_db  -i spi:nand:nor -c -o ");
}
else
{
   $fail = system("./build.pl  -f $flash_name -v $opt_v  -b armada_370_db  -i spi:nand -c -o ");
}
if($fail){
	print  "\n *** Error: Build u-boot bot from $flash_name failed\n\n";
	exit;
}

$fail = system("cp u-boot-a370-$opt_v-$flash_name-db.bin /tftpboot/u-boot.bin");
if($fail){
	print  "\n *** Error: Build u-boot bot from $flash_name failed\n\n";
	exit;
}
	print  "====> copied cp u-boot-a370-$opt_v-$flash_name-db.bin /tftpboot/u-boot.bin \n\n";

