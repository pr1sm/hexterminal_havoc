#  Makefile for Hexterminal Havoc
#  This is a generic makefile that has been modified for fit the source
#  structure of the project.
#
#  This makefile has been adapted to fit the project needs from the 
#  following source: https://gist.github.com/kbingham/4414922

#  the compiler: using gcc for C
CC = gcc

#  debug flags:
#  -ggdb ~> add gdb debugging information to the binary
DFLAGS = -ggdb

#  Attach a version number to each build
VERSION := $(shell git describe --tags --abbrev=0 2>/dev/null)

#  compiler flags:
#  -Wall     ~> display all warnings
#  -Werror   ~> treat warnings as errors
#  -DVERSION ~> attach a version to all builds
#  also include DFLAGS
CFLAGS = -Wall -Werror $(DFLAGS) -DVERSION="$(VERSION)"

#  the build target
TARGET := hexterm_havoc

#  Create Source list
SOURCES += $(wildcard src/*.c)
SOURCES += $(wildcard src/*/*.c)

#  Create list of objects based on .c files
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))

#  Create a list of dependencies based on .c files
DEPENDS := $(SOURCES:.c=.d)

#  Top Level Build Rule
all: ${TARGET}

#  Generic rule for dependencies
%.d: %.c
	$(CC) -M $(CFLAGS) -MQ '$*.o' $< | sed 's|$*\.o[ :]*|&$@ |g' > $@

-include $(DEPENDS)

#  Target build based on objects
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

#  Clean instruction
clean: 
	$(RM) -rf $(TARGET) $(OBJECTS) $(DEPENDS) *.pgm *~ *.dSYM/ logs/

#  More info for debugging
help: printvars helpsummary

#  Mark build rules as phony rules (they don't generate any files)
.PHONY: printvars clean helpsummary help

helpsummary:
	@echo "TARGET  : $(TARGET)"
	@echo "VERSION : $(VERSION)"
	@echo "SOURCES : $(SOURCES)"
	@echo "OBJECTS : $(OBJECTS)"
	@echo "DEPENDS : $(DEPENDS)"
	@echo "CFLAGS  : $(CFLAGS)"

printvars:
	@$(foreach V,$(sort $(.VARIABLES)), \
	$(if $(filter-out environment% default automatic, \
	$(origin $V)),$(warning $V=$($V) ($(value $V)))))

