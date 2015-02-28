#######################################################
# Configure this.
################################################# # # #

# Targets (.c files have to exist)

OBJS = main tables audioio voices
#OBJS = test signal array audioio


# Libraries

LIBS := mingw32 SDLmain SDL


# Machine dep. options
GCC_MACHINEFLAGS=


# C flags
CFLAGS=-g -std=c99 -Wall -I./include

CPPFLAGS=
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

obj/%.o: src/%.c
	gcc $(GCC_MACHINEFLAGS) $(CFLAGS) -c -o $@ $<

test:
	echo CFLAGS=\"$(CFLAGS)\"

obj/main.o: src/main.c src/song.c
		gcc $(GCC_MACHINEFLAGS) $(CFLAGS) -c -o $@ $<


main: $(OBJFILES)
#	echo LINKING: $^
	gcc $(GCC_MACHINEFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBFLAGS)

runmain: main
	./main

clean:
	-@rm $(OBJFILES) >/dev/null 2>&1
	-@rm main >/dev/null 2>&1
	-@rm main.exe >/dev/null 2>&1
