cmake_minimum_required(VERSION 3.24)
project(midi_synth C)

set(CMAKE_C_STANDARD 11)
SET(GCC_COVERAGE_LINK_FLAGS    "-lrtmidi")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
add_executable(midi_synth main.c intersynth.h intersynth.c floating_fix.h floating_fix.c messages.c messages.h)