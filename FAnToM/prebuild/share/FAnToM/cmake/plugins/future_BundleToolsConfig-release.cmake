#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "future_BundleTools" for configuration "Release"
set_property(TARGET future_BundleTools APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(future_BundleTools PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/fantom-plugins/future/libBundleTools.so"
  IMPORTED_SONAME_RELEASE "libBundleTools.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS future_BundleTools )
list(APPEND _IMPORT_CHECK_FILES_FOR_future_BundleTools "${_IMPORT_PREFIX}/lib/fantom-plugins/future/libBundleTools.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
