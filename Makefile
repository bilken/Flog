
# Define RELEASE properties
# Override as needed
ifdef RELEASE
OPATH ?= o/r/
else
OPATH ?= o/d/
endif

# Set 'QUIET=' to show commands
QUIET ?= @

# Set build inputs, relative to cwd
SPATH := ./
SOURCES_MAK := project.mak

# Build using gnu tools
RULES_MAK := bld/gnu.mak

include bld/build.mak

