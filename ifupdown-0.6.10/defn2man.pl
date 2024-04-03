#!/usr/bin/perl -w

use strict;

# declarations
my $line;
my $match;

# subroutines
sub nextline {
        $line = <>;
        while($line and ($line =~ /^#/ or $line =~ /^\s*$/)) {
                $line = <>;
        }
        if (!$line) { return 0; }
        chomp $line;
        while ($line =~ m/^(.*)\\$/) {
                my $addon = <>;
                chomp $addon;
                $line = $1 . $addon;
        }
        return 1;
}
sub match {
        my $line = $_[0];
        my $cmd = "$_[1]" ? "$_[1]\\b\\s*" : "";;
        my $indentexp = (@_ == 3) ? "$_[2]\\s+" : "";

        if ($line =~ /^${indentexp}${cmd}(([^\s](.*[^\s])?)?)\s*$/) {
                $match = $1;
                return 1;
        } else {
                return 0;
        } 
}
sub skip_section {
        my $struct = $_[0];
        my $indent = ($line =~ /(\s*)[^\s]/) ? $1 : "";

        1 while (nextline && match($line, "", $indent));
}
sub get_address_family {
        print ".SH " . uc($match) . " ADDRESS FAMILY\n";
        print "This section documents the methods available in the\n";
        print "$match address family.\n";
        nextline;
}
sub get_architecture {
        # no op
        nextline;
}
sub get_method {
        my $method = shift;
        my $indent = ($line =~ /(\s*)\S/) ? $1 : "";
        my $description = "";
        my @options = ();

        nextline;
        while ($line and match($line, "", $indent)) {
                if (match($line, "description", $indent)) {
                        $description = get_description();
                } elsif (match($line, "options", $indent)) {
                        @options = get_options();
                } else {
                        skip_section;
                }
        }

        print ".SS The $method Method\n";
        if ($description ne "") {
                print usenet2man($description) . "\n";
        } else {
                print "(No description)\n";
        }
        print ".PP\n";
        print ".B Options\n";
        print ".RS\n";
        if (@options) {
                foreach my $o (@options) {
                        if ($o =~ m/^\s*(\S*)\s*(.*)\s+--\s+(\S.*)$/) {
                                my $opt = $1;
                                my $optargs = $2;
                                my $dsc = $3;
                                print ".TP\n";
                                print ".BI $opt";
                                print " \" $optargs\"" unless($optargs =~ m/^\s*$/);
                                print "\n";
                                print usenet2man($dsc) . "\n";
                        } else {
                                print ".TP\n";
                                print ".B $o\n";
                        }
                }
        } else {
                print ".TP\n";
                print "(No options)\n";
        }
        print ".RE\n";
}
sub get_description {
        my $desc = "";
        my $indent = ($line =~ /(\s*)\S/) ? $1 : "";
        while(nextline && match($line, "", $indent)) {
                $desc .= "$match\n";
        }
        return $desc;
}
sub usenet2man {
        my $in = shift;
        my $out = "";

        $in =~ s/\s+/ /g;
        while ($in =~ m%^([^*/]*)([*/])([^*/]*)([*/])(.*)$%s) {
                my ($pre, $l, $mid, $r, $post) = ($1, $2, $3, $4, $5);
                if ($l eq $r && " $pre"  =~ m/[[:punct:][:space:]]$/ 
                             && "$post " =~ m/^[[:punct:][:space:]]/) {
                        $out .= $pre;
                        $out .= ($l eq "*" ? '\fB' : '\fI') . $mid . '\fP';
                        ($in = $post) =~ s/^\s+/ /;
                } else {
                        $out .= $pre . $l;
                        $in = $mid . $r . $post;
                }
        } 
        return $out . $in;
}
sub get_options {
        my @opts = ();
        my $indent = ($line =~ /(\s*)\S/) ? $1 : "";
        while(nextline && match($line, "", $indent)) {
                push @opts, $match;
        }
        return @opts;
}

# main code
nextline;
if ($line and match($line, "address_family")) {
        get_address_family $match;
} else {
        die "address_family must be listed first\n";
}
if ($line and match($line, "architecture")) {
        get_architecture $match;
}
while ($line and match($line, "method")) {
        get_method $match;
}
