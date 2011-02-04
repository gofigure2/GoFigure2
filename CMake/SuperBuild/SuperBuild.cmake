#---------------------------------------------------------------------------
# Git protocole option
#---------------------------------------------------------------------------

option(GoFigure2_USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)

set(git_protocol "git")
if(NOT GoFigure2_USE_GIT_PROTOCOL)
  set(git_protocol "http")
endif()

#---------------------------------------------------------------------------
# Qt - Let's check if a valid version of Qt is available
#---------------------------------------------------------------------------

FIND_PACKAGE(Qt4)
IF(QT_FOUND)
  INCLUDE( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigQT.cmake" )
ELSE()
  MESSAGE(FATAL_ERROR "error: Qt4 was not found on your system. You probably need to set the QT_QMAKE_EXECUTABLE variable")
ENDIF()

#---------------------------------------------------------------------------
# Enable and setup External project global properties
#---------------------------------------------------------------------------

INCLUDE(ExternalProject)

set(ep_base        "${CMAKE_BINARY_DIR}")
#set(ep_install_dir "${ep_base}/Install")

SET(ep_common_args
  #-DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DBUILD_TESTING:BOOL=OFF
  -DBUILD_EXAMPLES:BOOL=OFF
  )

SET(ep_common_c_flags "${CMAKE_C_FLAGS_INIT} ${ADDITIONAL_C_FLAGS}")
SET(ep_common_cxx_flags "${CMAKE_CXX_FLAGS_INIT} ${ADDITIONAL_CXX_FLAGS}")

# Compute -G arg for configuring external projects with the same CMake generator:
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()

#---------------------------------------------------------------------------
# Conditionnaly include ExternalProject Target
#---------------------------------------------------------------------------

include("${GOFIGURE2_SOURCE_DIR}/CMake/SuperBuild/External-ITK.cmake")
include("${GOFIGURE2_SOURCE_DIR}/CMake/SuperBuild/External-VTK.cmake")

set(GoFigure2_DEPENDENCIES ITK VTK)

#---------------------------------------------------------------------------
# Set superbuild boolean args
#

SET(GoFigure2_cmake_boolean_args
  BUILD_DOCUMENTATION
  BUILD_TESTING
  BUILD_SHARED_LIBS
  WITH_COVERAGE
  WITH_MEMCHECK
  )

SET(GoFigure2_superbuild_boolean_args)
FOREACH(GoFigure2_cmake_arg ${GoFigure2_cmake_boolean_args})
  LIST(APPEND GoFigure2_superbuild_boolean_args -D${GoFigure2_cmake_arg}:BOOL=${${GoFigure2_cmake_arg}})
ENDFOREACH()

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
    ${GoFigure2_superbuild_boolean_args}
    -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DSubversion_SVN_EXECUTABLE:FILEPATH=${Subversion_SVN_EXECUTABLE}
    -DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE}
    -DUSE_SUPERBUILD:BOOL=OFF
    -DGoFigure2_C_FLAGS:STRING=${GoFigure2_C_FLAGS}
    -DGoFigure2_CXX_FLAGS:STRING=${GoFigure2_CXX_FLAGS}
    # ITK
    -DITK_DIR:PATH=${ITK_DIR}
    # VTK
    -DVTK_DIR:PATH=${VTK_DIR}
#    -DVTK_DEBUG_LEAKS:BOOL=${GoFigure2_USE_VTK_DEBUG_LEAKS}
    # Qt
    -DQT_QMAKE_EXECUTABLE:PATH=${QT_QMAKE_EXECUTABLE}
    # MySQL
    # Boost
  INSTALL_COMMAND ""
  )
