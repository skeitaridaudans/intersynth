cmake_minimum_required(VERSION 3.24)
project(intersynth_bluetooth C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(platform)

target_include_directories(intersynth_bluetooth PUBLIC include)
target_include_directories(intersynth_bluetooth PUBLIC src)

file(GLOB SOURCES "src/*.c")
file(GLOB HEADERS "include/*.h")

# Set platform-specific sources based on the current platform
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    file(GLOB PLATFORM_SOURCES "platform/windows/*.c")
    file(GLOB PLATFORM_HEADERS "platform/windows/*.h")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    file(GLOB PLATFORM_SOURCES "platform/linux/*.c")
    file(GLOB PLATFORM_HEADERS "platform/linux/*.h")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    file(GLOB PLATFORM_SOURCES "platform/darwin/*.c")
    file(GLOB PLATFORM_HEADERS "platform/darwin/*.h")
endif()

# Add the platform-specific sources to the list of sources
list(APPEND SOURCES ${PLATFORM_SOURCES})
list(APPEND HEADERS ${PLATFORM_HEADERS})

# Add the library
add_library(intersynth_bluetooth ${SOURCES} ${HEADERS})

# Link the Bluetooth library
if(WIN32)
    target_link_libraries(intersynth_bluetooth ws2_32 Bthprops)
elseif(UNIX)
    target_link_libraries(intersynth_bluetooth bluetooth)
elseif(APPLE)
    target_link_libraries(intersynth_bluetooth "-framework CoreBluetooth"
                                                "-framework Foundation"
                                                "-framework CoreFoundation"
        objc)
endif()

add_executable(intersynth_bluetooth_runner main.c)
target_link_libraries(intersynth_bluetooth_runner intersynth_bluetooth)