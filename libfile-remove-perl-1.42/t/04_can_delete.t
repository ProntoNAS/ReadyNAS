#!/usr/bin/perl

# Test that File::Remove can recursively remove a directory that
# deeply contains a readonly file that is owned by the current user.
use strict;
BEGIN {
	$|  = 1;
	$^W = 1;
}

use Test::More qw(no_plan);
use File::Spec::Functions ':ALL';
use File::Copy   ();
use File::Remove ();





#####################################################################
# Set up for the test

my $in = catdir( curdir(), 't' );
ok( -d $in, 'Found t dir' );
my $d1 = catdir( $in, 'd1' );
my $d2 = catdir( $d1, 'd2' );
my $f3 = catfile( $d2, 'f3.txt' );

sub create_directory {
	mkdir $d1 or die "Failed to create $d1";
	ok( -d $d1, "Created $d1 ok" );
	ok( -r $d1, "Created $d1 -r" );
	ok( -w $d1, "Created $d1 -w" );
	mkdir $d2 or die "Failed to create $d2";
	ok( -d $d2, "Created $d2 ok" );
	ok( -r $d2, "Created $d2 -r" );
	ok( -w $d2, "Created $d2 -w" );
	# Copy in a known-readonly file (in this case, the File::Spec lib we are using
	File::Copy::copy( $INC{'File/Spec.pm'} => $f3 );
	chmod( 0400, $f3 );
	ok( -f $f3, "Created $f3 ok" );
	ok( -r $f3, "Created $f3 -r" );
    SKIP: {
	if ( $^O ne 'MSWin32' and $< == 0 ) {
		skip("This test doesn't work as root", 1);
	}
   	ok( ! -w $f3, "Created $f3 ! -w" );	
    };
}

sub clear_directory {
	if ( -e $f3 ) {
		chmod( 0700, $f3 ) or die "chmod 0700 $f3 failed";
		unlink( $f3 )      or die "unlink: $f3 failed";
		! -e $f3           or die "unlink didn't work";
	}
	if ( -e $d2 ) {
		rmdir( $d2 )       or die "rmdir: $d2 failed";
		! -e $d2           or die "rmdir didn't work";
	}
	if ( -e $d1 ) {
		rmdir( $d1 )       or die "rmdir: $d1 failed";
		! -e $d1           or die "rmdir didn't work";
	}
}

# Make sure there is no directory from a previous run
clear_directory();

# Create the directory
create_directory();

# Schedule cleanup
END {
	clear_directory();
}





#####################################################################
# Main Testing

# Call a recursive remove of the directory, nothing should be left after
is_deeply( [ File::Remove::remove( $f3 ) ], [ $f3 ], "remove('$f3') ok" );
ok( ! -e $f3, "Removed the file ok" );
