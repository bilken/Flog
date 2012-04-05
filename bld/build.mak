
# The default target
help :
default : help

# Path to this file
BLDPTH := $(dir $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
HELP_TXT := $(BLDPTH)help.txt
HOST_MAK := $(BLDPTH)host.mak

REQS := ":$(OPATH):$(SPATH):$(SOURCES_MAK):$(RULES_MAK):"
CREQS := $(findstring ::, $(REQS))
ifneq "$(CREQS)" ""
$(error Missing one or more definitions, $(REQS))
endif

# List all targets as either DEPSTGTS or NODEPSTGTS to control dependency generation
DEPSTGTS := app ftest utest test all test_app ftest_app utest_app
NODEPSTGTS := clean help default force tags
.PHONY: $(DEPSTGTS) $(NODEPSTGTS)

# Include host environment definitions
include $(HOST_MAK)

# Create $(OPATH) directory
$(shell $(MKDIR) -p $(OPATH))


# Define extensions to use, can be overridden in RULES_MAK
CEXT := .c
CPPEXT := .cpp
HEXT := .h
HPPEXT := .hpp
DEXT := .d
OEXT := .o
IEXT := .i
LSTEXT := .lst
ASMEXT := .s
#APPEXT :=  # Not defined unless building for windows, caller defined
LIBEXT := .a
LIBEXT := .so
RUNEXT := .test


FORCE_HEADERS =

# Fill out:
#   SOURCES, SOURCES_NOLIB, APP_SOURCES,
#   FTEST_SOURCES, FTEST_APP_SOURCES
#   UTEST_SOURCES, UTEST_APP_SOURCES
#   SINCLUDES
#   DEFINES
include $(SOURCES_MAK)

# Set project include paths relative to the build path
INCLUDES := $(addprefix $(SPATH), $(SINCLUDES)) $(OPATH)

# The platform makefile:
#  * Define the build rules, %.c:%.o, %.lib, .etc
include $(RULES_MAK)

EXTS := ":$(CEXT):$(CPPEXT):$(DEXT):$(OEXT):$(IEXT):$(LSTEXT):"
COLONS := $(findstring ::, $(EXTS))
ifneq "$(COLONS)" ""
$(error Missing extension definitions. Do not call directly)
endif

# Check to make sure outputs won't clobber inputs
INPUTS  := $(CEXT) $(CPPEXT) $(HEXT) $(HPPEXT)
OUTPUTS := $(DEXT) $(IEXT) $(IPPEXT) $(LSTEXT) $(OEXT) $(LIBEXT) $(APPEXT) $(RUNEXT)
SUFFIX_MATCHES := $(strip $(filter $(OUTPUTS), $(INPUTS)))
ifneq "$(SUFFIX_MATCHES)" ""
$(error Output suffix matches input $(SUFFIX_MATCHES))
endif

.SUFFIXES :
.SUFFIXES : $(CEXT) $(CPPEXT) $(OEXT) $(APPEXT) $(RUNEXT) $(IEXT) $(LSTEXT) $(DEXT)


# Make paths relative to this makefile
OBJS := $(addprefix $(OPATH), $(addsuffix $(OEXT), $(basename $(SOURCES))))
OBJSNL := $(addprefix $(OPATH), $(addsuffix $(OEXT), $(basename $(SOURCES_NOLIB))))
DEPS := $(patsubst %$(OEXT), %$(DEXT), $(OBJS) $(OBJSNL))
LIB := $(OPATH)$(addsuffix $(SHLIBEXT), $(SHLIBNAME))$(addsuffix $(LIBEXT), $(LIBNAME))
APP_OBJS := $(addprefix $(OPATH), $(addsuffix $(OEXT), $(basename $(APP_SOURCES))))
APP_DEPS := $(patsubst %$(OEXT), %$(DEXT), $(APP_OBJS))
APP_EXES := $(patsubst %$(OEXT), %$(APPEXT), $(APP_OBJS))

FTEST_OBJS := $(addprefix $(OPATH), $(addsuffix $(OEXT), $(basename $(FTEST_SOURCES))))
FTEST_DEPS := $(patsubst %$(OEXT), %$(DEXT), $(FTEST_OBJS))
FTEST_LIB := $(OPATH)ftest$(LIBEXT)
FTEST_APP_OBJS := $(addprefix $(OPATH), $(addsuffix $(OEXT), $(basename $(FTEST_APP_SOURCES))))
FTEST_APP_DEPS := $(patsubst %$(OEXT), %$(DEXT), $(FTEST_APP_OBJS))
FTEST_APP_EXES := $(patsubst %$(OEXT), %$(APPEXT), $(FTEST_APP_OBJS))
FTEST_APP_RUNS := $(patsubst %$(APPEXT), %$(RUNEXT), $(FTEST_APP_EXES))

UTEST_OBJS := $(addprefix $(OPATH), $(addsuffix $(OEXT), $(basename $(UTEST_SOURCES))))
UTEST_DEPS := $(patsubst %$(OEXT), %$(DEXT), $(UTEST_OBJS))
UTEST_LIB := $(OPATH)utest$(LIBEXT)
UTEST_APP_OBJS := $(addprefix $(OPATH), $(addsuffix $(OEXT), $(basename $(UTEST_APP_SOURCES))))
UTEST_APP_DEPS := $(patsubst %$(OEXT), %$(DEXT), $(UTEST_APP_OBJS))
UTEST_APP_EXES := $(patsubst %$(OEXT), %$(APPEXT), $(UTEST_APP_OBJS))
UTEST_APP_RUNS := $(patsubst %$(APPEXT), %$(RUNEXT), $(UTEST_APP_EXES))


# Platform makefile may choose additional precious targets
.PRECIOUS: $(OPATH)%$(OEXT)

# Skip dependencies for non-build targets
ifneq ("$(MAKECMDGOALS)","")
ifeq ("$(filter $(MAKECMDGOALS), $(NODEPSTGTS))", "")

ifndef NO_DEPENDENCIES
# The compiler includes relative paths for each header dependency
vpath %$(HEXT) .
-include $(DEPS) $(APP_DEPS) $(FTEST_DEPS) $(FTEST_APP_DEPS) $(UTEST_DEPS) $(UTEST_APP_DEPS)
endif # !NO_DEPENDENCIES

# Make sure auto-generated headers exist before any header dependencies
# without making the header dependencies explicitly depend on them. If
# the dependency files depended directly on these, they'd get rebuilt
# unnecessarily.
#
# Note, for some reason, makefile includes are done in LIFO order, so
# put this dependent one last.
FORCE_HEADERS_MAK := $(OPATH)force_headers.mak
include $(FORCE_HEADERS_MAK)

endif # !clean
endif # no target


$(FORCE_HEADERS_MAK): $(FORCE_HEADERS)
	@$(ECHO) "# Force $+ to exist before dependencies are built." > $@



APP_DONE_CMD = \
    $(ECHO) "All ($(words $+)) targets are up to date:"; \
    for a in $+; do echo "  $$a"; done; \


force :

all : $(LIB) $(APP_EXES) $(FTEST_APP_RUNS) $(UTEST_APP_RUNS)
	@$(APP_DONE_CMD)

app : $(APP_EXES)
	@$(APP_DONE_CMD)

lib : $(LIB)
	@$(APP_DONE_CMD)

$(APP_EXES) $(FTEST_APP_EXES) : $(LIB) $(LIBS)
$(APP_EXES) : $(OBJSNL)
$(LIB) : $(OBJS)
$(LIBS) :
	@$(ECHO) "Warning: $@ not found!"

test : $(FTEST_APP_RUNS) $(UTEST_APP_RUNS)
	@$(APP_DONE_CMD)

ftest : $(FTEST_APP_RUNS)
	@$(APP_DONE_CMD)

utest : $(UTEST_APP_RUNS)
	@$(APP_DONE_CMD)

test_app : $(FTEST_APP_EXES) $(UTEST_APP_EXES)
	@$(APP_DONE_CMD)

ftest_app : $(FTEST_APP_EXES)
	@$(APP_DONE_CMD)

utest_app : $(UTEST_APP_EXES)
	@$(APP_DONE_CMD)

$(FTEST_APP_EXES) : $(FTEST_LIB)
$(FTEST_LIB) : $(FTEST_OBJS)

$(UTEST_APP_EXES) : $(UTEST_LIB) $(LIBS)
$(UTEST_LIB) : $(UTEST_OBJS)

%$(RUNEXT) : %$(APPEXT)
	$(QUIET)$(ECHO) Run $<; \
		set -e; \
		$< > $@.tmp; \
		$(MV) $@.tmp $@

clean :
	$(QUIET)$(RM) \
		$(OBJS) $(LIB) $(OBJSNL) $(APP_OBJS) \
		$(FTEST_OBJS) $(FTEST_LIB) $(FTEST_APP_OBJS) \
		$(UTEST_OBJS) $(UTEST_LIB) $(UTEST_APP_OBJS) \
		$(DEPS) $(APP_DEPS) \
		$(FTEST_DEPS) $(FTEST_APPS_DEPS) \
		$(UTEST_DEPS) $(UTEST_APP_DEPS) \
		$(FORCE_HEADERS)

help :
	$(QUIET)$(CAT) $(HELP_TXT)
	$(QUIET)$(ECHO) "Build output is stored to $(OPATH) $(BUILD_LOG_OUTPUT_FILE)"; $(ECHO) ""


ifdef NO_DEPENDENCIES
$(warning "NO_DEPENDENCIES: be sure to clean after header file changes!")
endif

