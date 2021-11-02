#
# \defgroup cmake-macros CMake FAnToM macros tutorial
# @file FAnToMMacros.cmake
# @brief Set up all required paths to compile external FAnToM modules
#
# This file is part of the FAnToM build system
# It defines all required functions to declare FAnToM plugins in out-of-tree builds.
#
# Required global variables:
#   - FANTOM_DIR - installation directory of FAnToM (e.g. /usr/local, /usr, /local/install/dir)
#                  The system expects ${FANTOM_DIR}/share/FAnToM/cmake to contain the cmake configs
#                  of all plugins and ${FANTOM_DIR}/lib/fantom-plugins to contain the plugin libraries.
#
# Defined functions:
# function( FANTOM_ADD_PLUGIN PLUGIN )
# function( FANTOM_ADD_PLUGIN_DIRECTORY PLUGIN )
#
# For usage, just call FANTOM_ADD_PLUGIN_DIRECTORY on a plugin directory.
# Note that the plugin must be located at ${CMAKE_SOURCE_DIR}!
# If a plugin needs custom configuration, put a CMakeLists.txt inside the plugin directory
# that discovers needed libraries (as imported targets!) and puts them into the ${PLUGIN}_LIBS variable.
# Then call FANTOM_ADD_PLUGIN( ${PLUGIN} ) to add it to the build system.
# On install, the plugin will be merged into the FANTOM_DIR infrastructure.
#
# By default, the used toolbox name is "custom".
# A different toolbox name can be specified by setting FANTOM_TOOLBOX_NAME to a different value.


if( NOT DEFINED FANTOM_TOOLBOX_NAME )
    set( FANTOM_TOOLBOX_NAME "custom" )
endif()
include( FAnToMDefaultLibs )
include( fantom-libConfig )
list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/plugins )


# This function scans the module dir for a FAnToM-Dependencies.txt and returns the dependent modules
# in the variable ${TOOLBOX}_DEPENDENCIES
function( FANTOM_GET_DEPENDENCIES RESULT PLUGINPATH )
  # search for dependencies in current plugin
  IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGINPATH}/FAnToM-Dependencies.txt)
    FILE(READ ${PLUGINPATH}/FAnToM-Dependencies.txt dep_toolboxes)
    STRING(REGEX REPLACE "(^|\n)[ \t]*#[^\n]*(\n|$)" "\\1" dep_toolboxes "${dep_toolboxes}")
    STRING(REGEX REPLACE ";" "\\\\;" dep_toolboxes "${dep_toolboxes}")
    STRING(REGEX REPLACE "\n" ";" dep_toolboxes "${dep_toolboxes}")
    set( ${RESULT} ${dep_toolboxes} PARENT_SCOPE )
  ELSE(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGINPATH}/FAnToM-Dependencies.txt)
    set( ${RESULT} "" PARENT_SCOPE )
  ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGINPATH}/FAnToM-Dependencies.txt)
endfunction( FANTOM_GET_DEPENDENCIES )

# This function adds an out-of-tree plugin to the build.
# - It takes care of setting all linker setting according to what FAnToM needs.
# - It uses some global variables that have to be defined in the scope that calls
# the function to prepare the binary.
# global variables used:
# ${${PLUGIN}_LIBS}         the external (imported!) libraries the code has to link to
#
# - It scans the directory for a file called FAnToM-Dependencies.txt, which contains a list of
# dependencies that are either FAnToM toolboxes or external libraries.
# The dependent libraries are added such as "extra/ColorMap" (without the quotes) and libraries
# FAnToM uses internally can be added using the fake path name DEFAULT, e.g., "DEFAULT/OpenGL"
#
# Plugins should adhere to the following source layout:
# Plugin_dir
# - CMakeLists.txt (optional)
# - FAnToM-Dependencies.txt (optional)
# - include/   -> all headers for the public interface (discoverable in other plugins as <fantom-plugins/toolbox/plugin/...>
# - src/       -> all sources needed for implementation of the public interface
# - algos/     -> sources that contain the fantom::AlgorithmS for this plugin
# - resources  -> resources that are installed in share/FAnToM/toolbox/plugin/...
# - test       -> sources for integration tests and FAnToM Session files
#
# All of src, algos, test, include and resources are optional.
# In the special case, that you only need algos/, the content that would otherwise be
# in the algos directory can be put top-level instead.
# In the test-directory you can expect the catch framework to be available via <catch/catch.hpp>.
#
# If you have a CMakeLists.txt file, it should contain something like the following:
#    find_package( externalLib )
#    # add_library( externalLib INTERFACE IMPORTED )
#    set( PluginName_LIBS externalLib )
#    FANTOM_ADD_PLUGIN( PluginName )
#
# The Variable FANTOM_TOOLBOX_NAME has to be set beforehand.
#
function( FANTOM_ADD_PLUGIN PLUGIN )
  GET_FILENAME_COMPONENT(_fullPath ${CMAKE_SOURCE_DIR}/${PLUGIN} ABSOLUTE)

  if( EXISTS ${_fullPath}/src OR EXISTS ${_fullPath}/algos OR EXISTS ${_fullPath}/test OR EXISTS ${_fullPath}/include OR EXISTS ${_fullPath}/resources )
    file( GLOB_RECURSE SOURCES ${_fullPath}/src/*.cpp ${_fullPath}/src/*.hpp ${_fullPath}/include/*.hpp ${_fullPath}/algos/*.cpp ${_fullPath}/algos/*.hpp )
  else()
    file( GLOB_RECURSE SOURCES ${_fullPath}/*.cpp ${_fullPath}/*.hpp)
  endif()

  # symlink resources and includes
  execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/share/FAnToM/${TOOLBOX} )
  execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink ${_fullPath}/resources ${CMAKE_BINARY_DIR}/share/FAnToM/${TOOLBOX}/${PLUGIN} )
  execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${TOOLBOX}/${PLUGIN}/fantom-plugins/${TOOLBOX} )
  execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink ${_fullPath}/include ${CMAKE_BINARY_DIR}/include/${TOOLBOX}/${PLUGIN}/fantom-plugins/${TOOLBOX}/${PLUGIN} )

  # add library
  add_library( ${FANTOM_TOOLBOX_NAME}_${PLUGIN} SHARED ${SOURCES} )
  target_compile_options( ${FANTOM_TOOLBOX_NAME}_${PLUGIN} PRIVATE ${BUILD_CXX_DIALECT} )
  target_include_directories( ${FANTOM_TOOLBOX_NAME}_${PLUGIN} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include/${FANTOM_TOOLBOX_NAME}/${PLUGIN}/>
    $<INSTALL_INTERFACE:include>
    PRIVATE ${_fullPath}/src ${_fullPath}/include )
  # minic FS layout of install dir in build dir
  set_target_properties( ${FANTOM_TOOLBOX_NAME}_${PLUGIN} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/fantom-plugins/${FANTOM_TOOLBOX_NAME}/
    OUTPUT_NAME ${PLUGIN}
    LINKER_LANGUAGE CXX
    )

  # add dependencies to library
  target_link_libraries( ${FANTOM_TOOLBOX_NAME}_${PLUGIN} PUBLIC fantom-lib )
  target_link_libraries( ${FANTOM_TOOLBOX_NAME}_${PLUGIN} PUBLIC ${${PLUGIN}_LIBS} )
  if( BUILD_USE_OPENMP )
    target_link_libraries( ${FANTOM_TOOLBOX_NAME}_${PLUGIN} PRIVATE Threads::OpenMP )
  else()
    target_link_libraries( ${FANTOM_TOOLBOX_NAME}_${PLUGIN} PRIVATE Threads::Threads )
  endif()
  FANTOM_GET_DEPENDENCIES( DEPS ${PLUGIN} )
  foreach( DEPENDENT_PLUGIN ${DEPS} )
    IF( ${DEPENDENT_PLUGIN} MATCHES "^DEFAULT/.+" )
      STRING( REGEX REPLACE ".*/" "" external_lib ${DEPENDENT_PLUGIN} )
      # we provide some libraries against which FAnToM links anyways.
      # These can be added in the dependencies as DEFAULT/LibraryName
      # where LibraryName is one of:
      # - OpenGL
      # - Python
      # - Eigen3
      #
      # In terms of code design, we could use variables such as
      # ${external_lib}_FANTOM_INCLUDE_PATH etc to remove the code
      # here and do a more general setup of these libraries
      # On the other hand, we lose the flexibility to create more
      # complex code for linking those libraries if that is required.
      if( ${external_lib} STREQUAL "Python" )
        target_link_libraries( ${TOOLBOX}_${PLUGIN} PUBLIC Boost Python )
        target_link_libraries( ${TOOLBOX}_${PLUGIN} PRIVATE pybind11 )
      elseif( ${external_lib} STREQUAL "OpenGL" )
        target_link_libraries( ${TOOLBOX}_${PLUGIN} PUBLIC OpenGL )
      elseif( ${external_lib} STREQUAL "Eigen3" )
        target_link_libraries( ${TOOLBOX}_${PLUGIN} PUBLIC Eigen3 )
      else()
         #message( STATUS "!! External library ${external_lib} is not in the default libraries. Entry will be ignored." )
      endif()
    else()
      STRING(REGEX REPLACE "(.*)/.+" "\\1" DEPENDENT_TOOLBOX "${DEPENDENT_PLUGIN}")
      STRING(REGEX REPLACE ".*/" "" DEPENDENT_PLUGIN "${DEPENDENT_PLUGIN}")
      include( ${DEPENDENT_TOOLBOX}_${DEPENDENT_PLUGIN}Config )
      target_link_libraries( ${FANTOM_TOOLBOX_NAME}_${PLUGIN} PUBLIC ${DEPENDENT_TOOLBOX}_${DEPENDENT_PLUGIN})
    endif()
  endforeach()
  export( TARGETS ${FANTOM_TOOLBOX_NAME}_${PLUGIN} FILE share/FAnToM/cmake/plugins/${FANTOM_TOOLBOX_NAME}_${PLUGIN}Config.cmake )

  # copy the dependencies files
  IF(EXISTS ${_fullPath}/FAnToM-Dependencies.txt)
    configure_file( ${_fullPath}/FAnToM-Dependencies.txt ${CMAKE_BINARY_DIR}/lib/fantom-plugins/${FANTOM_TOOLBOX_NAME}/lib${PLUGIN}.dependency COPYONLY )
  ENDIF(EXISTS ${_fullPath}/FAnToM-Dependencies.txt)

  # implement tests
  if( EXISTS ${_fullPath}/test )
    file( GLOB_RECURSE TEST_SOURCES ${_fullPath}/test/*.cpp ${_fullPath}/test/*.hpp)
    add_executable( test_${FANTOM_TOOLBOX_NAME}_${PLUGIN} ${TEST_SOURCES} )
    target_compile_options( test_${FANTOM_TOOLBOX_NAME}_${PLUGIN} PRIVATE ${BUILD_CXX_DIALECT} )
    target_include_directories( test_${FANTOM_TOOLBOX_NAME}_${PLUGIN}
      PRIVATE ${_fullPath}/test ${_fullPath}/include )
    set_target_properties( test_${FANTOM_TOOLBOX_NAME}_${PLUGIN} PROPERTIES
      LINKER_LANGUAGE CXX
      )
    target_link_libraries( test_${FANTOM_TOOLBOX_NAME}_${PLUGIN} fantom-lib ${FANTOM_TOOLBOX_NAME}_${PLUGIN} catch )
    add_test( test_${FANTOM_TOOLBOX_NAME}_${PLUGIN} test_${FANTOM_TOOLBOX_NAME}_${PLUGIN} )
    # TODO: create custom targets for all python files in the test directory and hook them up as tests
  endif()

  # install toolbox into FANTOM_DIR
  IF(EXISTS ${_fullPath}/FAnToM-Dependencies.txt)
    install( FILES ${CMAKE_BINARY_DIR}/lib/fantom-plugins/${FANTOM_TOOLBOX_NAME}/lib${PLUGIN}.dependency DESTINATION ${FANTOM_DIR}/lib/fantom-plugins/${FANTOM_TOOLBOX_NAME} )
  ENDIF(EXISTS ${_fullPath}/FAnToM-Dependencies.txt)
  install(TARGETS ${FANTOM_TOOLBOX_NAME}_${PLUGIN} EXPORT ${FANTOM_TOOLBOX_NAME}_${PLUGIN}Config
    ARCHIVE  DESTINATION ${FANTOM_DIR}/lib/fantom-plugins/${FANTOM_TOOLBOX_NAME}
    LIBRARY  DESTINATION ${FANTOM_DIR}/lib/fantom-plugins/${FANTOM_TOOLBOX_NAME}
    RUNTIME  DESTINATION ${FANTOM_DIR}/lib/fantom-plugins/${FANTOM_TOOLBOX_NAME})  # This is for Windows
  if( EXISTS ${_fullPath}/include )
    install(DIRECTORY ${_fullPath}/include/ DESTINATION ${FANTOM_DIR}/include/fantom-plugins/${FANTOM_TOOLBOX_NAME}/${PLUGIN})
  endif()
  if( EXISTS ${_fullPath}/resources )
    install(DIRECTORY ${_fullPath}/resources/ DESTINATION ${FANTOM_DIR}/share/FAnToM/${FANTOM_TOOLBOX_NAME}/${PLUGIN})
  endif()
  install(EXPORT ${FANTOM_TOOLBOX_NAME}_${PLUGIN}Config DESTINATION ${FANTOM_DIR}/share/FAnToM/cmake/plugins)
endfunction( FANTOM_ADD_PLUGIN )


###
# add a directory as a new module. If the directory has a CMakeLists.txt file,
# the directory is included using add_subdirectory and the CMakeLists.txt has to
# comply with the module writing rules from above.
# If no CMakeLists.txt exists, the default behavior is applied, which includes
# checking the dependencies and compiling the module correctly.
#
# The Plugin must be located at ${CMAKE_SOURCE_DIR}!
function( FANTOM_ADD_PLUGIN_DIRECTORY PLUGIN )
    GET_FILENAME_COMPONENT(_fullPath ${CMAKE_SOURCE_DIR}/${PLUGIN} ABSOLUTE)
    IF(EXISTS ${_fullPath}/CMakeLists.txt)
        # if we have a CMakeLists.txt in the module directory,
        # use the file. It is preferable that the file calls
        # FANTOM_ADD_PLUGIN to add the code so FAnToM can perform all
        # required checks. Otherwise, it must create the module
        # code following the FANTOM_ADD_PLUGIN command.
        add_subdirectory( ${PLUGIN} )
    ELSE()
        FANTOM_ADD_PLUGIN( ${PLUGIN} )
    ENDIF()
endfunction( FANTOM_ADD_PLUGIN_DIRECTORY )
