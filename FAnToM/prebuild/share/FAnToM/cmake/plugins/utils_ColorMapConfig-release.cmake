#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "utils_ColorMap" for configuration "Release"
set_property(TARGET utils_ColorMap APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(utils_ColorMap PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/fantom-plugins/utils/libColorMap.so"
  IMPORTED_SONAME_RELEASE "libColorMap.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS utils_ColorMap )
list(APPEND _IMPORT_CHECK_FILES_FOR_utils_ColorMap "${_IMPORT_PREFIX}/lib/fantom-plugins/utils/libColorMap.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
