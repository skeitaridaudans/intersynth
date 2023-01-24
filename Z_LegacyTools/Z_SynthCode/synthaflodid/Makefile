#
# Synthaflóðið Makefile
#
# Dominic Mazzoni
# Modifications by Mikael Magnusson
# Modifications by Stelios Bounanos
#
# Modification by Kári Halldórsson
#

# THIS CAN CHANGE BETWEEN PROJECTS    !!!
portaudiodir = ../../portaudio
rtmididir = ../../rtmidi
freeglutdir = ../../freeglut

CC = gcc
CXX = 
INCLUDES = -I$(portaudiodir)/include -I$(rtmididir)/include -I$(freeglutdir)/include
CFLAGS = -g -lstdc++ -std=c++14 -O2 -D__WINDOWS_MM__ -D__WINDOWS_WINKS__ -DPA_LITTLE_ENDIAN $(INCLUDES) -DPA_USE_WMME=0 -DPA_USE_ASIO=1 -DPA_USE_WDMKS=0 -DPA_USE_DS=0 -DPA_USE_WASAPI=0 -UPA_USE_WMME -mthreads -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DLT_OBJDIR=\".libs/\" -DSIZEOF_SHORT=2 -DSIZEOF_INT=4 -DSIZEOF_LONG=4 -DHAVE_NANOSLEEP=1
LIBS =  -L"$(rtmididir)/lib" -lrtmidi -L"$(portaudiodir)/lib" -lportaudio -L"$(freeglutdir)/lib" -lfreeglut -lglu32 -lopengl32 -lwinmm -lm -lole32 -luuid -lsetupapi -lksuser
SHELL = /bin/sh
LIBTOOL = $(SHELL) ./libtool
CXXFLAGS = 
NASM = 
NASMOPT = 

COMMON_H = ./synthar/helpers_midi.h ./synthar/helpers_graphics.h
COMMON_CPP = ./synthar/helpers_midi.cpp ./synthar/helpers_graphics.cpp

DAUDA_H = ./synthar/patch.h ./synthar/notes.h ./synthar/daudasynthinn_ui.h ./synthar/daudasynthinn_music.h
DAUDA_CPP = ./synthar/daudasynthinn.cpp ./synthar/patch.cpp ./synthar/notes.cpp ./synthar/daudasynthinn_ui.cpp ./synthar/daudasynthinn_music.cpp

#PALIB = $(portaudiodir)/lib/libportaudio.la
PAINC = $(portaudiodir)/include/portaudio.h

#RTLIB = $(rtmididir)/.libs/librtmidi.la
RTINC = $(rtmididir)/include/RtMidi.h

PA_EXAMPLES = \
	bin/paex_pink \
	bin/paex_saw \

RT_EXAMPLES = \
	bin/cmidiin \
	bin/midiprobe \

all: synthaflodid #examples

examples: $(PA_EXAMPLES) $(RT_EXAMPLES)

$(PA_EXAMPLES): bin/%: $(PALIB) $(PAINC) examples/%.c
	 $(LIBTOOL) --mode=link $(CC) -o $@ $(CFLAGS) ./examples/$*.c $(PALIB) $(LIBS)

$(RT_EXAMPLES): bin/%: $(RTLIB) $(RTINC) examples/%.cpp
	 $(LIBTOOL) --mode=link $(CC) -o $@ $(CFLAGS) ./examples/$*.cpp $(RTLIB) $(LIBS)

#  $(PALIB) $(PAINC) $(RTLIB) $(RTINC)

synthaflodid: synthlaus # daudasynthinn knocktunal

daudasynthinn: $(DAUDA_H) $(COMMON_H)
	$(LIBTOOL) --mode=link $(CC) -o ./bin/daudasynthinn $(CFLAGS) $(DAUDA_CPP) $(COMMON_CPP) $(LIBS)

synthlaus: $(COMMON_H)
	$(LIBTOOL) --mode=link $(CC) -o ./bin/synthlaus $(CFLAGS) ./synthar/synthlaus.cpp $(COMMON_CPP) $(LIBS)

knocktunal: $(COMMON_H)
	$(LIBTOOL) --mode=link $(CC) -o ./bin/knocktunal $(CFLAGS) ./synthar/knocktunal.cpp ./synthar/knocktunal_midi.cpp $(COMMON_CPP) $(LIBS)

# $(PALIB) $(RTLIB) 

clean: empty_bin move_dll

empty_bin:
	rm -r bin
	mkdir bin

move_dll:
	cp ./*.dll ./bin/


#	$(LIBTOOL) --mode=clean rm -f $(EXAMPLES)
