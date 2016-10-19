#  Makefile for Hexterminal Havoc
#  This is a generic makefile that has been modified for fit the source
#  structure of the project.
#
#  This makefile has been adapted to fit the project needs from the 
#  following source: https://gist.github.com/kbingham/4414922

#  Define the shell we are using
SHELL := /bin/bash -O extglob

#  Define echo for displaying messages
ECHO = echo

#  the compiler: using gcc for C
CC = gcc

#  debug flags:
#  -ggdb ~> add gdb debugging information to the binary
DFLAGS = -ggdb -pg

#  linker flags:
#  -lncurses ~> link the ncurses library to the binary
LDFLAGS = -lncurses

#  Attach a version number to each build
VERSION := $(shell git describe --tags --abbrev=0 2>/dev/null)

#  Create submission directory
SUBMIT_DIR := dhanwada_srinivas.assignment-$(VERSION)

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

#  Create list of dependencies
DEPENDS := $(patsubst %.c,%.d,$(SOURCES))

#  Create list of objects based on .c files
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))

#  Top Level Build Rule
all: ${TARGET}
	@$(ECHO) Done!

-include $(DEPENDS)

#  Generic rule for dependencies
%.o: %.c
	@$(ECHO) Building $<...
	@$(CC) $(CFLAGS) -MMD -MF '$*.d' -c $< -o $(patsubst %.c,%.o,$<)


#  Target build based on objects
$(TARGET): $(OBJECTS)
	@$(ECHO) Linking $@...
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#  Clean instruction
clean: 
	@$(ECHO) Cleaning...
	@$(RM) -rf $(TARGET) $(OBJECTS) $(DEPENDS) *.swp *.tar.gz *.pgm *~ *.dSYM/ logs/
	@$(ECHO) Done!

#  More info for debugging
help: printvars helpsummary

#  Mark build rules as phony rules (they don't generate any files)
.PHONY: printvars clean helpsummary help submit

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

submit: clean
	@echo "Building Submission for Assignment $(VERSION)"
	@rm -rf $(SUBMIT_DIR)
	@mkdir $(SUBMIT_DIR)
	@cp -R !(.git|$(SUBMIT_DIR)) $(SUBMIT_DIR)
	@tar zcf $(SUBMIT_DIR).tar.gz $(SUBMIT_DIR)
	@rm -rf $(SUBMIT_DIR)
	@echo "Submission $(SUBMIT_DIR).tar.gz built!"
	
