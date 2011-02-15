#---------------------------------------------------------------------------
# Get and build boost

set( proj Boost )
set( version 1.45.0 )

ExternalProject_Add(${proj}
  GIT_REPOSITORY "${git_protocol}://github.com/boost-lib/boost-cmake.git"
  GIT_TAG "origin/cmake-${version}"
  SOURCE_DIR ${proj}
  BINARY_DIR ${proj}-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    # can generate erros on Mac 10.6
    -DBUILD_PROJECTS:STRING=NONE
    -DWITH_PYTHON:BOOL=OFF
    -DWITH_DOXYGEN:BOOL=OFF
    -DWITH_EXPAT:BOOL=OFF
    -DWITH_BZIP2:BOOL=OFF
    -DWITH_MPI:BOOL=OFF
    -DWITH_XSLTPROC:BOOL=OFF
    -DWITH_ICU:BOOL=OFF
  INSTALL_COMMAND ""
  DEPENDS
    ${BOOST_DEPENDENCIES}
)

INCLUDE_DIRECTORIES( BEFORE ${Boost_INCLUDE_DIRS} )

set(BOOST_LIBRARY ${CMAKE_BINARY_DIR}/${proj}-build/lib )
set(BOOST_INCLUDE_DIR ${CMAKE_BINARY_DIR}/${proj}/ )
