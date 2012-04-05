# Build rules for g++/clang in cygwin or mac/linux


# Override extensions for win vs linux
KERN := $(shell uname)
ifneq "$(patsubst CYGWIN%,%,$(KERN))" "$(KERN)"
APPEXT := .exe
LIBEXT := .lib
SHLIBEXT := .dll
else
APPEXT :=
LIBEXT := .a
SHLIBEXT := .so
# If doing a shared library...
CFLAGS += -fPIC
endif


ifndef CLANG
CC := gcc
CXX := g++
LINK := g++
else
CC := $(CLANG_BIN)clang
CXX := $(CLANG_BIN)clang++
LINK := $(CLANG_BIN)clang++
# inline C code doesn't link right between clang and gnu
CLANG_FLAGS := -std=c89# -Dinline=
endif

ASM := as
LBRN := ar

CFLAGS += -Wall

ifndef RELEASE
CFLAGS += -g -O0
else
CFLAGS += -O2
endif

CFLAGS += -Wformat
#CFLAGS += -pthread

GNU_INCLUDES := /usr/include /usr/local/include

CFLAGS += $(addprefix -D, $(DEFINES))
CFLAGS += $(addprefix -I, $(INCLUDES) $(GNU_INCLUDES))

CXXFLAGS += -fexceptions
CXXFLAGS += $(CFLAGS)
CXXFLAGS += $(addprefix -D, $(DEFINES_CPP))


# Compiler output switches
LISTASMSWITCH := -fverbose-asm
ASMLISTSWITCH := -L -alhs
PPSWITCH := -E -C

ASXFLAGS := -x assembler-with-cpp

CCFLAGS := $(CFLAGS) $(CLANG_FLAGS)
CCXXFLAGS := $(CXXFLAGS)
LFLAGS := -lm -lpthread -lstdc++

# Create a map file (may not work on mac)
LMAPFLAG = '-Wl,-Map=$(basename $@).map'

STRIP_STDINCS := $(SED) -e 's; /usr[^ ]*;;g' -e 's; .*lib/clang/[^ ]*;;g' | $(GREP) '[a-zA-Z]'

$(OPATH)%$(DEXT) : $(SPATH)%$(CEXT)
	$(QUIET)$(ECHO) "Build $@"; \
		set -e; \
		$(MKDIR) -p $(dir $@); \
		$(CC) -M -MT '$(basename $@)$(OEXT)' $(CCFLAGS) -I$(dir $<) $< | \
			$(STRIP_STDINCS) > $@.tmp; \
		$(MV) $@.tmp $@

$(OPATH)%$(DEXT) : $(SPATH)%$(CPPEXT)
	$(QUIET)$(ECHO) "Build $@"; \
		set -e; \
		$(MKDIR) -p $(dir $@); \
		$(CXX) -M -MT '$(basename $@)$(OEXT)' $(CCXXFLAGS) -I$(dir $<) $< | \
			$(STRIP_STDINCS) > $@.tmp; \
		$(MV) $@.tmp $@

$(OPATH)%$(LSTEXT) : $(SPATH)%$(CEXT) force
	$(QUIET)$(ECHO) "Build $@"; \
		set -e; \
		$(MKDIR) -p $(dir $@); \
		$(CC) -o $(basename $@)$(ASMEXT) $(CCFLAGS) -I$(dir $<) $(LISTASMSWITCH) -S $<; \
		$(ASM) $(ASMFLAGS) $(ASMLISTSWITCH) -o asm.out $(basename $@)$(ASMEXT) > $@; \
		$(RM) $*$(ASMEXT) asm.out ASM.OUT

$(OPATH)%$(LSTEXT) : $(SPATH)%$(CPPEXT) force
	$(QUIET)$(ECHO) "Build $@"; \
		set -e; \
		$(MKDIR) -p $(dir $@); \
		$(CXX) -o $(basename $@)$(ASMEXT) $(CCXXFLAGS) -I$(dir $<) $(LISTASMSWITCH) -S $<; \
		$(ASM) $(ASMFLAGS) $(ASMLISTSWITCH) -o asm.out $(basename $@)$(ASMEXT) > $@; \
		$(RM) $*$(ASMEXT) asm.out ASM.OUT

$(OPATH)%$(IEXT) : $(SPATH)%$(CEXT) force
	$(QUIET)$(ECHO) "Build $@"; \
		$(MKDIR) -p $(dir $@); \
		$(CC) -o $@ $(PPSWITCH) $(CCFLAGS) -I$(dir $<) $<

$(OPATH)%$(IEXT) : $(SPATH)%$(CPPEXT) force
	$(QUIET)$(ECHO) "Build $@"; \
		$(MKDIR) -p $(dir $@); \
		$(CXX) -o $@ $(PPSWITCH) $(CCXXFLAGS) -I$(dir $<) $<

$(OPATH)%$(OEXT) : $(SPATH)%$(CEXT)
	$(QUIET)$(ECHO) "Build $@"; \
		$(MKDIR) -p $(dir $@); \
		$(CC) -o $@ $(CCFLAGS) -I$(dir $<) -c $<

$(OPATH)%$(OEXT) : $(SPATH)%$(CPPEXT)
	$(QUIET)$(ECHO) "Build $@"; \
		$(MKDIR) -p $(dir $@); \
		$(CXX) -o $@ $(CCXXFLAGS) -I$(dir $<) -c $<

%$(LIBEXT) :
	$(QUIET)$(ECHO) Build $@; \
		$(RM) $@; \
		$(LBRN) -rsc $@ $+

%$(SHLIBEXT) :
	$(QUIET)$(ECHO) Build $@; \
		$(RM) $@; \
		$(LINK) -shared -o $@ $^

%$(APPEXT) : %$(OEXT)
	$(QUIET)$(ECHO) Build $@; \
		$(MKDIR) -p $(dir $@); \
		$(LINK) -o $@ $(LFLAGS) $(LMAPFLAG) $^

