#!/usr/bin/perl -w

BEGIN { unshift @INC, '.' ;
        $SIG{__WARN__}=sub {return "" if $_[0] =~ /used only once/; print @_;};
}

use Devel::Symdump::Export qw(filehandles hashes arrays);
use Test::More;

plan tests => 13;

init();

my %prefices = qw(
		  scalars	$
		  arrays	@
		  hashes	%
		  functions 	&
		  unknowns 	*
		 );

@prefices{qw(filehandles dirhandles packages)}=("") x 3;


format i_am_the_symbol_printing_format_lest_there_be_any_doubt =
Got these @*
  "$t:"
~~ ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  $a

.

$~ = 'i_am_the_symbol_printing_format_lest_there_be_any_doubt';

@a = packsort(filehandles('main'));
$t = 'filehandles';
$a = "@a";
# write;
ok (
    $a eq "main::DATA main::Hmmmm main::STDERR main::STDIN main::STDOUT main::stderr main::stdin main::stdout"
    ||
    $a eq "main::ARGV main::DATA main::Hmmmm main::STDERR main::STDIN main::STDOUT main::i_am_the_symbol_printing_format_lest_there_be_any_doubt main::stderr main::stdin main::stdout",
    $a
   );

@a = packsort(hashes 'main');
$t = 'hashes';
$a = uncontrol("@a");
$a =~ s/main:://g;
#write;
ok (
    $a eq "^H + - @ ENV INC SIG" # + named capture 29682
    ||
    $a eq "^H + @ ENV INC SIG"   # + named capture 28957
    ||
    $a eq "^H @ ENV INC SIG"     # ^H hints 27643 (?)
    ||
    $a eq "@ ENV INC SIG"
    ||
    $a eq "ENV INC SIG",
    $a
   );

@a = packsort(arrays());
$t = 'arrays';
$a = "@a";
#write;
like (
      $a, "/main::INC.*main::_.*main::a/"
     );

eval {
    @a = Devel::Symdump->really_bogus('main');
};
$a = $@ ? $@ : "@a";
like ($a,
      "/^invalid Devel::Symdump method: really_bogus\(\)/",
     );

$sob = rnew Devel::Symdump;

@m=();
for (active_packages($sob)) {
    push @m, "$_";
}
$a="@m";
like ($a,
      "/Carp.*Devel.*Devel::Symdump.*Devel::Symdump::Export.*DynaLoader.*Exporter.*Hidden.*big::long::hairy.*funny::little.*strict/");

my %m=();
for (active_modules($sob)) {
    $m{$_}=undef;
}
$a = join " ", keys %m;
#print "[$a]\n";
ok (exists $m{"Carp"} &&
    exists $m{"Devel::Symdump"} &&
    exists $m{"Devel::Symdump::Export"} &&
    exists $m{"Exporter"} &&
    exists $m{"strict"} &&
    exists $m{"vars"});

# Cannot test on the number of packages and functions because not
# every perl is built the same way. Static perls will reveal more
# packages and more functions being in them
# Testing on >= seems no problem to me, we'll see

# (Time passes) Much less unknowns in version 1.22 (perl5.003_10).

my %Expect=qw(
packages 13 scalars 28 arrays 7 hashes 5 functions 35 filehandles 9
dirhandles 2 unknowns 53
);

#we don't count the unknowns. Newer perls might have different outcomes
for $type ( qw{
	       packages
	       scalars arrays hashes
	       functions filehandles dirhandles
	     }){
    next unless @syms = $sob->$type();

    if ($I_REALLY_WANT_A_CORE_DUMP) {
	# if this block execute , mysteriously COREDUMPS at for() below
	# NOT TRUE anymore (watched by Andreas, 15.6.1995)
	@vars = ($type eq 'packages') ? sort(@syms) : packsort(@syms);
    } else {
	if ($type eq 'packages') {
	    @syms = sort @syms;
	} else {
	    @syms = packsort(@syms);
	}
    }

    ok (@syms >= $Expect{$type});
}

exit;

sub active_modules {
    my $ob = shift;
    my @modules = ();
    my($pack);
    for $pack ("main", $ob->packages) {
	if (
		defined &{ "$pack\::import"   } 	||
		defined &{ "$pack\::AUTOLOAD" } 	||
		defined @{ "$pack\::ISA"      }	||
		defined @{ "$pack\::EXPORT"   }	||
		defined @{ "$pack\::EXPORT_OK"}
	    )
	{
	    push @modules, $pack;
	}
    }
    return sort @modules;
}

sub active_packages {
    my $ob = shift;

    my @modules = ();
    my $pack;
    for $pack ($ob->packages) {
	$pob = new Devel::Symdump $pack;
	if ( $pob->scalars()	||
	     $pob->hashes()	||
	     $pob->arrays()	||
	     $pob->functions()	||
	     $pob->filehandles()||
	     $pob->dirhandles()
	   )
	{
	    push @modules, $pack;
	}
    }
    return sort @modules;
}


sub uncontrol {
    local $_  = $_[0];
    s/([\200-\377])/    'M-' . pack('c', ord($1) & 0177 )  /eg;
    s/([\000-\037\177])/ '^' . pack('c', ord($1) ^  64   ) /eg;
    return $_;
}

sub packsort {
    my (@vars, @pax, @fullnames);

    for (@_) {
        my($pack, $name) = /^(.*::)(.*)$/s;
        push(@vars, $name);
        push(@pax, $pack);
        push(@fullnames, $_);
    }

    return @fullnames [
		sort {
                    ($pax[$a] ne 'main::') <=> ($pax[$b] ne 'main::')
			||
                    $pax[$a] cmp $pax[$b]
                        ||
                    $vars[$a] cmp $vars[$b]
                } 0 .. $#fullnames
             ];
}


sub init {
    $big::long::hairy::thing++;
    sub Devel::testsub {};
    opendir(DOT, '.');
    opendir(funny::little::imadir, '/');
    $i_am_a_scalar_variable = 1;
    open(Hmmmm, ">/dev/null");
    open(Hidden::FH, ">/dev/null");
}


__END__
