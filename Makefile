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

LDFLAGS=-L./lib
ASFLAGS=

#######################################################
# Targets
################################################# # # #

.PHONY: default debug release test runmain clean

default: debug

debug: $(BINDIR)/$(EXECNAME_DBG)

release: $(BINDIR)/$(EXECNAME_RLS)

$(OBJDIR) $(OBJDIR)/dbg $(OBJDIR)/rls:
	mkdir -p $@

$(OBJDIR)/dbg/%.o: src/%.cpp | $(OBJDIR)/dbg
	g++ $(GCC_MACHINEFLAGS) $(CPPFLAGS) $(CPPFLAGS_DBG) -c -o $@ $<

$(OBJDIR)/rls/%.o: src/%.cpp | $(OBJDIR)/rls
	g++ $(GCC_MACHINEFLAGS) $(CPPFLAGS) $(CPPFLAGS_RLS) -c -o $@ $<

test:
	@echo CFLAGS=\"$(CFLAGS)\"
	@echo CFLAGS_DBG=\"$(CFLAGS_DBG)\"
	@echo CPPFLAGS=\"$(CPPFLAGS)\"
	@echo CPPFLAGS_DBG=\"$(CPPFLAGS_DBG)\"

#obj/main.o: src/main.c src/song.c
#		gcc $(GCC_MACHINEFLAGS) $(CFLAGS) -c -o $@ $<

$(BINDIR)/SDL2$(SO_SUFFIX): $(LIBDIR)/SDL2$(SO_SUFFIX)
	cp $< $@

$(BINDIR)/$(EXECNAME_DBG): $(OBJFILES_DBG) | $(BINDIR)/SDL2$(SO_SUFFIX)
#	echo LINKING: $^
	g++ $(GCC_MACHINEFLAGS) $(CPPFLAGS) $(CPPFLAGS_DBG) $(LDFLAGS) -o $@ $^ $(LIBFLAGS)

$(BINDIR)/$(EXECNAME_RLS): $(OBJFILES_RLS) | $(BINDIR)/SDL2$(SO_SUFFIX)
#	echo LINKING: $^
	g++ $(GCC_MACHINEFLAGS) $(CPPFLAGS) $(CPPFLAGS_RLS) $(LDFLAGS) -o $@ $^ $(LIBFLAGS)

runmain: $(BINDIR)/main_dbg
	./$(BINDIR)/main_dbg

clean:
	-@rm $(OBJFILES_DBG) >/dev/null 2>&1
	-@rm $(OBJFILES_RLS) >/dev/null 2>&1
	-@rm $(BINDIR)/SDL2$(SO_SUFFIX) >/dev/null 2>&1
	-@rm $(BINDIR)/$(EXECNAME_DBG)$(EXECUTABLE_SUFFIX) >/dev/null 2>&1
	-@rm $(BINDIR)/$(EXECNAME_RLS)$(EXECUTABLE_SUFFIX) >/dev/null 2>&1
