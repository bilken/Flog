Build environment to get iterative builds

The makefiles are written for GNU make and assume a Unix-y shell (e.g.
    Linux or Cygwin).

The build is iterative, relying on auto-generated dependency files (*.d).
These files capture header and source file dependencies, but they do not
capture dependencies on the compiler, external libraries, flags or defines.
Be aware:
  * After adding, removing, moving or renaming files, always run
    'make clean' to get rid of faulty dependencies.
  * After adding libraries, changing CFLAGS, or adding DEFINES, run
    'make clean' first to make sure all objects incorporate the changes.


build.mak
    The heart of the build system. It defines the dependency chains for
    all application and test targets (and their intermediates).
    Importantly, it allows for the use of auto-generated dependency files
    for sources so that iterative builds are possible.

    It includes a few other makefiles, some defined by the user. To use
    it, a number of macros must be defined including at least: SPATH,
    OPATH, SOURCES_MAK and RULES_MAK.

SPATH
    The relative path from make's working directory to all sources. Note,
    this requires that all sources be placed in a single directory
    hierarchy for a single build.

OPATH
    The relative path from make's working directory to plop build outputs
    into. Object files, etc., are placed under $(OPATH)/ with a directory
    layout mirroring the sources in $(SPATH).

SOURCES_MAK
    This makefile is defined by the user and defines the project's build
    elements. It includes:
     * A list of library sources, application sources (files with main()),
        test library sources and test applications sources (also with main()).
     * Include paths and general #defines for application and test builds.
     * Custom rules

RULES_MAK (e.g. gnu.mak)
    Rules for building objects, libraries and executables. Note that
    the rules are written with dependency files and SPATH/OPATH in mind.


Further details

    * SOURCES_MAK - The file defining sources, libraries, tests,
        and applications. It should define the following items:
        SOURCES - Sources to link with all tests and applications.
            These are built into objects which are then archived
            into a single library, 'app.$LIB'.
        SINCLUDES - External include paths for sources, tests and
            applications. Tool chain include paths should be
            included in gnu.mak. Includes within the project
            should be relative and shouldn't be specified here.
        SOURCES_NOLIB - Special sources that are linked directly
            against all applications. This is a special case and
            shouldn't generally be used.
        APP_SOURCES - Application sources (source files with main())
        DEFINES - Compile-time defines. These are global, so try
            to keep these to a minimal.
        FTEST_SOURCES - Functional test sources. Same as SOURCES
            but only used with functional tests. Functional tests
            depend on SOURCES + FTEST_SOURCES.
        FTEST_APP_SOURCES - Functional test applications. Same
            as APP_SOURCES but only used with the 'ftest' target.
            Note that 'all' depends on 'test' which depends on
            'ftest'.
        UTEST_SOURCES - Unit test sources, shared by all unit tests.
        UTEST_APP_SOURCES - Unit test applications. Unit test
            applications only include UTEST_SOURCES and themselves.
            To 'link' with a limited number of other source files,
            use #include. For example, in unit_widget.cpp, add
            #include "widget.cpp" at the top.
        LIBS - External libraries to link against. Note that
            gnu.mak should include tool chain libraries (like
            libstdc++.a).
    * RULES_MAK - This makefile defines the build rules. The
        build.mak file defines a number of things before including
        the rules:
        - File extensions can be overridden here if desired:
            OEXT - Object file (e.g. .o)
            LIBEXT - Library/archive file
            SHLIBEXT - Shared library file
            IEXT - Preprocessed C/C++ file
            LSTEXT - Assembly listing
            APPEXT - Application/image extension
            RUNEXT - Test results extension
        - Build definitions
            DEFINES - List of compile time defines, e.g. MYDEF[=VAL]
            INCLUDES - List of include paths separated by spaces
        The rules are constructed carefully (tediously) such that
            each rule is of the form:
            $(OPATH)%$(TARGET_EXT) : $(SPATH)%$(SOURCE_EXT)
        Note that invocation of a new shell process from make is
            expensive(ish), so the makefiles take care to chain
            commands in a single shell command where possible.
    * HELP_MAK - The help file. This is a separate file because
        the alternatives aren't very good:
        - individual echo commands are slow (seperate shells)
        - multi-line make syntax is weird
    * QUIET - Typically set to '@'. $(QUIET) is prepended to
        each shell line (in build.mak and gnu.mak). The caller
        can override this definition ('QUIET=') when invoking
        make to see all shell lines.

