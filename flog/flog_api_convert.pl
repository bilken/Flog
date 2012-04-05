#!/usr/bin/perl

# Given a file name, open the file and convert log calls to
# double-paren log calls:
#   FLOG($severity, $module, fmt, args);
#   DP_FLOG_$severity_$module((fmt, args));
#
# See log_config.pl for details
#
# It's not a real parser, so it will barf on things like:
#   FLOG(INFO, TEST, "testing %s %d %d\n",
#       //"not used", 1, 2);
#       "use this", 3, 4);
#   FLOG(INFO, TEST, "testing: %s; %d\n", "str", 3);
#   FLOG(INFO,
#           TEST, "oops\n");
#
sub convertLogApis
{
    my $need_start = 1;
    my $ignore = 0;

    while (my $line = <>) {
        if ($ignore == 0) {
            if ($need_start) {
                if ($line =~ s/\s+(FLOG\w*)\s*\(\s*(\w+)\s*,\s*(\w+)\s*,/ DP_$1_$2_$3\(\(/g) {
                    $need_start = 0;
                } elsif ($line =~ s/^(FLOG\w*)\s*\(\s*(\w+)\s*,\s*(\w+)\s*,/ DP_$1_$2_$3\(\(/g) {
                    $need_start = 0;
                }
            }
            if (($need_start == 0) and ($line =~ s/;/\);/)) {
                $need_start = 1;
            }
        }
        print $line;
    }
}

convertLogApis();

