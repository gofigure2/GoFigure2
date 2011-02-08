#---------------------------------------------------------------------------
# Get and build boost

set(proj Boost)
set( version 1.45.0 )

ExternalProject_Add(${proj}
  GIT_REPOSITORY "${git_protocol}://github.com/boost-lib/boost-cmake.git"
  GIT_TAG "origin/cmake-${version}"
  SOURCE_DIR ${proj}
  BINARY_DIR ${proj}-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
    -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
    # can generate erros on Mac 10.6
    -DWITH_PYTHON=OFF
  INSTALL_COMMAND ""
  DEPENDS
    ${BOOST_DEPENDENCIES}
)

INCLUDE_DIRECTORIES( BEFORE ${Boost_INCLUDE_DIRS} )

set(BOOST_LIBRARY ${CMAKE_BINARY_DIR}/${proj}-build/lib )
set(BOOST_INCLUDE_DIR ${CMAKE_BINARY_DIR}/${proj}/ )
