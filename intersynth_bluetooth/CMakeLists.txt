cmake_minimum_required(VERSION 3.24)
project(intersynth_bluetooth C)

set(CMAKE_C_STANDARD 11)

add_library(intersynth_bluetooth src/intersynth.c include/intersynth.h src/bluetooth.c include/bluetooth.h src/messages.c include/messages.h src/error.c include/error.h)
target_link_libraries(intersynth_bluetooth ws2_32)
add_executable(intersynth_bluetooth_runner main.c)
target_link_libraries(intersynth_bluetooth_runner intersynth_bluetooth)