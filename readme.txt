
Flog logging

    Flog is a C/C++ logging facility with the following features:
        * Highly configurable, both at compile-time and run-time, for each
            user-defined log module independently.
        * Fully C compliant
        * Optional ostream-style interface for C++
        * User-defined formatting, configurable at compile-time and run-time.
        * Minimal code size
        * Minimal run time cost (i.e. high performance)
        * Public domain license

    Written by Billy Kennemer <eyepits@gmail.com>. Reimplemented from
    work done at Flashlight Engineering and Consulting (http://flec.tv).


Usage

    Printf-style logging:
        FLOG(SEVERITY, MODULE, FMT, ...);

    C++ ostream-style logging (with C++ compilers only):
        FLOGS(SEVERITY, MODULE, "Value: (" << val << ")" << std::endl);

    Log calls will be inhibited at compile-time when the maximum SEVERITY
    for the MODULE is less severe than the FLOG/FLOGS call's SEVERITY.

    At run-time, if dynamic configuration is enabled, log calls will be
    inhibited when the current assigned SEVERITY for the MODULE is less
    severe than the FLOG/FLOGS call's SEVERITY.

    General conditionals:
        // Compile-time test with scripted configurations
        #if[def] FLOG_TEST_SEVERITY_MODULE
            ...
        #endif

        // Compile-time test with unscripted configurations
        // Run-time test when FLOG_STATIC is not defined
        IF_FLOG_SET(SEVERITY, MODULE) {
            ...
        }

    Dynamic configuration of MODULE SEVERITies and format options:
        // When FLOG_STATIC is not defined
        int flog_interact_s(const char *arg, char *outStr, size_t outStrLen)
            Show settings:    flog_interact("");
            Get a log level:  flog_interact("MODULE");
            Set a log level:  flog_interact("MODULE SEVERITY");
            Adjust format:    flog_interact("(+|-) <format name>");


Samples

    There are four sample programs that show different ways to use Flog
    logging:

    1 static/static.cpp

        This sample program uses only compile-time log definitions. That
        is, the run-time log configurations and checks are inhibited.

        Log invocations with inhibited severity-module pairs are removed
        by the preprocessor.

        Format options are only configured at compile-time.

        Modules and severities are defined via script, in static/config.txt.

    2 simple/simple.c

        This sample program uses both run-time and compile-time log
        definitions. It is a C-only program, skipping C++ elements.

        Logging may be dynamically configured through command line
        parameters, e.g. "all:error" to set all module levels to
        a maximum severity of ERROR.

        Modules and severities are defined via script, in simple/config.txt.

    3 example/example.cpp

        This sample program uses both run-time and compile-time log
        definitions. It further includes several features the other
        samples omit:
            fancy formatting: timestamp, thread id
            file/buffer output
            varying configurations for debug and release

        Modules and severities are defined via several scripts:
            example/cfg/*.txt
            example/flog.h

    4 xconfig/xconfig.cpp

        This sample program uses both run-time and compile-time log
        definitions, but does so using only X-macro declarations. That
        is, the configuration does not include the perl_config.pl
        script; it is configured entirely in the code.

        Modules, severities and formatting are defined in
        xconfig/flog_xconfig.h.


    To build all samples and the test, execute 'make all'. The host
    and target platform are assumed to be unix-y (e.g. Linux or cygwin).
    GNU make, g++ (or clang++) must be installed. For scripted
    configurations, Perl must also be installed. The code isn't
    too fancy so most non-archaic versions should "just work".

    Execute the samples and test (in debug builds) as:
        o/d/static/static
        o/d/simple/simple
        o/d/example/example
        o/d/xconfig/xconfig
        o/d/test/test_flog


Installation

    Make the contents of flog/* accessible to your project.

    In the minimal case (unscripted and static logging), only flogx.h is
    needed. In the maximal case (scripted and dynamic logging), the
    flog_config.pl script and flog.c are also needed.

    Flog is not available as a library because flog.c must be built
    with the project's logging configuration.


Configuration

    The severities and modules available for log calls are defined in one
    of two ways:
        1) Unscripted
        2) Scripted

    In the unscripted case, a user defines all severities, modules and
    format options entirely in a C/C++ header file.
    The unscripted option is easier to setup. The compile-time feature
    may not work properly with all compilers (though it's fine with gcc/clang).

    The scripted option adds some extra features to Flog:
        * Modules and severities are defined in a separate configuration file
        * Compile-time inhibited FLOG calls are removed by the preprocessor.
        * FLOG_SEV_MOD(...) APIs are generated if the user prefers that form
        * FLOG_TEST_SEV_MOD macros are declared for conditional compilation
    The flog/flog_config.pl script must be executed to generate the
    Flog header.


    Unscripted Configuration

        If choosing an unscripted configuration, first create the project's
        $flogHeader file (most any relevant name is suitable):
        
            An example:
                xconfig/flog_xconfig.h

            Define severities and modules using the following
            X-Macro declarations:
                FLOG_SEVERITY_LIST
                FLOG_MODULE_LIST

            The FLOG_SEVERITY_LIST X-Macro is composed of one or more
            FLOG_SEVERITY_LIST_ITEM(NAME) elements. The NAME parameter
            defines a severity that is available for module configuration
            and subsequent FLOG() calls. The order of the severity
            items matters. They should be ordered from least severe to
            most.

            The FLOG_MODULE_LIST X-Macro is composed of one or more
            FLOG_MODULE_LIST_ITEM(NAME, MAX, DEFAULT) elements. The
            NAME parameter defines a module that is available for
            subsequent FLOG() calls. The MAX parameter defines the
            maximum compile-time severity for the module. The DEFAULT
            parameter defines the default run-time severity for the
            module. The order of this list doesn't matter much other
            than that it shows up in this order when log levels are
            listed from the flog_interact_s() API.

            Define 'FLOGX' to configure unscripted mode.

            Include the 'flogx.h' header to define FLOG() and FLOGS() APIs.

        Then, skip to the 'Common Configuration' section


    Scripted Configuration

        If using a scripted configuration, define severities and modules in
        one or more $configFiles:

            For example, use one of the following samples as a guide:
                example/cfg/*.txt
                simple/config.txt
                static/config.txt

            The configuration file format is simple but quirky and not
            well-defined. The examples above ought to be sufficient.

        Generate the $flogHeader file from the configuration:

            `[perl] flog/flog_config.pl flog/flogx.h $configFiles > $flogHeader`

            Note, perl must be installed on the target system. No extra perl
            modules should be needed.

            Note, this must be re-executed each time the config file(s) are
            changed. That is, whenever the compile-time settings for
            severities or modules are changed, the $flogHeader must be
            regenerated.

            The sample project makefiles execute this automatically for each
            sample $flogHeader. See project.mak for details. Your project can
            generate the $flogHeader as it sees fit.


    Common Configuration

        The $flogHeader, as generated by either the scripted or unscripted
        configuration method, is functional at this point with the following
        caveats:
        a) Log calls use stdout with 'printf'
        b) No log formatting is defined
        c) Dynamic configuration elements are included

        If these caveats are okay, your project may include the $flogHeader
        directly wherever logging is needed and you may proceed to the 
        'Dynamic Configuration' section.


        Optional configurations may be defined when including the $flogHeader.
        The sample programs show the effects of these configurations. Typically,
        a common header file will include the necessary definitions from
        below and then include the $flogHeader. All code using Flog would
        then include this encapsulating header.

        Output destination

            The output destination defaults to stdout. To change this, define
            the FLOG_PRINTF macro as needed. It should be a function that
            has the same signature as printf(), i.e.:
                int FLOG_PRINTF(const char * fmt, ...);

            See example/flog.h for one example.

        Formatting

            Logging generally includes some project specific content with
            each log line. These elements can be defined in an X-Macros:
                FLOG_FORMAT_LIST(FA, SEVERITY, MODULE) \
                    FLOG_FORMAT_LIST_ITEM(NAME, CFG, FMT, VALUE, NONVALUE, FA) \

            If left undefined, no formatting is added to each FLOG/FLOGS call.

            Each FLOG_FORMAT_LIST_ITEM element has the following parameters:
                1) The name to be used in dynamic configuration
                2) The configuration specifier, one of the following:
                    ON   - The element is always present in log output
                    CON  - The element is configurable, on by default
                    COFF - The element is configurable, off by default
                3) The printf-style format specifier. E.g. "%s".
                    Note, this must be a string literal so that it can be
                    concatenated with other format specifies and the FLOG()
                    format specifier.
                4) The value. This is the argument matching the format specifier
                    as defined in the corresponding FLOG_FORMAT_LIST_ITEM.
                    Note that the SEVERITY and MODULE names are available here
                    if needed.
                5) The inhibited value. If the item is configurable at run-time,
                    this is the value to pass in place of the real value.
                6) A place holder for X-Macro selection element selection.
                    Must match the first parameter to FLOG_FORMAT_LIST().

            The order of this list matters. It defines the order in which items
            appear on each log line. Note that Flog will always place the format
            elements ahead of the user's text on each line. The user is
            responsible for spacing of the elements either in the format
            specifiers or in the values.

            Lastly, some arguments may require stack space of some sort to do
            their work. In such cases, define all needed local variables --
            using unique names that won't clash with other local symbols --
            with the FLOG_FORMAT_DEC macro. For example:
                #define FLOG_FORMAT_DEC  some_struct_t myFormatParam;
            That variable will be available to each FLOG_ARGS_LIST_ITEM.

            See example/flog.h for a full example.
            See simple/simple.c for a minimal example.

    Dynamic configuration

        If dynamic configuration is not desired, define FLOG_STATIC.
        When used, FLOG_ARGS_LIST_ITEMs must only use the 'ON' configuration.
        See static/static.cpp for an example.

        If dynamic configuration is desired, two extra steps are required:
        a) At the start of main (before using logging), set the default
            run-time log levels by calling flog_init().
        b) In one of your source files, include flog/flog.c after including
            $flogHeader. Alternatively, modify flog.c to include the
            necessary $flogHeader and build/link that with your project.

        Lastly, invoke flog_interact_s() as needed to adjust Flog dynamic
        configurations.

        See the samples for usage:
            simple/simple.c
            example/example.cpp
            xconfig/xconfig.cpp

Caveats

    1) Flog won't work with compilers that lack variadic macros.
        To fix this, there are two options I see:
        a) Use only FLOGS() and FLOGSTRING() as these have fixed parameters.
            Write a new section in flog/flogx.h to flesh these out for the
            FLOG_VA_NONE case.
        b) Finish the flog/flog_api_convert.pl script so that it converts
            all FLOG() AND FLOGS() calls to to DP_FLOG_SEV_MOD(()) and
            DP_FLOGS_SEV_MOD(()) calls. Add /* $dpDefs */ to the FLOG_VA_NONE
            section of flog/flogx.h.

    2) The FMT string for FLOG() calls must be a string literal if any
        formatting is used. The format feature relies on string concatenation.

    3) In each FLOG_FORMAT_LIST_ITEM(NAME, CFG, FMT, ARG, DISARG, FA), the
        FMT value must be a string literal. Same reason as '2)' above.

    4) Format specifiers like %1$d aren't supported with FLOG() since the
        format values affect the numbering in obscure ways. However, if
        using C++, things like boost::format will work with FLOGS().

