#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "utils_math" for configuration "Release"
set_property(TARGET utils_math APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(utils_math PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/fantom-plugins/utils/libmath.so"
  IMPORTED_SONAME_RELEASE "libmath.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS utils_math )
list(APPEND _IMPORT_CHECK_FILES_FOR_utils_math "${_IMPORT_PREFIX}/lib/fantom-plugins/utils/libmath.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
