#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "future_LineSet" for configuration "Release"
set_property(TARGET future_LineSet APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(future_LineSet PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/fantom-plugins/future/libLineSet.so"
  IMPORTED_SONAME_RELEASE "libLineSet.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS future_LineSet )
list(APPEND _IMPORT_CHECK_FILES_FOR_future_LineSet "${_IMPORT_PREFIX}/lib/fantom-plugins/future/libLineSet.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
