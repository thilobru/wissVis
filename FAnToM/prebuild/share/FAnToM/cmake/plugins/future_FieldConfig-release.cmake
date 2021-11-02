#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "future_Field" for configuration "Release"
set_property(TARGET future_Field APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(future_Field PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/fantom-plugins/future/libField.so"
  IMPORTED_SONAME_RELEASE "libField.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS future_Field )
list(APPEND _IMPORT_CHECK_FILES_FOR_future_Field "${_IMPORT_PREFIX}/lib/fantom-plugins/future/libField.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
