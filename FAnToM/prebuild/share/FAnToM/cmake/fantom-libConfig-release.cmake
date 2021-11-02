#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "fantom-lib" for configuration "Release"
set_property(TARGET fantom-lib APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(fantom-lib PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libfantom-lib.so"
  IMPORTED_SONAME_RELEASE "libfantom-lib.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS fantom-lib )
list(APPEND _IMPORT_CHECK_FILES_FOR_fantom-lib "${_IMPORT_PREFIX}/lib/libfantom-lib.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
