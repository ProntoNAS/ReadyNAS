#!perl
# vim:ts=4:sw=4:et

use strict;
use warnings;
use Test::More;
use File::Temp qw(tempfile tempdir); # in core since perl 5.6.1
use File::Path qw(make_path); # in core since Perl 5.001
use File::Basename; # in core since Perl 5
use FindBin; # in core since Perl 5.00307
use Linux::Clone; # neither in core nor in Debian :-/

# ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
# ┃ SETUP: in a new mount namespace, bindmount tmpdirs on /etc/systemd and    ┃
# ┃ /var/lib/systemd to start with clean directories yet use the actual       ┃
# ┃ locations and code paths.                                                 ┃
# ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

my $dsh = "$FindBin::Bin/../script/deb-systemd-helper";

sub _unit_check {
    my ($unit_file, $cmd, $cb, $verb) = @_;

    my $retval = system("DPKG_MAINTSCRIPT_PACKAGE=test $dsh $cmd $unit_file");
    isnt($retval, -1, 'deb-systemd-helper could be executed');
    ok(!($retval & 127), 'deb-systemd-helper did not exit due to a signal');
    $cb->($retval >> 8, 0, "random unit file $verb $cmd");
}

sub is_enabled { _unit_check($_[0], 'is-enabled', \&is, 'is') }
sub isnt_enabled { _unit_check($_[0], 'is-enabled', \&isnt, 'isnt') }

sub is_debian_installed { _unit_check($_[0], 'debian-installed', \&is, 'is') }
sub isnt_debian_installed { _unit_check($_[0], 'debian-installed', \&isnt, 'isnt') }

my $retval = Linux::Clone::unshare Linux::Clone::NEWNS;
BAIL_OUT("Cannot unshare(NEWNS): $!") if $retval != 0;

sub bind_mount_tmp {
    my ($dir) = @_;
    my $tmp = tempdir(CLEANUP => 1);
    system("mount -n --bind $tmp $dir") == 0
        or BAIL_OUT("bind-mounting $tmp to $dir failed: $!");
    return $tmp;
}

my $etc_systemd = bind_mount_tmp('/etc/systemd');
my $lib_systemd = bind_mount_tmp('/lib/systemd');
my $var_lib_systemd = bind_mount_tmp('/var/lib/systemd');

# ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
# ┃ Verify “is-enabled” is not true for a random, non-existing unit file.     ┃
# ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

my ($fh, $random_unit) = tempfile('unitXXXXX',
    SUFFIX => '.service',
    TMPDIR => 1,
    UNLINK => 1);
close($fh);
$random_unit = basename($random_unit);

isnt_enabled($random_unit);
isnt_debian_installed($random_unit);

# ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
# ┃ Verify “is-enabled” is not true for a random, existing unit file.         ┃
# ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

my $servicefile_path = "/lib/systemd/system/$random_unit";
make_path('/lib/systemd/system');
open($fh, '>', $servicefile_path);
print $fh <<'EOT';
[Unit]
Description=test unit

[Service]
ExecStart=/bin/sleep 1

[Install]
WantedBy=multi-user.target
EOT
close($fh);

isnt_enabled($random_unit);
isnt_debian_installed($random_unit);

# ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
# ┃ Verify “enable” creates the requested symlinks.                           ┃
# ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

ok(! -d '/etc/systemd/system/multi-user.target.wants',
    'multi-user.target.wants does not exist yet');

$retval = system("DPKG_MAINTSCRIPT_PACKAGE=test $dsh enable $random_unit");
my $symlink_path = "/etc/systemd/system/multi-user.target.wants/$random_unit";
ok(-l $symlink_path, "$random_unit was enabled");
is(readlink($symlink_path), $servicefile_path,
    "symlink points to $servicefile_path");

# ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
# ┃ Verify “is-enabled” now returns true.                                     ┃
# ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

is_enabled($random_unit);
is_debian_installed($random_unit);

# ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
# ┃ Verify deleting the symlinks and running “enable” again does not          ┃
# ┃ re-create the symlinks.                                                   ┃
# ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

unlink($symlink_path);
ok(! -l $symlink_path, 'symlink deleted');
isnt_enabled($random_unit);
is_debian_installed($random_unit);

$retval = system("DPKG_MAINTSCRIPT_PACKAGE=test $dsh enable $random_unit");

isnt_enabled($random_unit);

# ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
# ┃ Verify “disable” when purging deletes the statefile.                      ┃
# ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

my $statefile = "/var/lib/systemd/deb-systemd-helper-enabled/$random_unit.dsh-also";

ok(-f $statefile, 'state file exists');

$retval = system("DPKG_MAINTSCRIPT_PACKAGE=test _DEB_SYSTEMD_HELPER_PURGE=1 $dsh disable $random_unit");

ok(! -f $statefile, 'state file does not exist anymore after purging');
isnt_debian_installed($random_unit);

# ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
# ┃ Verify “enable” after purging does re-create the symlinks.                ┃
# ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

ok(! -l $symlink_path, 'symlink does not exist yet');
isnt_enabled($random_unit);

$retval = system("DPKG_MAINTSCRIPT_PACKAGE=test $dsh enable $random_unit");

is_enabled($random_unit);
is_debian_installed($random_unit);

# ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
# ┃ Verify “disable” removes the symlinks.                                    ┃
# ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

$retval = system("DPKG_MAINTSCRIPT_PACKAGE=test _DEB_SYSTEMD_HELPER_PURGE=1 $dsh disable $random_unit");

isnt_enabled($random_unit);

# ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
# ┃ Verify “enable” after purging does re-create the symlinks.                ┃
# ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

ok(! -l $symlink_path, 'symlink does not exist yet');
isnt_enabled($random_unit);

$retval = system("DPKG_MAINTSCRIPT_PACKAGE=test $dsh enable $random_unit");

is_enabled($random_unit);
is_debian_installed($random_unit);

# ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
# ┃ Verify “mask” (when enabled) results in the symlink pointing to /dev/null ┃
# ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

my $mask_path = "/etc/systemd/system/$random_unit";
ok(! -l $mask_path, 'mask link does not exist yet');

$retval = system("DPKG_MAINTSCRIPT_PACKAGE=test $dsh mask $random_unit");
ok(-l $mask_path, 'mask link exists');
is(readlink($mask_path), '/dev/null', 'service masked');

$retval = system("DPKG_MAINTSCRIPT_PACKAGE=test $dsh unmask $random_unit");
ok(! -e $mask_path, 'mask link does not exist anymore');

# ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
# ┃ Verify “mask” (when disabled) works the same way                          ┃
# ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

$retval = system("DPKG_MAINTSCRIPT_PACKAGE=test $dsh disable $random_unit");
ok(! -e $symlink_path, 'symlink no longer exists');

$retval = system("DPKG_MAINTSCRIPT_PACKAGE=test $dsh mask $random_unit");
ok(-l $mask_path, 'mask link exists');
is(readlink($mask_path), '/dev/null', 'service masked');

$retval = system("DPKG_MAINTSCRIPT_PACKAGE=test $dsh unmask $random_unit");
ok(! -e $mask_path, 'symlink no longer exists');

# ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
# ┃ Verify “mask”/unmask don’t do anything when the user already masked.      ┃
# ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

ok(! -l $mask_path, 'mask link does not exist yet');
symlink('/dev/null', $mask_path);
ok(-l $mask_path, 'mask link exists');
is(readlink($mask_path), '/dev/null', 'service masked');

$retval = system("DPKG_MAINTSCRIPT_PACKAGE=test $dsh mask $random_unit");
ok(-l $mask_path, 'mask link exists');
is(readlink($mask_path), '/dev/null', 'service still masked');

$retval = system("DPKG_MAINTSCRIPT_PACKAGE=test $dsh unmask $random_unit");
ok(-l $mask_path, 'mask link exists');
is(readlink($mask_path), '/dev/null', 'service still masked');

done_testing;
