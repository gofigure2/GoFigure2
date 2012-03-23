#---------------------------------------------------------------------------
# Git protocole option
#---------------------------------------------------------------------------

option(GoFigure2_USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)

set(git_protocol "git")
if(NOT GoFigure2_USE_GIT_PROTOCOL)
  set(git_protocol "http")
endif()

#---------------------------------------------------------------------------
# Enable and setup External project global properties
#---------------------------------------------------------------------------

include(ExternalProject)

set(ep_base        "${CMAKE_BINARY_DIR}")
#set(ep_install_dir "${ep_base}/Install")

# should be preset for user or for developer???
option( SUPER_SHARED_LIBS ON )

if( SUPER_SHARED_LIBS )
	add_definitions( -DGOFIGURE2_BUILD_SHARED_LIBS )
else( SUPER_SHARED_LIBS )
	remove_definitions( -DGOFIGURE2_BUILD_SHARED_LIBS )
endif( SUPER_SHARED_LIBS )

set(ep_install_dir ${CMAKE_INSTALL_PREFIX})
set(ep_common_c_flags "${CMAKE_C_FLAGS_INIT} ${ADDITIONAL_C_FLAGS}")
set(ep_common_cxx_flags "${CMAKE_CXX_FLAGS_INIT} ${ADDITIONAL_CXX_FLAGS}")
set(ep_common_cxx_compiler "${CMAKE_CXX_COMPILER}" )
set(ep_common_c_compiler "${CMAKE_C_COMPILER}" )

set(ep_common_args
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DBUILD_SHARED_LIBS:BOOL=${SUPER_SHARED_LIBS}
  -DBUILD_TESTING:BOOL=OFF
  -DBUILD_EXAMPLES:BOOL=OFF
  -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
  -DCMAKE_CXX_COMPILER:FILEPATH=${ep_common_cxx_compiler}
  -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
  -DCMAKE_C_COMPILER:FILEPATH=${ep_common_c_compiler}
  -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
  )

# Compute -G arg for configuring external projects with the same CMake generator:
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()

#---------------------------------------------------------------------------
# Conditionnaly include ExternalProject Target
#---------------------------------------------------------------------------

set(GoFigure2_DEPENDENCIES)

#-------------------------

option( SUPER_BOOST "SuperBuild BOOST" ON )

set( Boost_SUPPORT )

if( SUPER_BOOST )
  include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/SuperBuild/External-Boost.cmake")
  set( Boost_SUPPORT
    -DBoost_INCLUDE_DIR:PATH=${CMAKE_BINARY_DIR}/Boost
    -DBoost_LIBRARY_DIRS:PATH=${CMAKE_BINARY_DIR}/Boost-build/lib
 )
  list(APPEND GoFigure2_DEPENDENCIES Boost)
else( SUPER_BOOST )
  include( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/ConfigBoost.cmake" )
endif( SUPER_BOOST )

#-------------------------

# REQUIRED MYSQLand QT TO BUILD VTK
option( SUPER_VTK "SuperBuild VTK" ON )

if( SUPER_VTK )
  # check if we have MySQL - COMPULSORY
  include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/ConfigMySQL.cmake")
  # check if we have QT - COMPULSORY
  include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/ConfigQT.cmake")

  option( SUPER_VTK_VIDEO "ENABLE THE VIDEO SUPPORT IN SUPERBUILD" OFF )
  if( SUPER_VTK_VIDEO )
    if( NOT WIN32 )
      option( SUPER_FFMPEG "SuperBuild FFMPEG" )
      # check if we have some video support (FFMPEG or AVI) - OPTIONAL
      if( SUPER_FFMPEG )
        #add dependency to VTK if we have to build FFMPEG
        set(VTK_DEPENDENCIES FFMPEG)
        include( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/SuperBuild/External-FFMPEG.cmake")
      else( SUPER_FFMPEG )
        include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/ConfigVideo.cmake")
      endif( SUPER_FFMPEG )
    else( NOT WIN32 )
      include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/ConfigVideo.cmake")
    endif( NOT WIN32 )
  endif( SUPER_VTK_VIDEO)

  # add the vtk external project
  include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/SuperBuild/External-VTK.cmake")
  # add the external projrct "VTK" to the list of dependencies
  list(APPEND GoFigure2_DEPENDENCIES VTK)
else( SUPER_VTK )
  # check if our vtk is properly configured
  include( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/ConfigVTK.cmake" )
endif( SUPER_VTK )

#-------------------------

option( SUPER_ITK "SuperBuild ITK" ON )

if( SUPER_ITK )
  include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/SuperBuild/External-ITK.cmake")
  list(APPEND GoFigure2_DEPENDENCIES ITK)
else( SUPER_ITK )
  include( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/ConfigITK.cmake" )
endif( SUPER_ITK )

#---------------------------------------------------------------------------
# Set superbuild boolean args
#

#set(GoFigure2_cmake_boolean_args
#  BUILD_DOCUMENTATION
#  BUILD_TESTING
#  BUILD_SHARED_LIBS
#  WITH_COVERAGE
#  WITH_MEMCHECK
#  )

#set(GoFigure2_superbuild_boolean_args)
#foreach(GoFigure2_cmake_arg ${GoFigure2_cmake_boolean_args})
#  list(APPEND GoFigure2_superbuild_boolean_args -D${GoFigure2_cmake_arg}:BOOL=${${GoFigure2_cmake_arg}})
#endforeach()

# message("CMake args:")
# foreach(arg ${GoFigure2_superbuild_boolean_args})
#   message("  ${arg}")
# endforeach()

#---------------------------------------------------------------------------
# Configure and build GoFigure2
#---------------------------------------------------------------------------

set(proj GoFigure2)
option( GOFIGURE2_EXAMPLE "Force building GoFigure2's example" OFF )
option( GOFIGURE2_OPENMP "Enable OpenMP support for GoFigure2" ON )

ExternalProject_Add(${proj}
  DEPENDS ${GoFigure2_DEPENDENCIES}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  BINARY_DIR GoFigure2-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
     ${ep_common_args}
     ${GoFigure2_superbuild_boolean_args}
    -DSUPERBUILD:BOOL=OFF
    # ITK
    -DITK_DIR:PATH=${ITK_DIR}
    # VTK
    -DVTK_DIR:PATH=${VTK_DIR}
    # QT (From VTK)
    -DQT_QMAKE_EXECUTABLE:PATH=${QT_QMAKE_EXECUTABLE}
    -DBUILD_EXAMPLES:BOOL=${GOFIGURE2_EXAMPLE}
    -DOPENMP_SUPPORT:BOOL=${GOFIGURE2_OPENMP}
    ${Boost_SUPPORT}
  INSTALL_COMMAND ""
  )

add_custom_target(superinstall 
                  COMMAND ${CMAKE_COMMAND} -E chdir GoFigure2-build/ make install
                  DEPENDS GoFigure2)

add_custom_target(superpackage
                  COMMAND ${CMAKE_COMMAND} -E chdir GoFigure2-build/ make package
                  DEPENDS GoFigure2)
