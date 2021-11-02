

set(CMAKE_CXX14_EXTENSION_COMPILE_OPTION -std=c++14)
set( CMAKE_CXX_STANDARD 14 )
#=============================================================
# Python
#=============================================================
find_package(PythonLibs 2.6 REQUIRED) # export Python
add_library(Python INTERFACE IMPORTED)
set_property(TARGET Python PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES ${PYTHON_INCLUDE_DIRS})
set_property(TARGET Python PROPERTY
  INTERFACE_LINK_LIBRARIES ${PYTHON_LIBRARY})

#=============================================================
# Boost
#=============================================================
find_package( Boost REQUIRED ) # export Boost
add_library( Boost INTERFACE IMPORTED )
set_property( TARGET Boost PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

#=============================================================
# Pybind11
#=============================================================
# Use this, when apt package for Ubuntu 18.04LTS
# provides pybind11 v2.4 And remove pybind from ext/
#find_package( pybind11 REQUIRED )
#add_library( pybind11 INTERFACE IMPORTED )
#set_property( TARGET pybind11 PROPERTY
#  INTERFACE_INCLUDE_DIRECTORIES ${pybind11_INCLUDE_DIR})

#=============================================================
# OpenGL
#=============================================================
find_package( OpenGL REQUIRED ) # export OpenGL
add_library( OpenGL INTERFACE IMPORTED )
set_property( TARGET OpenGL PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES ${OPENGL_INCLUDE_DIR})
set_property(TARGET OpenGL PROPERTY
  INTERFACE_LINK_LIBRARIES ${OPENGL_LIBRARIES})

#=============================================================
# Eigen3
#=============================================================
find_package( Eigen3 REQUIRED )
add_library( Eigen3 INTERFACE IMPORTED )
set_property( TARGET Eigen3 PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${EIGEN3_INCLUDE_DIR})

#=============================================================
# Threads::Threads
#=============================================================
find_package( Threads REQUIRED )

#=============================================================
# Threads::OpenMP
#=============================================================
option( BUILD_USE_OPENMP "Use OpenMP if available" ON )
mark_as_advanced( FORCE BUILD_USE_OPENMP )
# export Threads::OpenMP only if BUILD_USE_OPENMP is ON
if( BUILD_USE_OPENMP )
    find_package( OpenMP )
    if( OpenMP_FOUND AND APPLE )
        add_library( Threads::OpenMP INTERFACE IMPORTED )
        target_link_libraries( Threads::OpenMP INTERFACE OpenMP::OpenMP_CXX )
    elseif( OPENMP_FOUND ) # cmake on ubuntu 18.04 requires old method
        add_library( Threads::OpenMP INTERFACE IMPORTED )
        set_property( TARGET Threads::OpenMP PROPERTY
            INTERFACE_COMPILE_OPTIONS ${OpenMP_CXX_FLAGS} )
        set_property( TARGET Threads::OpenMP PROPERTY
            INTERFACE_LINK_LIBRARIES ${OpenMP_CXX_FLAGS} )
    else()
        set( BUILD_USE_OPENMP OFF )
    endif()
endif()
