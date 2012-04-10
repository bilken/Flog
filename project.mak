
APP_SOURCES := \
    example/example.cpp \
    simple/simple.c \
    static/static.cpp \
    xconfig/xconfig.cpp \

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


LOGH := $(LOG_EXAMPLE_H) $(LOG_SIMPLE_H) $(LOG_STATIC_H) $(LOG_TEST_H)
LOGCFG := $(LOG_EXAMPLE_CFG) $(LOG_SIMPLE_CFG) $(LOG_STATIC_CFG) $(LOG_TEST_CFG)

$(LOGH) : $(LOG_CONFIG_PL)
	$(QUIET)$(ECHO) "Build $@ from $+"; \
    set -e; \
    $(PERL) $+ > $@.tmp; \
    $(MV) $@.tmp $@

$(LOG_EXAMPLE_H) : $(LOG_EXAMPLE_CFG)
$(LOG_SIMPLE_H) : $(LOG_SIMPLE_CFG)
$(LOG_STATIC_H) : $(LOG_STATIC_CFG)
$(LOG_TEST_H) : $(LOG_TEST_CFG)

FORCE_HEADERS += $(LOGH)

$(LOGCFG) :
	@$(ECHO) "Config file doesn't exist: $$@"; exit -1


# Rule for building tags
CTAGS_PATHS := flog example simple static xconfig $(OPATH)
tags :
	@$(ECHO) "Build $@"
	$(QUIET)$(CTAGS) -h .c.h.cpp.hpp --langmap=C++:.c.cpp.h.hpp --extra=q -R $(CTAGS_PATHS)

