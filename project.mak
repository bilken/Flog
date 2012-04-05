
APP_SOURCES := \
    example/example.cpp \
    simple/simple.c \
    static/static.cpp \

LIBNAME := libflog

SOURCES := \
    example/timestamp.c \
    example/pprintf.c \
    example/sbuf_printf.c \

UTEST_APP_SOURCES := \
    test/test_flog.c \


# Rules for generating the log headers
# A Normal project's makefile would only have one config rule
#   but each sample uses its own unique config.

LOG_CONFIG_PL := $(SPATH)flog/flog_config.pl

LOG_EXAMPLE_H := $(OPATH)flog_example.h
LOG_EXAMPLE_CFG_PATH := $(SPATH)example/cfg/
ifdef RELEASE
LOG_EXAMPLE_CFG := $(LOG_EXAMPLE_CFG_PATH)config.txt \
    $(LOG_EXAMPLE_CFG_PATH)config_release.txt \
    $(wildcard $(LOG_EXAMPLE_CFG_PATH)config_release_user.txt)
else
LOG_EXAMPLE_CFG := $(LOG_EXAMPLE_CFG_PATH)config.txt \
    $(wildcard $(LOG_EXAMPLE_CFG_PATH)config_user.txt)
endif

LOG_SIMPLE_H := $(OPATH)flog_simple.h
LOG_SIMPLE_CFG := $(SPATH)simple/config.txt

LOG_STATIC_H := $(OPATH)flog_static.h
LOG_STATIC_CFG := $(SPATH)static/config.txt

LOG_TEST_H := $(OPATH)flog_test.h
LOG_TEST_CFG := $(SPATH)test/config.txt

define LOG_TEMPLATE =
$($(1)_H) : $(LOG_CONFIG_PL) $($(1)_CFG)
	$(QUIET)$(ECHO) "Build $$@ from $$+"; \
        set -e; \
        $(PERL) $$+ > $$@.tmp; \
        $(MV) $$@.tmp $$@
$($(1)_CFG) :
	@$(ECHO) "Config file doesn't exist: $$@"; exit -1
FORCE_HEADERS += $($(1)_H)
endef

LGS := LOG_EXAMPLE LOG_SIMPLE LOG_STATIC LOG_TEST
$(foreach LG,$(LGS),$(eval $(call LOG_TEMPLATE,$(LG))))


# Rule for building tags
CTAGS_PATHS := flog example simple static $(OPATH)
tags :
	@$(ECHO) "Build $@"
	$(QUIET)$(CTAGS) -h .c.h.cpp.hpp --langmap=C++:.c.cpp.h.hpp --extra=q -R $(CTAGS_PATHS)

