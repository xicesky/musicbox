#######################################################
# Configure this.
################################################# # # #

# Targets (.c files have to exist)

#OBJS = main tables audioio voices
#OBJS = test signal array audioio
OBJS = main platform tables voices

# Libraries
LIBS := mingw32 SDL2main SDL2

EXECNAME := main
DEBUG_SUFFIX := _dbg

# TARGET: Mingw 64 or 32 bit?
#TARGET := i686-w64-mingw32
#TARGET := x86_64-w64-mingw32

#######################################################
# Don't change this
################################################# # # #

OBJDIR := local/obj
BINDIR := bin
LIBDIR := lib
INCLUDEDIR := include

EXECNAME_DBG := $(EXECNAME)$(DEBUG_SUFFIX)
EXECNAME_RLS := $(EXECNAME)

EXECUTABLE_SUFFIX := .exe
SO_SUFFIX := .dll

OBJFILES_DBG := $(patsubst %,$(OBJDIR)/dbg/%.o,$(OBJS))
OBJFILES_RLS := $(patsubst %,$(OBJDIR)/rls/%.o,$(OBJS))

LIBFLAGS := $(addprefix -l,$(LIBS))

# OS detection and info
OSNAME := $(shell uname -s)
#$(info $$OS      is [$(OS)])
#$(info $$OSNAME  is [$(OSNAME)])

ifeq ($(OS),Windows_NT)
    ifneq (,$(findstring CYGWIN,$(OSNAME)))
        # Detected cygwin
        MY_OS := cygwin
    else
        MY_OS := winnt
    endif
else
    MY_OS := linux
endif
$(info Detected OS: $(MY_OS))

# Select compiler etc based on target
ifeq ($(TARGET),)
    ifeq ($(CXX),)
        TARGET := $(shell gcc -dumpmachine)
    else
        #TARGET := $(shell $(CXX) -v 2>&1 | grep "^Target" | cut -d" " -f2)
        TARGET := $(shell $(CXX) -dumpmachine)
    endif
else
    CC:=$(TARGET)-gcc
    CXX:=$(TARGET)-g++
endif

ifeq ($(CC),)
    CC:=$(TARGET)-gcc
    # TODO: Check for mingw w64
endif
CC:=$(shell which $(CC))

ifeq ($(CXX),)
    CXX:=$(TARGET)-g++
    # TODO: Check for mingw w64
endif
CXX:=$(shell which $(CXX))

REAL_LIBDIR := $(LIBDIR)
ifneq ($(TARGET),)
    REAL_LIBDIR := $(REAL_LIBDIR)/$(TARGET)
else
endif

# Machine dep. options
GCC_MACHINEFLAGS=

# Tool flags
INCLUDEFLAGS=-I./include

CFLAGS=-std=c99 -Wall $(INCLUDEFLAGS)
CFLAGS_DBG=-g
CFLAGS_RLS=-static-libgcc -Os -s

CPPFLAGS=-g -std=gnu++11 -Wall $(INCLUDEFLAGS)
CPPFLAGS_DBG=-g
CPPFLAGS_RLS=-static-libgcc -static-libstdc++ -Os -s

CXXFLAGS=$(CPPFLAGS)
CXXFLAGS_DBG=$(CPPFLAGS_DBG)
CXXFLAGS_RLS=$(CPPFLAGS_RLS)

LDFLAGS=-L./$(REAL_LIBDIR)
ASFLAGS=

# How would this work on unix?
SEARCH_SO_DIR := $(abspath $(dir $(shell which gcc)))

#######################################################
# Targets
################################################# # # #

.PHONY: default debug release runmain clean info copydeps

default: debug

debug: $(BINDIR)/$(EXECNAME_DBG)

release: $(BINDIR)/$(EXECNAME_RLS)

$(OBJDIR) $(OBJDIR)/dbg $(OBJDIR)/rls:
		mkdir -p $@

$(OBJDIR)/dbg/%.o: src/%.cpp | $(OBJDIR)/dbg
		$(CXX) $(GCC_MACHINEFLAGS) $(CPPFLAGS) $(CPPFLAGS_DBG) -c -o $@ $<

$(OBJDIR)/rls/%.o: src/%.cpp | $(OBJDIR)/rls
		$(CXX) $(GCC_MACHINEFLAGS) $(CPPFLAGS) $(CPPFLAGS_RLS) -c -o $@ $<

info:
		@echo "--------------------------------------------------------------------------------"
		@echo "Compiler"
		@echo "    TARGET          : $(TARGET)"
		@echo "    CC              : $(CC)"
		@echo "    CPP             : $(CPP)"
		@echo "    CXX             : $(CXX)"
		@echo "--------------------------------------------------------------------------------"
		@echo "Flags"
		@echo "    CFLAGS          : $(CFLAGS)"
		@echo "    CFLAGS_DBG      : $(CFLAGS_DBG)"
		@echo "    CPPFLAGS        : $(CPPFLAGS)"
		@echo "    CPPFLAGS_DBG    : $(CPPFLAGS_DBG)"
		@echo "--------------------------------------------------------------------------------"
		@echo "Directories"
		@echo "    REAL_LIBDIR     : $(REAL_LIBDIR)"
		@echo "    SEARCH_SO_DIR   : $(SEARCH_SO_DIR)"
		@echo "--------------------------------------------------------------------------------"
		@echo "Problems"
		@[ -x "$(CC)" ] || \
		 echo "    C Compiler not found    : $(CC)"
		@[ -x "$(CXX)" ] || \
		 echo "    C++ Compiler not found  : $(CXX)"
		@[ -d "$(REAL_LIBDIR)" ] || \
		 echo "    Missing bundled libs    : $(REAL_LIBDIR)"

#obj/main.o: src/main.c src/song.c
#        gcc $(GCC_MACHINEFLAGS) $(CFLAGS) -c -o $@ $<

$(BINDIR)/SDL2$(SO_SUFFIX): $(REAL_LIBDIR)/SDL2$(SO_SUFFIX)
		cp $< $@

$(BINDIR)/$(EXECNAME_DBG): $(OBJFILES_DBG) | $(BINDIR)/SDL2$(SO_SUFFIX)
		$(CXX) $(GCC_MACHINEFLAGS) $(CPPFLAGS) $(CPPFLAGS_DBG) $(LDFLAGS) -o $@ $^ $(LIBFLAGS)

$(BINDIR)/$(EXECNAME_RLS): $(OBJFILES_RLS) | $(BINDIR)/SDL2$(SO_SUFFIX)
		$(CXX) $(GCC_MACHINEFLAGS) $(CPPFLAGS) $(CPPFLAGS_RLS) $(LDFLAGS) -o $@ $^ $(LIBFLAGS)

# Copy shared libraries into bin dir, e.g. for MinGW
# XXX: How can we find which libs are required?
$(BINDIR)/%$(SO_SUFFIX): $(SEARCH_SO_DIR)/%$(SO_SUFFIX)
		cp $< $@

copydeps: $(BINDIR)/libgcc_s_dw2-1$(SO_SUFFIX) $(BINDIR)/libstdc++-6$(SO_SUFFIX)

runmain: $(BINDIR)/main_dbg
		./$(BINDIR)/main_dbg

clean:
		-@rm $(OBJFILES_DBG) >/dev/null 2>&1
		-@rm $(OBJFILES_RLS) >/dev/null 2>&1
		-@rm $(BINDIR)/SDL2$(SO_SUFFIX) >/dev/null 2>&1
		-@rm $(BINDIR)/$(EXECNAME_DBG)$(EXECUTABLE_SUFFIX) >/dev/null 2>&1
		-@rm $(BINDIR)/$(EXECNAME_RLS)$(EXECUTABLE_SUFFIX) >/dev/null 2>&1
