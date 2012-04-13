#!/usr/bin/perl

use strict;
use warnings;

my %modules = ( );
my @severities = ('NONE');
my %defs = ( );

sub generateDefs
{
    my $c = 0;
    $defs{sevDefs} .= "#define FLOG_SEVERITY_LIST \\\n";
    foreach my $s (@severities) {
        $defs{sevDefs} .= "    FLOG_SEVERITY_LIST_ITEM($s) \\\n";
    }
    $defs{sevDefs} .= "\n";

    $c = 0;
    $defs{modDefs} .= "#define FLOG_MODULE_LIST \\\n";
    for my $k ( sort( keys %modules ) ) {
        my $max = $modules{$k}[0];
        my $def = $modules{$k}[1];
        $defs{modDefs} .= "    FLOG_MODULE_LIST_ITEM( $k, $max, $def ) \\\n";
    }
    $defs{modDefs} .= "\n";

    for my $m ( sort( keys %modules ) ) {
        my $buildlevel = $modules{$m}[0];

        my $i;

        # Stub out values below the compile-time minimum
        for ($i = 0; $i < scalar @severities; $i++) {
            my $s = $severities[$i];
            if ($buildlevel eq $s) {
                last;
            }
            my $s_ = $s . "_";

            $defs{gccDefs} .= "#define FLOG_$s_$m(FMT, ARGS...)\n";
            $defs{c99Defs} .= "#define FLOG_$s_$m(...)\n";
            $defs{noneDefs} .= "#define DP_FLOG_$s_$m(FMT_ARGS)\n";
            $defs{strDefs} .= "#define FLOGS_$s_$m(SSTR)\n";
        }

        while ($i < scalar @severities) {
            my $s = $severities[$i];
            my $s_ = $s . "_";
            $i++;

            $defs{gccDefs} .= "#define FLOG_$s_$m(FMT, ARGS...) _FLOG_CHKDO($s, $m, FMT , ##ARGS)\n";
            $defs{c99Defs} .= "#define FLOG_$s_$m(...) _FLOG_CHKDO($s, $m, __VA_ARGS__, \"\")\n";
            $defs{noneDefs} .= "#define DP_FLOG_$s_$m(FMT_ARGS) IF_FLOG_SET($s, $m) { FLOG_PRINTF FMT_ARGS; }\n";
            $defs{strDefs} .= "#define FLOGS_$s_$m(SSTR) _FLOGS_CHKDO($s, $m, SSTR)\n";
            $defs{testDefs} .= "#define FLOG_TEST_$s_$m 1\n";
        }   # foreach severity
    }   # foreach module
}

sub makeHeader
{
    my ($flogx) = @_;
    open(FILE, $flogx) or die "Error: Can't open source header, $flogx";
    while (my $fl = <FILE>) {
        print $fl;
        if ($fl =~ /\$(\w+Defs)/) {
            exists($defs{$1}) || die "Error: Unknown def $1";
            print $defs{$1};
        }
    }
}

sub parseConfigFile
{
    my ($myfile, $index) = @_;
    my @errors;
    my $num = 0;

    open(FILE, $myfile) or die "Error: Could not open $myfile";
    while (my $fl = <FILE>) {
        $num++;
        my ($mod, $max, $def);
        my ($line) = split(/[#\n]/, $fl);
        if ($line =~ /\((.*)\)/ ) {
            push @severities, split(/[,:\s()]+/, $1);
        } else { 
            if ($line =~ /(\w+)[^\w]+(\w+)[^\w]+(\w+)/) {
                $mod = $1;
                $max = uc $2;
                $def = uc $3;
            } elsif ($line =~ /([\w]+)[^\w]+([\w]+)/) {
                $mod = $1;
                $max = $def = uc $2;
            } else {
                if ($line =~ /\w+/) {
                    push @errors, "Error line $num: Unknown text '$line'\n";
                }
                next;
            }

            if ($index  && !(exists $modules{$mod})) {  
                push @errors, "Error line $num: Unknown module $mod in '$line'\n";
            } elsif (!grep(/^$max$/i, @severities)) {
                push @errors, "Error line $num: Unknown severity $max in '$line'\n";
            } elsif (!grep(/^$def$/i, @severities)) {
                push @errors, "Error line $num: Unknown severity $def in '$line'\n";
            } else {
                $modules{$mod} = [$max, $def];
            }
        }
    }
    close(FILE);

    if (@errors) {
         die "Failed to parse $myfile\n", @errors;
    }
}

# Store file contents to %modules
sub main
{
    my $flogxHeader = shift @_;

    my $i = 0;
    foreach my $cfg (@_) {
        parseConfigFile($cfg, $i);
        $i++;
    }

    shift @severities;  # Pop of 'NONE'
    @severities or die "No severities defined";
    @severities = map { uc $_ } reverse(@severities);

    my $numModules = keys %modules;
    if ($numModules eq 0) {
        die "Error: No modules defined";
    }

    generateDefs();
    makeHeader($flogxHeader);
}

main(@ARGV);

