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

INCLUDE(ExternalProject)

set(ep_base        "${CMAKE_BINARY_DIR}")
#set(ep_install_dir "${ep_base}/Install")

# should be preset for user or for developer???
OPTION( SUPER_SHARED_LIBS ON )

IF( SUPER_SHARED_LIBS )
	ADD_DEFINITIONS( -DGOFIGURE2_BUILD_SHARED_LIBS )
ELSE( SUPER_SHARED_LIBS )
	REMOVE_DEFINITIONS( -DGOFIGURE2_BUILD_SHARED_LIBS )
ENDIF( SUPER_SHARED_LIBS )

SET(ep_install_dir ${CMAKE_INSTALL_PREFIX})
SET(ep_common_c_flags "${CMAKE_C_FLAGS_INIT} ${ADDITIONAL_C_FLAGS}")
SET(ep_common_cxx_flags "${CMAKE_CXX_FLAGS_INIT} ${ADDITIONAL_CXX_FLAGS}")

SET(ep_common_args
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DBUILD_SHARED_LIBS:BOOL=${SUPER_SHARED_LIBS}
  -DBUILD_TESTING:BOOL=OFF
  -DBUILD_EXAMPLES:BOOL=OFF
  -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
  -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
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

# REQUIRED MYSQLand QT TO BUILD VTK
OPTION( SUPER_VTK "SuperBuild VTK" ON )

IF( SUPER_VTK )
  # check if we have MySQL - COMPULSORY
  include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/ConfigMySQL.cmake")
  # check if we have QT - COMPULSORY
  include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/ConfigQT.cmake")
  
  OPTION( SUPER_VTK_VIDEO "ENABLE THE VIDEO SUPPORT IN SUPERBUILD" OFF )
  IF( SUPER_VTK_VIDEO )
    # check if we have some video support (FFMPEG or AVI) - OPTIONAL
    include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/ConfigVideo.cmake")
  ENDIF( SUPER_VTK_VIDEO)

  # add the vtk external project
  include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/SuperBuild/External-VTK.cmake")
  # add the external projrct "VTK" to the list of dependencies
  LIST(APPEND GoFigure2_DEPENDENCIES VTK)
ELSE( SUPER_VTK )
  # check if our vtk is properly configured
  include( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/ConfigVTK.cmake" )
ENDIF( SUPER_VTK )

#-------------------------

OPTION( SUPER_ITK "SuperBuild ITK" ON )

IF( SUPER_ITK )
  include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/SuperBuild/External-ITK.cmake")
  LIST(APPEND GoFigure2_DEPENDENCIES ITK)
ELSE( SUPER_ITK )
  include( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/ConfigITK.cmake" )
ENDIF( SUPER_ITK )

#-------------------------

OPTION( SUPER_BOOST "SuperBuild BOOST" ON )

IF( SUPER_BOOST )
  include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/SuperBuild/External-Boost.cmake")
  LIST(APPEND GoFigure2_DEPENDENCIES Boost)
ELSE( SUPER_BOOST )
  include( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/ConfigBoost.cmake" )
ENDIF( SUPER_BOOST )

#---------------------------------------------------------------------------
# Set superbuild boolean args
#

#SET(GoFigure2_cmake_boolean_args
#  BUILD_DOCUMENTATION
#  BUILD_TESTING
#  BUILD_SHARED_LIBS
#  WITH_COVERAGE
#  WITH_MEMCHECK
#  )

#SET(GoFigure2_superbuild_boolean_args)
#FOREACH(GoFigure2_cmake_arg ${GoFigure2_cmake_boolean_args})
#  LIST(APPEND GoFigure2_superbuild_boolean_args -D${GoFigure2_cmake_arg}:BOOL=${${GoFigure2_cmake_arg}})
#ENDFOREACH()

# MESSAGE("CMake args:")
# FOREACH(arg ${GoFigure2_superbuild_boolean_args})
#   MESSAGE("  ${arg}")
# ENDFOREACH()

#---------------------------------------------------------------------------
# Configure and build GoFigure2
#---------------------------------------------------------------------------

set(proj GoFigure2)

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
    -DQT_QMAKE_EXECUTABLE:PATH=${VTK_QT_QMAKE_EXECUTABLE}
  INSTALL_COMMAND ""
  )

ADD_CUSTOM_TARGET(superinstall 
                  COMMAND ${CMAKE_COMMAND} -E chdir GoFigure2-build/ make install
                  DEPENDS GoFigure2)

ADD_CUSTOM_TARGET(superpackage
                  COMMAND ${CMAKE_COMMAND} -E chdir GoFigure2-build/ make package
                  DEPENDS GoFigure2)
