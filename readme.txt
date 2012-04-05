
Flog logging

    Flog is a logging library of sorts with the following features:
        * Generates uniform logging output with compile-time and run-time
        configurability
        * Supports both printf-style (for C projects) and ostream-style
        * Supports distinct severity levels for each log module
        * Public domain license


Samples

    There are three sample programs that show different ways to use Flog
    logging:

    1 static/static.cpp

        This sample program uses only compile-time log definitions. That
        is, the run-time log configurations and checks are inhibited.
        Log invocations with inhibited severity-module pairs are removed
        by the preprocessor.

        Static/default log definitions:
            static/config.txt
            static/flog.h

    2 simple/simple.c

        This sample program uses both run-time and compile-time log
        definitions. It is a C-only program, skipping C++ elements.
        Pass dynamic log settings on the command line.

        Simple/default log definitions:
            simple/config.txt
            simple/flog.h

    3 example/example.cpp

        This sample program uses both run-time and compile-time log
        definitions. It further includes several features the other
        examples omit:
            timestamp
            thread id
            file/buffer output
            varying configurations for debug and release

        Static/default log definitions:
            example/cfg/*.txt
            example/flog.h

    To build all samples and the test, execute 'make all'. The host
    and target platform are assumed to be unix-y (e.g. Linux or cygwin).
    GNU make, g++ (or clang++) and Perl must be installed. The code isn't
    too fancy so most non-archaic versions should "just work".

    Execute the samples (in debug builds) as:
        o/d/static/static
        o/d/simple/simple
        o/d/example/example


Installation and Configuration

    1 Define severities and modules in a config file

        For example, use one of the following samples as a guide:
            - example/cfg/*.txt
            - simple/config.txt
            - static/config.txt

    2 Generate the flog header from the configuration:

        `[perl] flog/flog_config.pl $configFiles > $flogHeader`

        Note, perl must be installed on the target system. No
        extra perl modules should be needed.

        Note, this must be re-executed each time the config file(s)
        are changed. That is, whenever the compile-time settings for
        severities, modules or format options are changed, the
        flog inner header must be regenerated.

        The sample project makefiles execute this automatically
        for each sample $flogHeader. See project.mak for details.

        The format is simple but quirky and not well-defined.

    3 Configure Flog output

        The $flogHeader from step 2 is functional with the following
        caveats:
        a) Log calls output to stdout with 'printf'
        b) Timestamp and thread id elements are not supported
        c) Dynamic configuration elements are included

        If these caveats are okay, your project may include the
        $flogHeader directly wherever logging is needed and you may
        proceed to step 4.

        Otherwise, one or more of the following macros should be defined
        whenever the $flogHeader is included. You may wish to define a
        new header, such as example/flog.h, with the expected definitions
        and then include $flogHeader from there. Configurable macros:
            FLOG_STATIC     Define to disable dynamic configuration
            FLOG_PRINTF     The printf-like function Flog uses for output
            FLOG_TIME(ARG)  Return the time, optionally using 'ARG'
            FLOG_TIME_DEC   The declaration of the ARG for FLOG_TIME()
            FLOG_TIME_ARG   The ARG value as a parameter, not a declaration.
            FLOG_THREAD()   The routine to return a thread id (an int)

        Theoretically, things like FLOG_PRINTF could be defined
        differently in different source files. That's probably not the
        best idea, though. Logging usually needs to be consistent.

    4 Dynamic configuration

        If FLOG_STATIC is not defined, two extra steps are required:
        a) At the start of main (before using logging), set the
            default log levels by calling flog_init().
        b) In one of your source files, include flog/flog.c after
            including the $flogHeader.

        Lastly, invoke flog_interact_s() as needed to adjust Flog
        dynamic configurations.


Usage

    Printf-style logging:
        FLOG(<SEVERITY>, <MODULE>, <FMT>, ...);
        FLOG_<SEVERITY>_<MODULE>(<FMT>, ...);

    C++ ostream-style logging (with C++ compilers only):
        FLOGS(<SEVERITY>, <MODULE>, "Values: (" << item1 << ", " << item2 << ")" << std::endl);
        FLOGS_<SEVERITY>_<MODULE>("Values: (" << item1 << ", " << item2 << ")" << std::endl);

    Compile-time severity test:
        #if[def] FLOG_TEST_<SEVERITY>_<MODULE>
        ...
        #endif

    Dynamic configuration of log levels and formatting:
        int flog_interact(const char *arg);
            Show all modules: flog_interact("");
            Get a log level:  flog_interact("<module>");
            Set a log level:  flog_interact("<module> <severity>");
            Adjust format:    flog_interact("(+|-) <option>");

    At compile-time, each module has a maximum value defined.
    All log calls with that module and a lower severity are
    removed from compilation.
    


Issues and trade-offs

    * Each module has a maximum severity defined at compile-time. Unused
        module/severity pairs are compiled-out, so there is zero run-time
        performance.
    * Each module has a configurable run-time severity. The severity is
        checked before executing any logging code for each call. The
        severity check is the only thing executed in a log statement when
        the severity is inhibited.
    * The format options -- timestamp, mod/sev, file/line and function --
        are all configurable at compile-time and/or run-time. The thread
        id is configurable only at compile time.
    * __FILE__ strings are left intact so the compiler can produce only
        one copy per file (if it's sufficiently clever).
    * The format parameters to each FLOG* call are specified seperately,
        so code size is bloated a bit instead. For example, there'd be
        less code size if the module/severity, timestamp, thread id were
        all generated from within a function instead of being
        instantiated anew for invocation of FLOG.
    * flog_config.pl must be run before compilation, when changing
        compile-time maximums/defaults or changing modules. Changes to
        any module force recompilation of all files using logging. And
        either the build system or the user must ensure that the flog
        header and flog.c are rebuilt.
    * The configuration file format doesn't follow any conventions, but
        it's not too complicated.
    * Installation is not ideal because of the header generation
        dependency. The user defines the name of the flog header (and
        may generate another header to encapsulate that header), but the
        flog.c source file depends on that header during build. It is not
        possible to use it simply as a library. So the user has to do
        weird stuff to integrate flog into their project.
    * Run-time configuration checks (like the timestamp) hurt performance
        when the module's severity is permitted. Of course, run-time
        configurability can be disabled (that is, each configuration item
        can be configured only at compile-time).
    * The compiler errors produced for invalid modules and severities are
        a little confusing since they will report a faulty symbol,
        'FLOG_SEV_MOD', where 'MOD' and 'SEV' are whatever is in the FLOG
        call. The error line number will be accurate, so it's workable.
    * The conditional before each log call encodes the module and
        severity into a single value to reduce code size and the cost of
        the function call.

