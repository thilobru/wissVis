#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "future_VTK" for configuration "Release"
set_property(TARGET future_VTK APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(future_VTK PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/fantom-plugins/future/libVTK.so"
  IMPORTED_SONAME_RELEASE "libVTK.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS future_VTK )
list(APPEND _IMPORT_CHECK_FILES_FOR_future_VTK "${_IMPORT_PREFIX}/lib/fantom-plugins/future/libVTK.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
