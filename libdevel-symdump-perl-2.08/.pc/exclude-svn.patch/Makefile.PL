use lib "./lib";
use Devel::Symdump ();
use ExtUtils::MakeMaker qw(:DEFAULT);

eval { require File::Spec; };
my $HAVE_FILE_SPEC = !$@;
use strict;
use vars qw($VERSION);

my $Id = q$Id: Makefile.PL 146 2005-08-09 04:25:21Z k $;
$VERSION = sprintf "%.3f", 1 + substr(q$Rev: 146 $,4)/1000;

my $version_diff = 0; # we'll have to die if this becomes true
{
  local $^W;
  $ExtUtils::MakeMaker::VERSION = eval $ExtUtils::MakeMaker::VERSION;
}
if ($HAVE_FILE_SPEC) {
  my $version_from = File::Spec->catfile(qw(lib Devel Symdump.pm));
  my $version_set_manually = 1; # not by SVN

  if ($ARGV[0] && $ARGV[0] eq "--setversion") {
    die "Your perl is a bit dated[$]].\nDo not make a release with it\n" if $] < 5.008;
    die "Your MakeMaker is a bit dated[$]].\nDo not make a release with it\n"
        if $ExtUtils::MakeMaker::VERSION < 6.3004;
    die "Your MakeMaker doesn't do the sign woodoo" unless
        MM->can("signature_target");
    shift @ARGV;
    my $st;
    local $ENV{LANG} = "C";
    open $st, "svn st -u|" or die;
    my $dirty = 0;
    while (<$st>) {
      next if /^\?/;
      next if /^Head revision:/;
      next if /^Status against revision:/;
      print;
      $dirty++;
    }
    close $st;
    die "Not everything checked in?" if $dirty;

    # $re parses the upper boundary of the output of svnversion
    my $re;
    eval q{ $re = qr/(?:\d+:)?(\d+)[SM]*$/; };
    die $@ if $@; # old perl
    my($sv_dot)  = `svnversion .`             =~ $re;
    my($sv_from) = `svn st -u -v $version_from`  =~ /^.{8}\s*(?:\d+)\s*(\d+)/;
    die "Could not determine sv_from or sv_dot"
        unless $sv_from > 0 && $sv_dot > 0;
    if ($version_set_manually) {
      unshift @INC, "lib";
      require $version_from;
      open my $fh, "make the-release-name|" or die;
      my $have_version;
      while (<$fh>) {
        next unless /^version\s+([\d\._]+)/;
        $have_version = eval $1;
      }
      die "could not determine current version from Makefile" unless $have_version;
      eval q{
      no warnings "numeric";
      if ($Devel::Symdump::VERSION != $have_version) {
        warn "Not equal: D:S:VERSION[$Devel::Symdump::VERSION] Makefile version[$have_version]";
        $version_diff = 1;
      }
};
      die $@ if $@;
    } else {
      if ($sv_dot > $sv_from) {
        warn "setting some property to make a checkin of this file possible";
        0==system(svn => "propset",
                  "cpan:release",
                  "after_$sv_dot",
                  $version_from) or die;
        warn "checking in to get the version up to the bleading edge";
        0==system(svn => "ci",
                  "-m",
                  "Property set for release after_$sv_dot",
                  $version_from) or die;
        # will have to die after Makefile is rewritten:
        $version_diff = 1;
      }
    }
    exit unless $version_diff;
  }
}

my @sign = (MM->can("signature_target") ? (SIGN => 1) : ());
WriteMakefile(
	      NAME => "Devel::Symdump",
	      DISTNAME => "Devel-Symdump",
	      VERSION_FROM => "lib/Devel/Symdump.pm",
              ($ExtUtils::MakeMaker::VERSION >= 6.3001 ?
               (LICENSE      => "perl") : (),
              ),
              PREREQ_PM => {
                            'Test::More' => 0,
                           },
	      'clean' => {FILES => '*/*/*~'},
              'dist' => {
                         DIST_DEFAULT => 'Makefile setversion README all chlog tardist',
                         COMPRESS => 'gzip -9f'
                        },
              @sign,
	     );

if ($version_diff){
  die "
==> I had to update some \$VERSIONs <==
==> Your Makefile has been rebuilt. <==
==> Please rerun the make command.  <==
";
}

sub MY::postamble {
  q{
setversion:
	$(PERL) Makefile.PL --setversion

README: lib/Devel/Symdump.pm Makefile
	chmod +w $@
	/usr/bin/pod2text lib/Devel/Symdump.pm > $@

chlog ::
	LC_ALL=en_GB.utf8 svn log -v -r `svnversion . | perl -ne '/(?:\d+:)?(\d+)[SM]*$$/&&print $$1'`:1 > ChangeLog.svn
	cat ChangeLog.local >> ChangeLog.svn

the-release-name :
	$(NOECHO) $(ECHO) 'version ' $(VERSION)
	$(NOECHO) $(ECHO) 'release-name ' $(DISTVNAME).tar$(SUFFIX)

release ::
	/usr/bin/perl -I../../CPAN/SVN/release-lib -S release $(DISTVNAME).tar$(SUFFIX)

sign:
	cpansign -s

howto-release:
	@$(ECHO) manually set version in Symdump.pm, edit ChangeLog
	@$(ECHO) make ci dist \&\& make release
}
}

sub MY::dist_ci {
  return qq{ci :
	svn ci
};
}

sub MY::dist_test {
  return q{
# if we depend on $(DISTVNAME).tar$(SUFFIX), then the rest of the
# Makefile breaks our intent to NOT remake dist
disttest :
	rm -rf $(DISTVNAME)
	tar xvzf $(DISTVNAME).tar$(SUFFIX)
	cd $(DISTVNAME) && $(ABSPERLRUN) Makefile.PL
	cd $(DISTVNAME) && $(MAKE) $(PASTHRU)
	cd $(DISTVNAME) && $(MAKE) test $(PASTHRU)
}
}
