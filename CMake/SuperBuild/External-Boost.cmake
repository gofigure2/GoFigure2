#---------------------------------------------------------------------------
# Get and build boost

set( proj Boost )
set( version 1.46.1 )

ExternalProject_Add(${proj}
  GIT_REPOSITORY "${git_protocol}://github.com/pocb/boost.git"
  GIT_TAG "origin/cmake-${version}"
  SOURCE_DIR ${proj}
  BINARY_DIR ${proj}-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    # can generate erros on Mac 10.6
#    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/${proj}-INSTALL
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

#set(Boost_LIBRARY ${CMAKE_BINARY_DIR}/${proj}-INSTALL/lib )
#set(Boost_INCLUDE_DIR ${CMAKE_BINARY_DIR}/${proj}-INSTALL/include )
set(Boost_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/${proj})
