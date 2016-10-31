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

#  the compiler: using gcc for C, g++ for C++
CC = gcc
CXX = g++

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

#  the build target (for C)
CTARGET := hexterm_havoc_c

#  the build target (for CXX)
CXXTARGET := hexterm_havoc

#  Create C Source list
CSOURCES += $(wildcard src/*.c)
CSOURCES += $(wildcard src/*/*.c)

#  Create C++ Source list
CXXSOURCES += $(wildcard src/*.cpp)
CXXSOURCES += $(wildcard src/*/*.cpp)
CXXCSOURCES := $(filter-out $(patsubst %.cpp,%.c,$(CXXSOURCES)),$(CSOURCES))

#  Create list of dependencies
CDEPENDS := $(patsubst %.c,%.d,$(CSOURCES))
CXXDEPENDS := $(patsubst %.cpp,%.d,$(CXXSOURCES))

#  Create list of objects based on .c files
COBJECTS := $(patsubst %.c,%.o,$(CSOURCES))
CXXOBJECTS := $(patsubst %.cpp,%.opp,$(CXXSOURCES))
CXXCOBJECTS := $(patsubst %.c,%.oc,$(CXXCSOURCES))

#  Top Level Build Rule
all: cxx
	@$(ECHO) Done!

cxx: ${CXXTARGET}
	@$(ECHO) C++ Target Built!

-include $(CXXDEPENDS)

#  Generic rule for C++ dependencies
%.opp: %.cpp
	@$(ECHO) Building $<...
	@$(CXX) $(CFLAGS) -MMD -MF '$*.d' -c $< -o $(patsubst %.cpp,%.opp,$<)

$(CXXTARGET): $(CXXOBJECTS) $(CXXCOBJECTS)
	@$(ECHO) Linking $@
	@$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#  C Target Build Rule
c: ${CTARGET}
	@$(ECHO) C Target Built!

-include $(CDEPENDS)

#  Generic rule for C dependencies
%.o: %.c
	@$(ECHO) Building $<...
	@$(CC) $(CFLAGS) -MMD -MF '$*.d' -c $< -o $(patsubst %.c,%.o,$<)

#  Generic Rule for building C sources using CXX compiler
%.oc: %.c
	@$(ECHO) Building with cxx $<...
	@$(CXX) $(CFLAGS) -MMD -MF '$*.d' -c -x c++ $< -o $(patsubst %.c,%.oc,$<)

#  Target build based on objects
$(CTARGET): $(COBJECTS)
	@$(ECHO) Linking $@...
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#  Clean instruction
clean: 
	@$(ECHO) Cleaning...
	@$(RM) -rf $(CXXTARGET) $(CTARGET) $(COBJECTS) $(CXXOBJECTS) $(CXXCOBJECTS) $(CXXDEPENDS) $(CDEPENDS) *.swp *.tar.gz *.pgm *~ *.dSYM/ logs/
	@$(ECHO) Done!

#  More info for debugging
help: printvars helpsummary

#  Mark build rules as phony rules (they don't generate any files)
.PHONY: printvars clean helpsummary help submit

helpsummary:
	@echo "TARGET      : $(CXXTARGET)"
	@echo "VERSION     : $(VERSION)"
	@echo "CTARGET     : $(CTARGET)"
	@echo "CSOURCES    : $(CSOURCES)"
	@echo "CXXSOURCES  : $(CXXSOURCES)"
	@echo "CXXCSOURCES : $(CXXCSOURCES)"
	@echo "COBJECTS    : $(COBJECTS)"
	@echo "CDEPENDS    : $(CDEPENDS)"
	@echo "CFLAGS      : $(CFLAGS)"

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
	
