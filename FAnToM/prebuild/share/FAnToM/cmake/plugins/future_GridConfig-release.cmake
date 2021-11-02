#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "future_Grid" for configuration "Release"
set_property(TARGET future_Grid APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(future_Grid PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/fantom-plugins/future/libGrid.so"
  IMPORTED_SONAME_RELEASE "libGrid.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS future_Grid )
list(APPEND _IMPORT_CHECK_FILES_FOR_future_Grid "${_IMPORT_PREFIX}/lib/fantom-plugins/future/libGrid.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
