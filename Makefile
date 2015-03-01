#######################################################
# Configure this.
################################################# # # #

# Targets (.c files have to exist)

#OBJS = main tables audioio voices
#OBJS = test signal array audioio
OBJS = main platform tables voices

# Libraries

LIBS := mingw32 SDL2main SDL2


# Machine dep. options
GCC_MACHINEFLAGS=


# C flags
CFLAGS=-g -std=c99 -Wall -static-libgcc -I./include

CPPFLAGS=-g -std=gnu++11 -static-libgcc -static-libstdc++ -Wall -I./include
CXXFLAGS=$(CPPFLAGS)

LDFLAGS=-L./lib
ASFLAGS=

#######################################################
# Don't change this
################################################# # # #

LIBFLAGS := $(addprefix -l,$(LIBS))
OBJFILES := $(patsubst %,obj/%.o,$(OBJS))


#######################################################
# Targets
################################################# # # #

.PHONY: test runmain clean

default: main

obj/%.o: src/%.cpp
	g++ $(GCC_MACHINEFLAGS) $(CPPFLAGS) -c -o $@ $<

test:
	@echo CFLAGS=\"$(CFLAGS)\"
	@echo CPPFLAGS=\"$(CPPFLAGS)\"

#obj/main.o: src/main.c src/song.c
#		gcc $(GCC_MACHINEFLAGS) $(CFLAGS) -c -o $@ $<

main: $(OBJFILES)
#	echo LINKING: $^
	g++ $(GCC_MACHINEFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LIBFLAGS)

runmain: main
	./main

clean:
	-@rm $(OBJFILES) >/dev/null 2>&1
	-@rm main >/dev/null 2>&1
	-@rm main.exe >/dev/null 2>&1
