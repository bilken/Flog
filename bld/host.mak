# Definitions for the host environment

# Quiet for now, unless overridden
QUIET ?= @

# Tools
AWK := awk
BASENAME := basename
BC := bc
BZIP2 := bzip2
CAT := cat
CHMOD := chmod
CP := cp -p
COPY := $(CP)
CTAGS := ctags
DIFF := diff
DIRNAME := dirname
DOS2UNIX := dos2unix -n
DOXYGEN := doxygen
ECHO := echo
FIND := find
GCC := gcc -B$(dir $(shell which gcc))
GREP := grep
HEAD := head
HEX2BIN := xxd -g 1 -ps -r
LN := ln -f
MKDIR := mkdir
MV := mv
PATCH := patch
PERL := perl
RM := rm -f
SED := sed
SORT := sort
TAIL := tail
TAR := tar
TOUCH := touch
TR := tr
UNIQ := uniq
UNIX2DOS := unix2dos -n
UNZIP := unzip
UUDECODE := uudecode
UUENCODE := uuencode
WC := wc
XARGS := xargs
ZIP := zip

