#!/usr/bin/perl

use strict;
use warnings;

my %hash = ( );
my @severities = ('NONE');

my %confhash =
(
    'timestamp' => -1,
    'thread' => 0,
    'sevmod' => -1,
    'fileline' => 0,
    'function' => 0,
);

sub outputSeverities
{
    my $c = 0;
    print "enum {\n";
    print "    FLOG_SVR_INVALID = -1,\n";
    foreach my $s (@severities) {
        print "    FLOG_SVR_$s = $c, \n";
        $c++;
    }
    print "    FLOG_SVR_NUM_ELEMENTS = $c,\n";
    print "};\n";

    print "\n";

    print "#define FLOG_SEVERITY_LIST \\\n";
    foreach my $s (@severities) {
        print "    FLOG_SEVERITY_LIST_ITEM($s) \\\n";
    }
    print "\n";
}

sub outputModules
{
    my $c = 0;
    print "enum {\n";
    print "    FLOG_MOD_INVALID = -1,\n";
    for my $k ( sort( keys %hash ) ) {
        print "    FLOG_MOD_$k = $c,\n";
        $c++;
    }
    print "    FLOG_MOD_NUM_ELEMENTS = $c,\n";
    print "    FLOG_MOD_ALL = $c,\n";
    print "};\n";
    print "\n";

    print "#define FLOG_MODULE_LIST \\\n";
    for my $k ( sort( keys %hash ) ) {
        my $max = $hash{$k}[0];
        my $def = $hash{$k}[1];
        print "    FLOG_MODULE_LIST_ITEM( $k, $max, $def ) \\\n";
    }
    print "\n";
}

sub outputFlags
{
    my $k;
    my $c = 1;
    my $d = 0;
    print "enum {\n";
    my $flist = "#define FLOG_FLAGS_LIST \\\n";
    foreach $k (keys %confhash) {
        my $l = $confhash{$k};
        if ($l eq 0) {
            next;
        }
        print "    FLOG_FLAG_" . uc $k . " = $c,\n";
        $flist .= "    FLOG_FLAGS_LIST_ITEM(" . uc $k . ", $c) \\\n";
        $d += ($l > 0) ? $c : 0;
        $c *= 2;
    }
    print "};\n";
    print "\n";

    print "$flist\n";

    print "#define FLOG_FLAGS_DEFAULT $d\n";
    print "#define FLOG_FLAG(FF) (flog_flags & FLOG_FLAG_##FF)\n";
}

sub get_formatting
{
    my $timestamp = $confhash{timestamp};
    my $thread = $confhash{thread};
    my $sevmod = $confhash{sevmod};
    my $fileline = $confhash{fileline};
    my $function = $confhash{function};

    # If the compiler lets us insert items, use this
    my $fmt = '';
    my $args = '';
    my $decl = '';

    # If the compiler doesn't let us insert args, use simple string concatenation
    my $fmt_no_args = "";

    if ($timestamp) {
        $fmt .= "%s";
        $args .= ($timestamp >= 2) ?
                " FLOG_TIME(FLOG_TIME_ARG) , " :
                " FLOG_FLAG(TIMESTAMP) ? FLOG_TIME(FLOG_TIME_ARG) : \"\" , ";
        $decl .= "FLOG_TIME_DEC;"

        # There is no argument free way to do this :-(
    }

    if ($thread) {
        $fmt .= '%d ';
        $args .= ' FLOG_THREAD() , ';
    }

    if ($sevmod) {
        $fmt .= "%s";
        my $ms = '#SEVERITY "[" #MODULE "] "';
        $args .= ($sevmod >= 2) ?
                $ms . ' , ' :
                ' FLOG_FLAG(SEVMOD) ? ' . $ms . ' : "" , ';

        $fmt_no_args .= $ms;
    }

    if ($fileline) {
        $fmt .= "%s%s";
        if ($fileline >= 2) {
            # the line could be put in the args here...
            $args .= " FILE_SHORTEN(__FILE__), LINETOSTRING(__LINE__) , ";
        } else {
            $args .= " FLOG_FLAG(FILELINE) ? FILE_SHORTEN(__FILE__) : \"\" ,";
            $args .= " FLOG_FLAG(FILELINE) ? LINETOSTRING(__LINE__) : \"\" , ";
        }

        $fmt_no_args .= "\"__FILE__ LINETOSTRING(__LINE__)\"";
    }

    if ($function) {
        $fmt .= "%s%s";
        if ($function >= 2) {
            $args .= " __FUNCTION__, \"() \" , ";
        } else {
            $args .= " FLOG_FLAG(FUNCTION) ? __FUNCTION__ : \"\" ,";
            $args .= " FLOG_FLAG(FUNCTION) ? \"() \" : \"\" ,";
        }

# It's a variable, not a literal string :-(
#        $fmt_no_args .= $space . "\" __FUNCTION__ \"";
    }

    if ($fmt) {
        $fmt = "\"$fmt\"";
        $fmt_no_args = "\"$fmt_no_args\"";
    }

    return ($fmt, $args, $decl);
}

sub outputMacroForm
{
    print <<FLOGALL;
#define FLOGSTRING(SEVERITY, MODULE, STR) FLOG(SEVERITY, MODULE, "%s", (STR).c_str())
#define FLOGS(SEVERITY, MODULE, SSTR) FLOGS_##SEVERITY##_##MODULE(SSTR)
FLOGALL

    my ($ex_fmt, $ex_args, $ex_decl) = get_formatting();
    for my $type ('GCC', 'C99', '') {
        print "\n";
        if ($type) {
            print "#if FLOG_VA_TYPE == FLOG_VA_$type\n";
            if ($type eq 'GCC') {
                print <<FLOGGCC;
#define FLOG(SEVERITY, MODULE, FMT, ARGS...) FLOG_##SEVERITY##_##MODULE(FMT , ##ARGS)
#define _FLOG_DO(SEVERITY, MODULE, FMT, ARGS...) \\
    $ex_decl FLOG_PRINTF($ex_fmt FMT , $ex_args ##ARGS);
#define _FLOG_CHKDO(SEVERITY, MODULE, FMT, ARGS...) \\
    IF_FLOG_SET(SEVERITY, MODULE) { _FLOG_DO( SEVERITY, MODULE, FMT , ##ARGS ); }
#define _FLOGS_CHKDO(SEVERITY, MODULE, SSTR) \\
    IF_FLOG_SET(SEVERITY, MODULE) { \\
        std::ostringstream _mSSTR; _mSSTR << SSTR; \\
        _FLOG_DO(SEVERITY, MODULE, "%s", _mSSTR.str().c_str()); \\
    }
FLOGGCC
            } elsif ($type eq 'C99') {
                print <<FLOGC99;
/* Note, the extra "" arg lets fmt-only FLOG() calls work */
#define FLOG(SEVERITY, MODULE, ...) FLOG_##SEVERITY##_##MODULE(__VA_ARGS__, "")
#define _FLOG_DO(SEVERITY, MODULE, FMT, ...) \\
    $ex_decl FLOG_PRINTF($ex_fmt FMT "%s", $ex_args __VA_ARGS__);
#define _FLOG_CHKDO(SEVERITY, MODULE, FMT, ...) \\
    IF_FLOG_SET(SEVERITY, MODULE) { _FLOG_DO( SEVERITY, MODULE, FMT, __VA_ARGS__); }
#define _FLOGS_CHKDO(SEVERITY, MODULE, SSTR) \\
    IF_FLOG_SET(SEVERITY, MODULE) { \\
        std::ostringstream _mSSTR; _mSSTR << SSTR; \\
        _FLOG_DO(SEVERITY, MODULE, "%s", _mSSTR.str().c_str(), ""); \\
    }
FLOGC99
            }
        }
        for my $m ( sort( keys %hash ) ) {
            my $buildlevel = $hash{$m}[0];

            my $i;

            # Stub out values below the compile-time minimum
            for ($i = 0; $i < scalar @severities; $i++) {
                my $s = $severities[$i];
                if ($buildlevel eq $s) {
                    last;
                }
                my $s_ = $s . "_";

                if ($type eq 'GCC') {
                    print "#define FLOG_$s_$m(FMT, ARGS...)\n";
                } elsif ($type eq 'C99') {
                    print "#define FLOG_$s_$m(...)\n";
                } elsif ($type eq 'NONE') {
                    print "#define DP_FLOG_$s_$m(FMT_ARGS)\n";
                } else {
                    print "#define FLOGS_$s_$m(SSTR)\n";
                }
            }

            while ($i < scalar @severities) {
                my $s = $severities[$i];
                my $s_ = $s . "_";
                $i++;

                if ($type eq 'GCC') {
                    print <<FORGCC;
#define FLOG_$s_$m(FMT, ARGS...) _FLOG_CHKDO($s, $m, FMT , ##ARGS )
FORGCC
                } elsif ($type eq 'C99') {
                    print <<FORC99;
#define FLOG_$s_$m(...) _FLOG_CHKDO($s, $m, __VA_ARGS__)
FORC99
                } elsif ($type eq 'NONE') {
                    print <<FORNONE;
#define DP_FLOG_$s_$m(FMT_ARGS) \\
    IF_FLOG_SET($s, $m) { FLOG_PRINTF FMT_ARGS; }
FORNONE
                } else {
                    print <<FOROTHER;
#define FLOGS_$s_$m(SSTR) _FLOGS_CHKDO($s, $m, SSTR)
#define FLOG_TEST_$s_$m 1
FOROTHER
                }
            }   # foreach severity
        }   # foreach module
        if ($type) {
            print "#endif /* FLOG_VA_TYPE == $type */\n";
        }
    }
}

sub outputLogHead
{
    print <<DEF_FLOG_HEAD;

#ifdef __cplusplus
#include <sstream>
#endif

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void flog_init(void);

typedef int flog_severity_t;
typedef int flog_module_t;
typedef int flog_sev_mod_t;

extern int flog_interact_s(const char *arg, char *outStr, size_t outStrLen);
extern int flog_set(flog_severity_t s, flog_module_t m);

typedef struct
{
    char * buf;
    size_t size;
} psn_t;
extern int flog_psnprintf( psn_t * psn, const char *format, ... );

extern int flog_ms_is_set(flog_sev_mod_t ms);
extern int flog_flags;
extern const char *flog_file_name_shorten(const char *fn);

#ifdef __cplusplus
}
#endif


#ifndef FLOG_PRINTF
#include <stdio.h>
#define FLOG_PRINTF printf
#endif

#ifndef FLOG_TIME
#define FLOG_TIME
#undef FLOG_TIME_DEC
#define FLOG_TIME_DEC
#undef FLOG_TIME_ARG
#define FLOG_TIME_ARG ""
#endif

#ifndef FLOG_THREAD
#define FLOG_THREAD 0
#endif

#define FLOG_VA_NONE 0
#define FLOG_VA_GCC  1
#define FLOG_VA_C99  2

#if defined(__GNUC__) || defined(__clang__)
#  define FLOG_VA_TYPE FLOG_VA_GCC
#  define FILE_SHORTEN(fn) flog_file_name_shorten(fn)
#elif 0 /* C-99 compiler */
#  define FLOG_VA_TYPE FLOG_VA_C99
#  define FILE_SHORTEN(fn) fn
#else   /* No variadic macros */
#error "not supported"
#  define FLOG_VA_TYPE FLOG_VA_NONE
#  define FILE_SHORTEN(fn) fn
#endif

#ifndef FLOG_STATIC
#define FLOG_ENCODE_SEV_BITS 4
#define FLOG_ENCODE(S, M) \\
    ( (flog_sev_mod_t) ( ( (M) << FLOG_ENCODE_SEV_BITS ) | (S) ) )
#define FLOG_DECODE(MS, S, M) \\
    do { \\
        (S) = (MS) & ((1 << FLOG_ENCODE_SEV_BITS) - 1); \\
        (M) = (MS) >> FLOG_ENCODE_SEV_BITS; \\
    } while (0)
#define IF_FLOG_SET(S, M) if (flog_ms_is_set(FLOG_ENCODE(FLOG_SVR_##S, FLOG_MOD_##M)))
#else
#define IF_FLOG_SET(S, M)
#endif

#define LINE2STR(X) #X
#define LINETOSTRING(X) "[" LINE2STR(X) "] "

DEF_FLOG_HEAD
}

sub outputConfigHeader
{
    print <<WARNINGS;
/** WARNING: AUTOGENERATED CONTENT, CHANGES WILL BE LOST **/
#ifndef __AUTO_FLOG_H__
#define __AUTO_FLOG_H__

WARNINGS

    outputSeverities();
    outputModules();
    outputFlags();

    outputLogHead();
    outputMacroForm();

    print <<WARNINGS;

#endif  /* __AUTO_FLOG_H__ */

WARNINGS
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
        if ($line =~ /([-+])\s*(\w+)/ ) {
            my $fmt = $2;
            my $op = $1;
            if (!exists $confhash{$2}) {  
                push @errors, "Error line $num: Unknown format option $2 in '$line'\n";
                next;
            } elsif ($line =~ /([-+]{2})\s*(\w+)/ ) {
                $op = $1;
            }
            $confhash{$fmt} =
                    $op eq '+' ? 1 :    # config, on
                    $op eq '-' ? -1 :   # config, off
                    $op eq '++' ? 2 :   # always on
                    0;                  # --, always off
        } elsif ($line =~ /\((.*)\)/ ) {
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

            if ($index  && !(exists $hash{$mod})) {  
                push @errors, "Error line $num: Unknown module $mod in '$line'\n";
            } elsif (!grep(/^$max$/i, @severities)) {
                push @errors, "Error line $num: Unknown severity $max in '$line'\n";
            } elsif (!grep(/^$def$/i, @severities)) {
                push @errors, "Error line $num: Unknown severity $def in '$line'\n";
            } else {
                $hash{$mod} = [$max, $def];
            }
        }
    }
    close(FILE);

    if (@errors) {
         die "Failed to parse $myfile\n", @errors;
    }
}

# Store file contents to %hash
sub main
{
    my $i = 0;
    foreach my $cfg (@_) {
        parseConfigFile($cfg, $i);
        $i++;
    }

    @severities or die "No severities defined";
    @severities = map { uc $_ } reverse(@severities);

    my $numModules = keys %hash;
    if ($numModules eq 0) {
        die "Error: No modules defined";
    }

    outputConfigHeader();
}


main(@ARGV);

