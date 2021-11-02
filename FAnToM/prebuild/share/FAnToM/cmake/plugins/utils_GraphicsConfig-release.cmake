#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "utils_Graphics" for configuration "Release"
set_property(TARGET utils_Graphics APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(utils_Graphics PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/fantom-plugins/utils/libGraphics.so"
  IMPORTED_SONAME_RELEASE "libGraphics.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS utils_Graphics )
list(APPEND _IMPORT_CHECK_FILES_FOR_utils_Graphics "${_IMPORT_PREFIX}/lib/fantom-plugins/utils/libGraphics.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
