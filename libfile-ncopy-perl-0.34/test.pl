# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

#########################

# change 'tests => 1' to 'tests => last_test_to_print';

use Test;
use File::Spec;
BEGIN { plan tests => 4 };
use File::NCopy;
ok(1); # Loaded

# New object
my $test = File::NCopy->new(test => 1);
ok($test);

# Need this later
my $dirsep = File::Spec->catfile('a','b');
$dirsep =~ s!a(.+)b$!$1!;
$rdirsep = ($dirsep eq '\\' ? '\\\\' : $dirsep );

# Test Defaults
ok($test->{recursive} == 0 && $test->{preserve} == 0 && $test->{follow_links} == 0 && $test->{force_write} == 0);

$tmp_dir = File::Spec->tmpdir();
$path = File::Spec->catfile($tmp_dir,'test_ncpy_inst');
mkdir $path unless (-e $path);
$test->{recursive} = 1;
my @files = $test->copy($tmp_dir,$path);
if ((scalar(@files) == 0)) {
    # Skip, no files to test with
    skip(1,0);
} else {
    my $done = 0;
    foreach my $path (@files) {
		# Remove a leading one, if it has it
		if (index($path,$dirsep) == 0) {
			$path = substr($path,(length($path) - length($path) - 1),(length($path) - 1));
		}
        my $parts = scalar(split(/$rdirsep/,$path));
        if ($parts > 0) {
            # it should contain a seperator
            $done = 1;
            if (index($path,$dirsep)) {
                # it has some in it.
                ok(1);
                $done = 1;
            } else {
                # this is bad.
				warn "Path '$path' ($parts parts) did not contain a seperator\n";
                ok(0);
                $done = 1;
            }
        } else {
            # no seperator, try next one;
            next;
        }
        if ($done) {
            last;
        }
    }
    if (! $done) {
        ok(0);
    }
}




