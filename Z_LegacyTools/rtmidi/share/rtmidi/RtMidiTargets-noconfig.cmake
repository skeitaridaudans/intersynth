#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "RtMidi::rtmidi" for configuration ""
set_property(TARGET RtMidi::rtmidi APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(RtMidi::rtmidi PROPERTIES
  IMPORTED_IMPLIB_NOCONFIG "${_IMPORT_PREFIX}/lib/librtmidi.dll.a"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/bin/librtmidi.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS RtMidi::rtmidi )
list(APPEND _IMPORT_CHECK_FILES_FOR_RtMidi::rtmidi "${_IMPORT_PREFIX}/lib/librtmidi.dll.a" "${_IMPORT_PREFIX}/bin/librtmidi.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
