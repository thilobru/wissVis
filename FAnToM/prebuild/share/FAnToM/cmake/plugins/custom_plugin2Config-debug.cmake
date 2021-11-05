#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "custom_plugin2" for configuration "Debug"
set_property(TARGET custom_plugin2 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(custom_plugin2 PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/../prebuild/lib/fantom-plugins/custom/libplugin2.so"
  IMPORTED_SONAME_DEBUG "libplugin2.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS custom_plugin2 )
list(APPEND _IMPORT_CHECK_FILES_FOR_custom_plugin2 "${_IMPORT_PREFIX}/../prebuild/lib/fantom-plugins/custom/libplugin2.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
