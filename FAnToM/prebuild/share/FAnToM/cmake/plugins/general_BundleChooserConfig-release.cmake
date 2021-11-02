#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "general_BundleChooser" for configuration "Release"
set_property(TARGET general_BundleChooser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(general_BundleChooser PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/fantom-plugins/general/libBundleChooser.so"
  IMPORTED_SONAME_RELEASE "libBundleChooser.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS general_BundleChooser )
list(APPEND _IMPORT_CHECK_FILES_FOR_general_BundleChooser "${_IMPORT_PREFIX}/lib/fantom-plugins/general/libBundleChooser.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
