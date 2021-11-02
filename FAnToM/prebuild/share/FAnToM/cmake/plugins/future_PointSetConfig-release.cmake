#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "future_PointSet" for configuration "Release"
set_property(TARGET future_PointSet APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(future_PointSet PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/fantom-plugins/future/libPointSet.so"
  IMPORTED_SONAME_RELEASE "libPointSet.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS future_PointSet )
list(APPEND _IMPORT_CHECK_FILES_FOR_future_PointSet "${_IMPORT_PREFIX}/lib/fantom-plugins/future/libPointSet.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
