#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "general_DatasetHelpers" for configuration "Release"
set_property(TARGET general_DatasetHelpers APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(general_DatasetHelpers PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/fantom-plugins/general/libDatasetHelpers.so"
  IMPORTED_SONAME_RELEASE "libDatasetHelpers.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS general_DatasetHelpers )
list(APPEND _IMPORT_CHECK_FILES_FOR_general_DatasetHelpers "${_IMPORT_PREFIX}/lib/fantom-plugins/general/libDatasetHelpers.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
