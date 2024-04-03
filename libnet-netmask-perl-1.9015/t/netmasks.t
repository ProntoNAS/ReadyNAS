#!/usr/bin/perl -I. -w

print "1..282\n";

use Net::Netmask;
use Net::Netmask qw(sameblock cmpblocks);
use Carp;
use Carp qw(verbose);

#  addr			mask		base		newmask	     bits  mb
my @rtests = qw(
 209.157.68.22:255.255.224.0	u	209.157.64.0	255.255.224.0	19 18
 209.157.68.22		255.255.224.0	209.157.64.0	255.255.224.0	19 18
 209.157.70.33		0xffffe000	209.157.64.0	255.255.224.0	19 18
 209.157.70.33/19		u	209.157.64.0	255.255.224.0	19 18
 209.157.70.33			u	209.157.70.33	255.255.255.255	32 32
 140.174.82			u	140.174.82.0	255.255.255.0	24 23
 140.174			u	140.174.0.0	255.255.0.0	16 15
 10				u	10.0.0.0	255.0.0.0	8  7
 10/8				u	10.0.0.0	255.0.0.0	8  7
 209.157.64/19			u	209.157.64.0	255.255.224.0	19 18
 209.157.64.0-209.157.95.255	u	209.157.64.0	255.255.224.0	19 18
 209.157/17			u	209.157.0.0	255.255.128.0	17 16
 default			u	0.0.0.0		0.0.0.0		0  0
);
push(@rtests, '209.157.68.22#0.0.31.255', 'u', '209.157.64.0', '255.255.224.0', '19', '18');

my @store = qw(
 209.157.64.0/19
 default
 209.157.81.16/28
 209.157.80.0/20
);

my @lookup = qw(
 209.157.75.75	209.157.64.0/19
 209.157.32.10	0.0.0.0/0
 209.157.81.18	209.157.81.16/28
 209.157.81.14	209.157.80.0/20
);

my @store2 = qw(
 209.157.64.0/19
 default
 209.157.81.16/28
 209.157.80.0/24
);

my @lookup2 = qw(
 209.157.75.75	209.157.64.0/19
 209.157.32.10	0.0.0.0/0
 209.157.81.18	209.157.81.16/28
 209.157.81.14	209.157.64.0/19
);

my $debug = 0;
my $test = 1;
my $x;

my ($addr, $mask, $base, $newmask, $bits, $max);
while (($addr, $mask, $base, $newmask, $bits, $max) = splice(@rtests, 0, 6)) {
	$mask = undef if $mask eq 'u';
	$x = new Net::Netmask ($addr, $mask);

	printf STDERR "test $test, %s %s: %s %s %d %d\n", 
		$addr, $mask, $x->base(), $x->mask(), 
		$x->bits(), $x->maxblock() if $debug;
	
	print $x->base() eq $base ? "ok $test\n" : "not ok $test\n"; $test++;
	print $x->mask() eq $newmask ? "ok $test\n" : "not ok $test\n"; $test++;
	print $x->maxblock() == $max ? "ok $test\n" : "not ok $test\n"; $test++;
	print $x->bits() == $bits ? "ok $test\n" : "not ok $test\n"; $test++;
}

my @y;

$x = new Net::Netmask ('209.157.64.0/19');
print $x->size() == 8192 ? "ok $test\n" : "not ok $test\n"; $test++;

print $x->hostmask() eq '0.0.31.255' ? "ok $test\n" : "not ok $test\n"; $test++;

@y = $x->inaddr();
print STDERR "REVERSE: @y\n" if $debug;
print $y[0] eq '64.157.209.in-addr.arpa' 
	? "ok $test\n" : "not ok $test\n"; $test++;
print $y[31*3] eq '95.157.209.in-addr.arpa' 
	? "ok $test\n" : "not ok $test\n"; $test++;
print defined($y[32*3]) ? "not ok $test\n" : "ok $test\n"; $test++;

$x = new Net::Netmask ('140.174.82.4/32');
print $x->size() == 1 ? "ok $test\n" : "not ok $test\n"; $test++;

# perl bug: cannot just print this.
my $p = ($x->inaddr())[0] eq '82.174.140.in-addr.arpa' 
	?  "ok $test\n"
	: "not ok $test\n";
print $p;
printf STDERR "REVERSE$test %s\n", $x->inaddr() if $debug;
$test++;

$x = new Net::Netmask ('140.174.82.64/27');
print (($x->inaddr())[1] == 64 ? "ok $test\n" : "not ok $test\n"); $test++;
print (($x->inaddr())[2] == 95 ? "ok $test\n" : "not ok $test\n"); $test++;
@y = $x->inaddr();
print STDERR "Y$test @y\n" if $debug;

$x = new Net::Netmask ('default');
print $x->size() == 4294967296 ? "ok $test\n" : "not ok $test\n"; $test++;

$x = new Net::Netmask ('209.157.64.0/27');
@y = $x->enumerate();
print $y[0] eq '209.157.64.0' ? "ok $test\n" : "not ok $test\n"; $test++;
print $y[31] eq '209.157.64.31' ? "ok $test\n" : "not ok $test\n"; $test++;
print defined($y[32]) ? "not ok $test\n" : "ok $test\n"; $test++;

$x = new Net::Netmask ('10.2.0.16/19');
@y = $x->enumerate();
print $y[0] eq '10.2.0.0' ? "ok $test\n" : "not ok $test\n"; $test++;
print $y[8191] eq '10.2.31.255' ? "ok $test\n" : "not ok $test\n"; $test++;
print defined($y[8192]) ? "not ok $test\n" : "ok $test\n"; $test++;

my $table = {};
my $table9 = {};

for my $b (@store) {
	$x = new Net::Netmask ($b);
	$x->storeNetblock();
}

for my $b (@store2) {
	$x = new Net::Netmask ($b);
	$x->storeNetblock($table);
	$x->storeNetblock($table9);
}

my $result;
while (($addr, $result) = splice(@lookup, 0, 2)) {
	my $nb = findNetblock($addr);
	printf STDERR "lookup(%s): %s, wanting %s.\n",
		$addr, $nb->desc(), $result if $debug;
	print $nb->desc() eq $result ? "ok $test\n" : "not ok $test\n"; $test++;
}

while (($addr, $result) = splice(@lookup2, 0, 2)) {
	my $nb = findNetblock($addr, $table);
	printf STDERR "lookup(%s): %s, wanting %s.\n",
		$addr, $nb->desc(), $result if $debug;
	print $nb->desc() eq $result ? "ok $test\n" : "not ok $test\n"; $test++;
}


$newmask = Net::Netmask->new("192.168.1.0/24");
print (($newmask->broadcast() eq "192.168.1.255") ? "ok $test\n" : "not ok $test\n"); $test++;
print (($newmask->next() eq "192.168.2.0") ? "ok $test\n" : "not ok $test\n"); $test++;
print ($newmask->match("192.168.0.255") ? "not ok $test\n" : "ok $test\n"); $test++;
print ($newmask->match("192.168.2.0") ? "not ok $test\n" : "ok $test\n"); $test++;
print ($newmask->match("10.168.2.0") ? "not ok $test\n" : "ok $test\n"); $test++;
print ($newmask->match("209.168.2.0") ? "not ok $test\n" : "ok $test\n"); $test++;
print ($newmask->match("192.168.1.0") ? "ok $test\n" : "not ok $test\n"); $test++;
print ($newmask->match("192.168.1.255") ? "ok $test\n" : "not ok $test\n"); $test++;
print ($newmask->match("192.168.1.63") ? "ok $test\n" : "not ok $test\n"); $test++;

print (($newmask->nth(1) eq '192.168.1.1') ? "ok $test\n" : "not ok $test\n"); $test++;
print (($newmask->nth(-1) eq '192.168.1.255') ? "ok $test\n" : "not ok $test\n"); $test++;
print (($newmask->nth(-2) eq '192.168.1.254') ? "ok $test\n" : "not ok $test\n"); $test++;
print (($newmask->nth(0) eq '192.168.1.0') ? "ok $test\n" : "not ok $test\n"); $test++;
print (($newmask->match('192.168.1.1') == 1) ? "ok $test\n" : "not ok $test\n"); $test++;
print (($newmask->match('192.168.1.100') == 100) ? "ok $test\n" : "not ok $test\n"); $test++;
print (($newmask->match('192.168.1.255') == 255) ? "ok $test\n" : "not ok $test\n"); $test++;

print (($newmask->match('192.168.2.1') == 0) ? "ok $test\n" : "not ok $test\n"); $test++;
print (!($newmask->match('192.168.2.1')) ? "ok $test\n" : "not ok $test\n"); $test++;
print (((0+$newmask->match('192.168.1.0')) == 0) ? "ok $test\n" : "not ok $test\n"); $test++;
print (($newmask->match('192.168.1.0')) ? "ok $test\n" : "not ok $test\n"); $test++;

my $bks;
$block = new Net::Netmask '209.157.64.1/32';
$block->storeNetblock($bks);
print findNetblock('209.157.64.1',$bks) ? "ok $test\n" : "not ok $test\n"; $test++;


my @store3 = qw(
 216.240.32.0/19
 216.240.40.0/24
 216.240.40.0/27
 216.240.40.4/30
);
my $table3 = {};
my $table8 = {};
my $table7 = {};
my $table6 = {};
for my $b (@store3) {
	$x = new Net::Netmask ($b);
	$x->storeNetblock($table3);
	$x->storeNetblock($table8);
	$x->storeNetblock($table7);
	$x->storeNetblock($table6);
}
lookeq($table3, "216.240.40.5", "216.240.40.4/30");
lookeq($table3, "216.240.40.1", "216.240.40.0/27");
lookeq($table3, "216.240.40.50", "216.240.40.0/24");
lookeq($table3, "216.240.50.150", "216.240.32.0/19");
lookeq($table3, "209.157.32.32", undef);
fdel("216.240.40.1", "216.240.40.0/27", $table3);
lookeq($table3, "216.240.40.5", "216.240.40.4/30");
lookeq($table3, "216.240.40.1", "216.240.40.0/24");
lookeq($table3, "216.240.40.50", "216.240.40.0/24");
lookeq($table3, "216.240.50.150", "216.240.32.0/19");
lookeq($table3, "209.157.32.32", undef);
fdel("216.240.50.150", "216.240.32.0/19", $table3);
lookeq($table3, "216.240.40.5", "216.240.40.4/30");
lookeq($table3, "216.240.40.1", "216.240.40.0/24");
lookeq($table3, "216.240.40.50", "216.240.40.0/24");
lookeq($table3, "216.240.50.150", undef);
lookeq($table3, "209.157.32.32", undef);
fdel("216.240.40.4", "216.240.40.4/30", $table3);
lookeq($table3, "216.240.40.5", "216.240.40.0/24");
lookeq($table3, "216.240.40.1", "216.240.40.0/24");
lookeq($table3, "216.240.40.50", "216.240.40.0/24");
lookeq($table3, "216.240.50.150", undef);
lookeq($table3, "209.157.32.32", undef);
fdel("216.240.40.4", "216.240.40.0/24", $table3);
lookeq($table3, "216.240.40.5", undef);
lookeq($table3, "216.240.40.1", undef);
lookeq($table3, "216.240.40.50", undef);
lookeq($table3, "216.240.50.150", undef);
lookeq($table3, "209.157.32.32", undef);

sub lookeq
{
	my ($table, $value, $result) = @_;
	my $found = findNetblock($value, $table);
	if ($result) {
#printf "value = $value eresult = $result found = @{[$found->desc]}\n";
		print (($found->desc eq $result) ? "ok $test\n" : "not ok $test\n");
	} else {
		print ($found ? "not ok $test\n" : "ok $test\n");
	}
	$test++;
}
sub fdel
{
	my ($value, $result, $table) = @_;
	my $found = findNetblock($value, $table);
#print "search for $value, found and deleting @{[ $found->desc ]} eq $result\n";
	print (($found->desc eq $result) ? "ok $test\n" : "not ok $test\n");
	$found->deleteNetblock($table);
	$test++;
}

my (@c) = range2cidrlist("66.33.85.239", "66.33.85.240");
my $dl = dlist(@c);
print ($dl eq '66.33.85.239/32 66.33.85.240/32' ? "ok $test\n" : "not ok $test\n"); $test++;

(@c) = range2cidrlist('216.240.32.128', '216.240.36.127');
$dl = dlist(@c);
print ($dl eq '216.240.32.128/25 216.240.33.0/24 216.240.34.0/23 216.240.36.0/25' ? "ok $test\n" : "not ok $test\n"); $test++;

my @d;
@d = (@c[0,1,3]);

my (@e) = cidrs2contiglists(@d);

print (@e == 2 ? "ok $test\n" : "not ok $test\n"); $test++;

print (dlist(@{$e[0]}) eq '216.240.32.128/25 216.240.33.0/24' ? "ok $test\n" : "not ok $test\n"); $test++;
print (dlist(@{$e[1]}) eq '216.240.36.0/25' ? "ok $test\n" : "not ok $test\n"); $test++;

sub dlist 
{
	my (@b) = @_;
	return join (' ', map { $_->desc() } @b);
}

sub generate {
	my $count = shift || 10000;
	my @list;
	$list[$count-1]='';  ## preallocate
	for (my $i=0; $i<$count; $i++) {
		my $class = int(rand(3));
		if ($class == 0) {
			## class A ( 1.0.0.0 - 126.255.255.255 )
			$list[$i] = int(rand(126))+1;
		} elsif ($class == 1) {
			## class B ( 128.0.0.0 - 191.255.255.255 )
			$list[$i] = int(rand(64))+128;
		} else {
			## class C ( 192.0.0.0 - 223.255.255.255 )
			$list[$i] = int(rand(32))+192;
		}
		$list[$i] .= '.' . int(rand(256));
		$list[$i] .= '.' . int(rand(256));
		$list[$i] .= '.' . int(rand(256));
	}
	return @list;
}

sub by_net_netmask_block2
{
	$a->{'IBASE'} <=> $b->{'IBASE'}
		|| $a->{'BITS'} <=> $b->{'BITS'};
}

my (@iplist) = generate(500);

my (@sorted1) = sort_by_ip_address(@iplist);

my (@blist) = map { new Net::Netmask $_ } @iplist;
my (@clist) = sort @blist;
my (@sorted2) = map { $_->base() } @clist;

if ($] < 5.006_001) {
	print "ok $test # skip Overload not supported at $[\n"; $test++;
} else {
	print ("@sorted1" eq "@sorted2" ? "ok $test\n" : "not ok $test\n"); $test++;
}

my (@dlist) = sort @blist;
my (@sorted3) = map { $_->base() } @dlist;

if ($] < 5.006_001) {
	print "ok $test # skip Overload not supported at $[\n"; $test++;
} else {
	print "# AT TEST $test\n";
	print ("@sorted1" eq "@sorted3" ? "ok $test\n" : "not ok $test\n"); $test++;
}

my $q144 = new Net::Netmask '216.240.32.0/25';

for my $i (qw(216.240.32.0/24 216.240.32.0/26 216.240.33.0/25)) {
	my $q144p = new Net::Netmask $i;
	print ($q144 eq $q144p ? "not ok $test\n" : "ok $test\n"); $test++;
	print ($q144 == $q144p ? "not ok $test\n" : "ok $test\n"); $test++;
	print (sameblock($q144, $i) ? "not ok $test\n" : "ok $test\n"); $test++;
	print (cmpblocks($q144, $i) ? "ok $test\n" : "not ok $test\n"); $test++;
	print ($q144->sameblock($i) ? "not ok $test\n" : "ok $test\n"); $test++;
	print ($q144->cmpblocks($i) ? "ok $test\n" : "not ok $test\n"); $test++;
}

my $q144pp = new Net::Netmask '216.240.32.0/25'; 
print ($q144 == $q144pp ? "ok $test\n" : "not ok $test\n"); $test++;
print ($q144 eq $q144pp ? "ok $test\n" : "not ok $test\n"); $test++;
print (sameblock($q144, '216.240.32.0/25') ? "ok $test\n" : "not ok $test\n"); $test++;
print (cmpblocks($q144, '216.240.32.0/25') ? "not ok $test\n" : "ok $test\n"); $test++;
print ($q144->sameblock('216.240.32.0/25') ? "ok $test\n" : "not ok $test\n"); $test++;
print ($q144->cmpblocks('216.240.32.0/25') ? "not ok $test\n" : "ok $test\n"); $test++;

print ($q144->desc eq "$q144" ? "ok $test\n" : "not ok $test\n"); $test++;

my $dnts = join(' ',dumpNetworkTable($table9));
print ($dnts eq '0.0.0.0/0 209.157.64.0/19 209.157.80.0/24 209.157.81.16/28' ? "ok $test\n" : "not ok $test\n"); $test++;

sub lookouter
{
	my ($table, $value, $result) = @_;
	my $found = findOuterNetblock($value, $table);
	if ($result) {
#printf "value = $value eresult = $result found = @{[$found->desc]}\n";
		print (($found->desc eq $result) ? "ok $test\n" : "not ok $test\n");
	} else {
		print ($found ? "not ok $test\n" : "ok $test\n");
	}
	$test++;
}

# 216.240.32.0/19
# 216.240.40.0/24
# 216.240.40.0/27
# 216.240.40.4/30

lookouter($table8, "216.240.40.5", "216.240.32.0/19");
lookouter($table8, "216.240.40.1", "216.240.32.0/19");
lookouter($table8, "216.240.40.50", "216.240.32.0/19");
lookouter($table8, "216.240.50.150", "216.240.32.0/19");
lookouter($table8, "209.157.32.32", undef);
fdel("216.240.32.10", "216.240.32.0/19", $table8);
lookouter($table8, "216.240.40.5", "216.240.40.0/24");
lookouter($table8, "216.240.40.1", "216.240.40.0/24");
lookouter($table8, "216.240.40.50", "216.240.40.0/24");
lookouter($table8, "216.240.50.150", undef);
lookouter($table8, "209.157.32.32", undef);
fdel("216.240.40.150", "216.240.40.0/24", $table8);
lookouter($table8, "216.240.40.5", "216.240.40.0/27");
lookouter($table8, "216.240.40.1", "216.240.40.0/27");
lookouter($table8, "216.240.40.50", undef);
lookouter($table8, "216.240.50.150", undef);
lookouter($table8, "209.157.32.32", undef);
fdel("216.240.40.3", "216.240.40.0/27", $table8);
lookouter($table8, "216.240.40.5", "216.240.40.4/30");
lookouter($table8, "216.240.40.1", undef);
lookouter($table8, "216.240.40.50", undef);
lookouter($table8, "216.240.50.150", undef);
lookouter($table8, "209.157.32.32", undef);
fdel("216.240.40.4", "216.240.40.4/30", $table8);
lookouter($table8, "216.240.40.5", undef);
lookouter($table8, "216.240.40.1", undef);
lookouter($table8, "216.240.40.50", undef);
lookouter($table8, "216.240.50.150", undef);
lookouter($table8, "209.157.32.32", undef);


sub lookouterO
{
	my ($table, $value, $result) = @_;
	my $block = new2 Net::Netmask $value;
	my $found = findOuterNetblock($block, $table);
	if ($result) {
#printf "value = $value eresult = $result found = @{[$found->desc]}\n";
		print (($found->desc eq $result) ? "ok $test\n" : "not ok $test\n");
	} else {
		print ($found ? "not ok $test\n" : "ok $test\n");
	}
	$test++;
}

lookouterO($table7, "216.240.40.5/30", "216.240.32.0/19");
lookouterO($table7, "216.240.40.5/29", "216.240.32.0/19");
lookouterO($table7, "216.240.40.50/24", "216.240.32.0/19");
lookouterO($table7, "216.240.50.150/23", "216.240.32.0/19");
lookouterO($table7, "209.157.32.32", undef);
fdel("216.240.32.10", "216.240.32.0/19", $table7);
lookouterO($table7, "216.240.40.5/30", "216.240.40.0/24");
lookouterO($table7, "216.240.40.5/29", "216.240.40.0/24");
lookouterO($table7, "216.240.40.50/24", "216.240.40.0/24");
lookouterO($table7, "216.240.50.150/23", undef);
lookouterO($table7, "209.157.32.32", undef);
fdel("216.240.40.150", "216.240.40.0/24", $table7);
lookouterO($table7, "216.240.40.5/30", "216.240.40.0/27");
lookouterO($table7, "216.240.40.5/29", "216.240.40.0/27");
lookouterO($table7, "216.240.40.50/24", undef);
lookouterO($table7, "216.240.50.150/23", undef);
lookouterO($table7, "209.157.32.32", undef);
fdel("216.240.40.3", "216.240.40.0/27", $table7);
lookouterO($table7, "216.240.40.5/30", "216.240.40.4/30");
lookouterO($table7, "216.240.40.5/29", undef);
lookouterO($table7, "216.240.40.50/24", undef);
lookouterO($table7, "216.240.50.150/23", undef);
lookouterO($table7, "209.157.32.32", undef);
fdel("216.240.40.4", "216.240.40.4/30", $table7);
lookouterO($table7, "216.240.40.5/30", undef);
lookouterO($table7, "216.240.40.1/29", undef);
lookouterO($table7, "216.240.40.50/24", undef);
lookouterO($table7, "216.240.50.150/23", undef);
lookouterO($table7, "209.157.32.32/8", undef);

sub ctest
{
	my $a = new Net::Netmask shift;
	my $b = new Net::Netmask shift;

	print "# ctest($a, $b)\n";
	print ($a->contains($a) ? "ok $test\n" : "not ok $test\n"); $test++;
	print ($b->contains($b) ? "ok $test\n" : "not ok $test\n"); $test++;
	print ($a->contains($b) ? "ok $test\n" : "not ok $test\n"); $test++;
	print (($a->sameblock($b) || ! $b->contains($a)) ? "ok $test\n" : "not ok $test\n"); $test++;
}

sub ctestno
{
	my $a = new Net::Netmask shift;
	my $b = new Net::Netmask shift;

	print "# ctestno($a, $b)\n";
	print ($a->contains($b) ? "not ok $test\n" : "ok $test\n"); $test++;
	print ($b->contains($a) ? "not ok $test\n" : "ok $test\n"); $test++;
}

ctest("10.20.30.0/24", "10.20.30.0/25");
ctest("10.20.30.0/23", "10.20.30.0/24");
ctest("10.20.30.0/24", "10.20.30.128/25");
ctest("0.0.0.0/8", "0.255.255.255/32");
ctest("255.255.255.255/32", "255.255.255.255/32");
ctest("255.255.255.0/24", "255.255.255.255/32");

ctest("66.106.19.144/28", "66.106.19.152/29");
ctest("66.106.19.144/28", "66.106.19.144/29");

ctestno("66.106.19.144/28", "66.106.19.168/29");
ctestno("66.106.19.144/28", "198.175.15.10/29");
ctestno("66.106.19.144/28", "66.106.19.160/29");

sub multinew
{
	return map { new Net::Netmask $_ } @_;
}

(@c) = cidrs2cidrs(multinew(qw(216.240.32.0/25 216.240.32.128/25 216.240.33.0/25 216.240.34.0/24)));
$dl = dlist(@c);
print ($dl eq '216.240.32.0/24 216.240.33.0/25 216.240.34.0/24' ? "ok $test\n" : "not ok $test\n"); $test++;

(@c) = cidrs2cidrs(multinew(qw(216.240.32.0/32 216.240.32.1/32 216.240.32.2/32 216.240.32.3/32 216.240.32.4/32)));
$dl = dlist(@c);
print ($dl eq '216.240.32.0/30 216.240.32.4/32' ? "ok $test\n" : "not ok $test\n"); $test++;


(@c) = cidrs2cidrs(multinew(qw(216.240.32.64/28 216.240.32.0/25 216.240.32.128/25 216.240.33.0/25 216.240.34.0/24)));
$dl = dlist(@c);
print ($dl eq '216.240.32.0/24 216.240.33.0/25 216.240.34.0/24' ? "ok $test\n" : "not ok $test\n"); $test++;


my $block = new Net::Netmask ('172.2.4.0', '255.255.255.0');
$table = {};
$block->storeNetblock($table);
@b = findAllNetblock('172.2.4.1', $table);
print $#b == 0 ? "ok $test\n" : "not ok $test\n"; $test++;

$block->tag('a', 'b');
$block->tag('b', 'c');
$block->tag('c', 'x');
$block->tag('c', undef);
$block->tag('d', 'x');
$block->tag('d');

print $block->tag('a') eq 'b' ? "ok $test\n" : "not ok $test\n"; $test++;
print $block->tag('b') eq 'c' ? "ok $test\n" : "not ok $test\n"; $test++;
print(!defined($block->tag('c')) ? "ok $test\n" : "not ok $test\n"); $test++;
print $block->tag('d') eq 'x' ? "ok $test\n" : "not ok $test\n"; $test++;
print $block->tag('a') eq 'b' ? "ok $test\n" : "not ok $test\n"; $test++;

(@c) = cidrs2inverse('216.240.32.0/22', (multinew(qw(216.240.32.64/28 216.240.32.0/25 216.240.32.128/25 216.240.33.0/25 216.240.34.0/24))));
$dl = dlist(@c);
print ($dl eq '216.240.33.128/25 216.240.35.0/24' ? "ok $test\n" : "not ok $test\n"); $test++;

(@c) = cidrs2inverse('216.240.32.0/22', (multinew(qw(215.0.0.0/16 216.240.32.64/28 216.240.32.0/25 216.240.32.128/25 216.240.33.0/25 216.240.34.0/24 216.240.45.0/24))));
$dl = dlist(@c);
print ($dl eq '216.240.33.128/25 216.240.35.0/24' ? "ok $test\n" : "not ok $test\n"); $test++;

(@c) = cidrs2inverse('216.240.32.0/22', (multinew(qw(216.240.0.0/16 215.0.0.0/16 216.240.32.64/28 216.240.32.0/25 216.240.32.128/25 216.240.33.0/25 216.240.34.0/24 216.240.45.0/24))));
$dl = dlist(@c);
print ($dl eq '' ? "ok $test\n" : "not ok $test\n"); $test++;


my $table77 = {};
my $block77 = new2 Net::Netmask("10.1.2.0/24", $table77);
$block77->storeNetblock();
if(defined(findNetblock("10.2.1.0", $table77))) {
  print "not ok $test\n";
} else {
  print "ok $test\n";
}
$test++;


{
	my $b = new Net::Netmask("192.168.0.0/23");
	my @t = (
		undef	,  '192.168.2.0/23',   # => would turn undef into "undef"
		10	=> '192.168.20.0/23',
		7	=> '192.168.14.0/23',
		-1	=> '192.167.254.0/23',
	);
	while (@t) {
		my $arg = shift(@t);
		my $result = shift(@t);
		if ($b->nextblock($arg)."" eq $result) {
			print "ok $test\n";
		} else {
			print "not ok $test\n";
		}
		$test++;
	}
}


{
	my $obj1 = new2 Net::Netmask ('1.0.0.4/31');
	my $obj2 = new2 Net::Netmask ('1.0.0.4/32');
	my @leftover = cidrs2inverse($obj1, $obj2);
	# print "leftover = @leftover\n";
	if (@leftover == 1 && "$leftover[0]" eq "1.0.0.5/32") {
		print "ok $test\n";
	} else {
		print "not ok $test\n";
	}
	$test++;
}

{
	my $obj1 = new2 Net::Netmask ('1.0.0.4/32');
	my $obj2 = new2 Net::Netmask ('1.0.0.0/8');
	my @leftover = cidrs2inverse($obj1, $obj2);
	if (@leftover) {
		print "not ok $test # leftover = @leftover\n";
	} else {
		print "ok $test\n";
	}
	$test++;
}

{
	my $obj1 = new2 Net::Netmask ('1.0.0.4/32');
	my $obj2 = new2 Net::Netmask ('1.0.0.4/32');
	my @leftover = cidrs2inverse($obj1, $obj2);
	if (@leftover) {
		print "not ok $test # leftover2 = @leftover\n";
	} else {
		print "ok $test\n";
	}
	$test++;
}

{
	my $obj1 = new2 Net::Netmask ('1.0.0.4/32');
	my $obj2 = new2 Net::Netmask ('1.0.0.6/32');
	my @leftover = cidrs2inverse($obj1, $obj2);
	if (@leftover == 1 && "$leftover[0]" eq '1.0.0.4/32') {
		print "ok $test\n";
	} else {
		print "not ok $test # leftover3 = @leftover\n";
	}
	$test++;
}

{
	my $obj1 = new2 Net::Netmask ('1.0.0.4/31');
	my $obj2 = new2 Net::Netmask ('1.0.0.5/32');
	my @leftover = cidrs2inverse($obj1, $obj2);
	if (@leftover == 1 && "$leftover[0]" eq '1.0.0.4/32') {
		print "ok $test\n";
	} else {
		print "not ok $test # leftover3 = @leftover\n";
	}
	$test++;
}

{
	my $obj1 = new2 Net::Netmask ('1.0.0.4/31');
	my $obj2 = new2 Net::Netmask ('1.0.0.4/32');
	my @leftover = cidrs2inverse($obj1, $obj2);
	if (@leftover == 1 && "$leftover[0]" eq '1.0.0.5/32') {
		print "ok $test\n";
	} else {
		print "not ok $test # leftover3 = @leftover\n";
	}
	$test++;
}
