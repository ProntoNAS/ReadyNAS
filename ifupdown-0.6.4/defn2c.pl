#!/usr/bin/perl -w

use strict;

# declarations
my $address_family = "";
my %methods = ();
my $line = "";
my $match = "";

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
sub get_address_family {
        $address_family = $_[0] if ($address_family eq "");
        nextline;
}
sub get_architecture {
        my $arch = $_[0];
        die("architecture declaration appears too late") if (keys %methods);
        print "#include \"arch${arch}.h\"\n\n\n";
        nextline;
}
sub get_method {
        my $method = $_[0];
        my $indent = ($line =~ /(\s*)[^\s]/) ? $1 : "";

        die "Duplicate method $method\n" if ($methods{$method}++);

        nextline;
        if (match($line, "description", $indent)) {
                skip_section();
        }
        if (match($line, "options", $indent)) {
                skip_section();
        }
        if (match($line, "up", $indent)) {
                get_commands(${method}, "up");
        } else {
                print "static int ${method}_up(interface_defn ifd) { return 0; }\n"
        }
        if (match($line, "down", $indent)) {
                get_commands(${method}, "down");
        } else {
                print "static int ${method}_down(interface_defn ifd) { return 0; }\n"
        }
}
sub skip_section {
        my $struct = $_[0];
        my $indent = ($line =~ /(\s*)[^\s]/) ? $1 : "";

        1 while (nextline && match($line, "", $indent));
}
sub get_commands {
        my $method = $_[0];
        my $mode = $_[1];
        my $function = "${method}_${mode}";
        my $indent = ($line =~ /(\s*)[^\s]/) ? $1 : "";

        print "static int ${function}(interface_defn *ifd, execfn *exec) {\n";

        while (nextline && match($line, "", $indent)) {
                if ( $match =~ /^(.*[^\s])\s+if\s*\((.*)\)\s*$/ ) {
                        print "if ( $2 ) {\n";
                        print "  if (!execute(\"$1\", ifd, exec)) return 0;\n";
                        print "}\n";
                } elsif ( $match =~ /^(.*[^\s])\s+elsif\s*\((.*)\)\s*$/ ) {
                        print "else if ( $2 ) {\n";
                        print "  if (!execute(\"$1\", ifd, exec)) return 0;\n";
                        print "}\n";
                } elsif ( $match =~ /^(.*[^\s])\s*$/ ) {
                        print "{\n";
                        print "  if (!execute(\"$1\", ifd, exec)) return 0;\n";
                        print "}\n";
                }
        }

        print "return 1;\n";
        print "}\n";
}

# main code
print "#include \"header.h\"\n\n\n";
nextline;
while($line) {
        if (match($line, "address_family")) {
                get_address_family $match;
                next;
        }
        if (match($line, "architecture")) {
                get_architecture $match;
                next;
        }
        if (match($line, "method")) {
                get_method $match;
                next;
        }

        # ...otherwise
        die("Unknown command \"$line\"");
}
print "static method methods[] = {\n";
my $method;
foreach $method (keys %methods) {
        print <<EOF;
        {
                "$method",
                ${method}_up, ${method}_down,
        },
EOF
}
print "};\n\n";

print <<EOF;
address_family addr_${address_family} = {
        "$address_family",
        sizeof(methods)/sizeof(struct method),
        methods
};
EOF
