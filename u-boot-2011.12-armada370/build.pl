#!/usr/bin/perl

use Cwd qw();

sub HELP_MESSAGE
{
    print "\n";
	print "Usage  : build -f \"Flash type\" -v X.X.X [-b \"board name\"] [-c] [-o \"Output file\"] \\\n";
	print "              [-s \"rsa_priv_key\" [-a \"aes_128_key\"] -k \"board ID\" -m \"Flash ID\" [-j \"ms\"]]\n";
	print "Example: build -f nor -b pcac -c\n";
	print "\n";
	print "Options:\n";
	print "\t-f\tBoot device/interface. Accepts spi, nor, nand\n";
	print "\t-b\tBoard type. Accepted types:\n";
	print "\t\t\tarmada_xp_db, armada_xp_rdnas, armada_xp_pcac armada_xp_rdserver armada_xp_dbgp,\n";
	print "\t\t\tarmada_xp_rdcustomer armada_370_db armada_370_rd bobcat2_db bobcat2_rd\n";
	print "\t-c\tClean build. calls \"make mrproper\"\n";
	print "\t-o\tOutput dir/file. The image will be copied into this dir/file\n";
	print "\t-e\tBig Endian platform. If not specified, the Little endian is assumed\n";
	print "\t-i\tList of additionally supported interfaces. A semicolon \":\" seperates the list elements\n";
	print "\t\t\tSupports spi, nor, nand. The boot interface will always be suppored\n";
	print "\t-n\tNAND Flash controller. Accepted nfc(defualt), lnc(legacy controller)\n";
	print "\t-v\tSW version (add to binary file name u-boot--axp-X.X.X-spi.bin)\n";
	print "\t-D\tPath to DTB to embed\n";
	print "\n";
	print "Secure boot options:\n";
	print "\t-s\tUse secure boot mode and build image signed with RSA-2048 signature\n";
	print "\t\t\tIf the private key file name is \"@@\", the new RSA key pair will be generated and used\n";
	print "\t\t\tThis option is ignored if typed without any value\n";
	print "\t-a\tEncrypt boot image with AES-128 key.\n";
	print "\t\t\tIf the AES key file name is \"@@\", the new key will be generated\n";
	print "\t\t\tThis option is valid only if secure boot mode is selected and ignored if typed without any value\n";
	print "\t-j\tIn secure boot mode - enable JTAG interface after parsing the boot header \n";
	print "\t\t\tand wait the \"ms\" milliseconds before continuing the boot flow\n";
	print "\t-k\tHexadecimal value of Board ID. This paranmeter is MANDATORY in secure boot mode\n";
	print "\t-m\tHexadecimal value of Flash ID. This paranmeter is MANDATORY in secure boot mode\n";
	print "\n";
	print "Environment Variables:\n";
	print "\tCROSS_COMPILE     Cross compiler to build U-BOOT\n";
	print "\tCROSS_COMPILE_BH  Cross compiler to build bin hdr\n";
	print "\n";
}

# Main
use Getopt::Std;

getopt('f:b:o:i:v:n:s:a:k:m:j:p:m:D:');

$cross    = $ENV{'CROSS_COMPILE'};
$cross_bh = $ENV{'CROSS_COMPILE_BH'};

if(!defined $cross){
	printf " *** Error: Please set environment variables CROSS_COMPILE\n";
	HELP_MESSAGE();
	exit 1;
}
if(!defined $cross_bh){
	printf " *** Error: Please set environment variables CROSS_COMPILE_BH\n";
	HELP_MESSAGE();
	exit 1;
}
# Validate flash type
if($opt_f eq "spi")
{
	$img_opts   = "";
	$flash_name = "spi";
	$img_type   = "flash";
}
elsif ($opt_f eq "nor")
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
	if (defined) {
		print "\n *** Error: Bad flash type $opt_f specified\n\n";
	}
	else {
		print "\n *** Error: Flash type unspecified\n\n";
	}
	HELP_MESSAGE();
	exit 1;
}

# RSA key
if (defined $opt_s) {
    if ($opt_s eq "@@") {
	print("Secure boot, generate new RSA key\n");
    } else {
	printf("Use RSA key from file : %s\n", $opt_s);
    }
    $rsa_opts = "-Z $opt_s";
    $bin_hdr_n = "bin_hdr_sec.bin"
} else {
    print("No secure boot option selected\n");
    $rsa_opts = "";
    $bin_hdr_n = "bin_hdr.bin"
}

# AES key
if (defined $opt_s) {
    if (defined $opt_a) {
	if ($opt_a eq "@@") {
	    print("Secure boot, encrypted image, generate new AES key\n");
	} else {
	    printf("Secure boot, encrypted image, use AES key from file : %s\n", $opt_a);
	}
	$aes_opts = "-A $opt_a";
    } else {
	print("Secure boot, no image encryption\n");
	$aes_opts = "";
    }
}

# Flash ID, Board ID and JTAG
if ($opt_s) {
    if(!defined $opt_k){
	HELP_MESSAGE();
	print("***Error: Board ID is a mandatory parameters for secure mode!\n\n");
	exit 1;
    }
    if(!defined $opt_m){
	HELP_MESSAGE();
	print("***Error: Flash ID is a mandatory parameters for secure mode!\n\n");
	exit 1;
    }
    if(!defined $opt_j){
	$id_opts = "-B $opt_k -F $opt_m";
    } else {
	$id_opts = "-B $opt_k -F $opt_m -J $opt_j";
    }
    printf("Secure boot, Additional options : %s\n", $id_opts);
}
# Handle clean build
if($opt_c eq 1)
{
	if(($opt_b eq "armada_xp_db") or
           ($opt_b eq "armada_xp_rdnas") or
           ($opt_b eq "armada_xp_pcac") or
           ($opt_b eq "armada_xp_amc") or
           ($opt_b eq "armada_xp_rdserver") or
           ($opt_b eq "armada_xp_dbgp") or
           ($opt_b eq "armada_370_db") or
           ($opt_b eq "armada_370_rd") or
           ($opt_b eq "armada_xp_rdcustomer") or
           ($opt_b eq "bobcat2_db") or
           ($opt_b eq "bobcat2_rd"))
	{
         $board = $opt_b;

                if( (substr $board,7 , 3) eq "370" ) {
				$boardID="a370";
                                $targetBoard = substr $board, 11;
		}
		elsif ( (substr $board,7 , 2) eq "xp" ) {
				$boardID="axp";
        $targetBoard = substr $board, 10;
	}
		elsif ( (substr $board,0 , 7) eq "bobcat2" ) {
				$boardID="msys";
                                $targetBoard = substr $board, 8;
		}
	}
	else
	{
		if (defined) {
			print "\n *** Error: Bad board type $opt_b specified\n\n";
		}
		else {
			print "\n *** Error: Board type unspecified\n\n";
		}
		HELP_MESSAGE();
		exit 1;
	}

	# Configure Make
	system("make mrproper");
	print "\n**** [Cleaning Make]\t*****\n\n";

        my $path = Cwd::cwd();
#	print " clean tools\n";
	chdir  ("./tools/marvell");
        system("make clean BOARD=$boardID -s");
	chdir  ("$path");
#	print "\n**** [Configuring Make] version $opt_v\t to board $targetBoard *****\n\n";
	system("make ${board}_config");

	# Set pre processors
	print "\n**** [Setting Macros]\t*****\n\n";
	if($opt_f eq "spi")      {
		system("echo \"#define MV_SPI_BOOT\" >> include/config.h");
		system("echo \"#define MV_INCLUDE_SPI\" >> include/config.h");
                print "Boot from SPI\n";

	}
	elsif ($opt_f eq "nor")  {
		system("echo \"#define MV_NOR_BOOT\" >> include/config.h");
                print "Boot from NOR\n";
	}
	elsif  ($opt_f eq "nand"){
		system("echo \"#define MV_NAND_BOOT\" >> include/config.h");
                print "Boot from NAND\n";
	}

	# Big endian place holder
	if(defined $opt_e) {

        $endian = "be";
	system("echo \"#define __BE\" >> include/config.h");
	system("echo \"BIG_ENDIAN = y\" >> include/config.mk");
	system("echo \"LDFLAGS += -EB  \" >> include/config.mk");
	system("echo \"LDFLAGS_FINAL += -be8  \" >> include/config.mk");
	system("echo \"  * Big Endian byte ordering \"");
	system("echo \"PLATFORM_CPPFLAGS += -march=armv7-a \" >>  arch/arm/cpu/armv7/config.mk");
	system("echo \"#define CPU_ARMARCH7 \" >> include/config.h");
	system("echo \"  * ARM Architecture 7 - Using be8 compile flag\"");
	system("echo \"CPPFLAGS += -falign-labels=4\" >> include/config.mk");
	system("echo \"CFLAGS += -mno-tune-ldrd\" >> include/config.mk");
        print "** BIG ENDIAN ** \n";
	}
        else {
           $endian = "le";
           print "** Little ENDIAN ** \n";
        }
        if($opt_n eq "lnc")
        {
                $nandController = "lnc";
                system("echo \"#define CONFIG_MTD_NAND_LNC\" >> include/config.h");
                print "Enable Legacy NAND controller \n";
        }
        else
        {
                $nandController = "nfc";
                system("echo \"#define CONFIG_MTD_NAND_NFC\" >> include/config.h");
                print "Enable NAND controller NFC\n";
        }

	#Interface support
	if(defined $opt_i)
	{
		@interfaces = split(':', $opt_i);

		if (($boardID eq "a370") and
                        ($nandController eq "nfc") and
			(grep{$_ eq 'nor'} @interfaces)  and
			(grep{$_ eq 'nand'} @interfaces))
		{
			print"\n *** Error: Armada-370 does not Support nand and nor interfaces together\n";
			exit 1;
		}

		print "Support flash: ";
		foreach $if (@interfaces)
		{
			if   ($if eq "spi"){
                           system("echo \"#define MV_INCLUDE_SPI\" >> include/config.h");
                           print "SPI ";
                           }
			elsif($if eq "nor"){
                           system("echo \"#define MV_INCLUDE_NOR\" >> include/config.h");
                           print "NOR ";
                           }
			elsif($if eq "nand"){
                           system("echo \"#define MV_NAND\" >> include/config.h");
                           print "NAND ";
                           }
			else {
				print " *** Warning: Ignoring unrecognized interface - $if";
			}
		}
		print "\n";
	}

	if(defined $opt_p)
	{
		print "Project: ";
		if   ($opt_p eq "usi_rn104"){
			system("echo \"#define NETGEAR_RN104\" >> include/config.h");
			print "RN104 ";
		}
		elsif ($opt_p eq "usi_rn102"){
			system("echo \"#define NETGEAR_RN102\" >> include/config.h");
			print "RN102 ";
		}
		elsif ($opt_p eq "usi_rn25"){
			system("echo \"#define NETGEAR_RN25\" >> include/config.h");
			print "RN25 ";
		}
		else {
			print " *** Warning: Ignoring unrecognized project - $p";
		}
	}

}
if($opt_d eq 1) {
	system("echo \"DDR3LIB = 1\" >> include/config.mk");
	print "\n *** DDR3LIB = 1 *********************************\n\n";
}
if ($opt_D) {
	$dtb_opts = "-d $opt_D ";
}

# Build !
print "\n**** [Building U-BOOT]\t*****\n\n";
$fail = system("make -j6 -s");

if($fail){
	print "\n *** Error: Build failed\n\n";
	exit 1;
}

#Create Image and Uart Image
print "\n**** [Creating Image]\t*****\n\n";
$failUart = system("./tools/marvell/doimage -T uart -D 0 -E 0 -G ./tools/marvell/bin_hdr/bin_hdr.uart.bin u-boot.bin u-boot-$boardID-$opt_v-$flash_name-$targetBoard-uart.bin");
$fail = system("./tools/marvell/doimage -T $img_type -D 0x0 -E 0x0 $img_opts $rsa_opts $aes_opts $id_opts $dtb_opts -G ./tools/marvell/bin_hdr/$bin_hdr_n u-boot.bin u-boot-$boardID-$opt_v-$flash_name-$targetBoard.bin");

if($fail){
	print "\n *** Error: Doimage failed\n\n";
	exit 1;
}
if($failUart){
	print "\n *** Error: Doimage for uart image failed\n\n";
	exit 1;
}

if(defined $opt_o)
{
	print "\n**** [Copying Image]\tto ",$opt_o,"  *****\n\n";
	system("mkdir -p $opt_o/$endian/$opt_f");
	system("mkdir -p $opt_o/bin_hdr");
	system("cp u-boot-$boardID-$opt_v-$flash_name-$targetBoard.bin $opt_o/u-boot.bin");
	system("cp u-boot-$boardID-$opt_v-$flash_name-$targetBoard.bin $opt_o/$endian/$opt_f/ ");
	system("cp u-boot $opt_o/$endian/$opt_f/u-boot-$boardID-$opt_v-$flash_name-$targetBoard");
	system("cp u-boot.srec $opt_o/$endian/$opt_f/u-boot-$boardID-$opt_v-$flash_name-$targetBoard.srec");
        system("cp u-boot-$boardID-$opt_v-$flash_name-$targetBoard-uart.bin $opt_o/$endian/$opt_f/");

        system("cp tools/marvell/bin_hdr/bin_hdr.bin $opt_o/bin_hdr/");
        system("cp tools/marvell/bin_hdr/bin_hdr.elf $opt_o/bin_hdr/");
        system("cp tools/marvell/bin_hdr/bin_hdr.dis $opt_o/bin_hdr/");
        system("cp tools/marvell/bin_hdr/bin_hdr.srec $opt_o/bin_hdr/");
        system("cp tools/marvell/bin_hdr/bin_hdr_sec.bin $opt_o/bin_hdr/");
        system("cp tools/marvell/bin_hdr/bin_hdr_sec.elf $opt_o/bin_hdr/");
        system("cp tools/marvell/bin_hdr/bin_hdr_sec.dis $opt_o/bin_hdr/");
        system("cp tools/marvell/bin_hdr/bin_hdr_sec.srec $opt_o/bin_hdr/");
        system("cp tools/marvell/bin_hdr/bin_hdr.uart.bin $opt_o/bin_hdr/");
        system("cp tools/marvell/bin_hdr/bin_hdr.uart.elf $opt_o/bin_hdr/");
        system("cp tools/marvell/bin_hdr/bin_hdr.uart.dis $opt_o/bin_hdr/");
        system("cp tools/marvell/bin_hdr/bin_hdr.uart.srec $opt_o/bin_hdr/");
}

exit 0;
