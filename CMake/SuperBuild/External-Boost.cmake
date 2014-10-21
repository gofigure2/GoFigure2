#---------------------------------------------------------------------------
# Get and build boost

if( WIN32 )
  ExternalProject_Add( Boost
    GIT_REPOSITORY "${git_protocol}://github.com/pocb/boost.git"
    GIT_TAG "origin/cmake-1.46.1"
    SOURCE_DIR ${proj}
    BINARY_DIR ${proj}-build
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${ep_common_args}
      -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/INSTALL
      -DBUILD_PROJECTS:STRING=program_options
      -DWITH_PYTHON:BOOL=OFF
      -DWITH_DOXYGEN:BOOL=OFF
      -DWITH_EXPAT:BOOL=OFF
      -DWITH_BZIP2:BOOL=OFF
      -DWITH_MPI:BOOL=OFF
      -DWITH_XSLTPROC:BOOL=OFF
      -DWITH_ICU:BOOL=OFF
    #INSTALL_COMMAND ""
  )

  set( Boost_LIBRARY_DIR ${CMAKE_BINARY_DIR}/INSTALL/lib/boost/ )
  set( Boost_INCLUDE_DIR ${CMAKE_BINARY_DIR}/INSTALL/include/ )
else()
  set( Boost_Bootstrap_Command ./bootstrap.sh )
  set( Boost_b2_Command ./b2 )

  set( Boost_Patches_DIR ${Patches_DIR}/boost )
  set( Boost_Patch_Script ${Boost_Patches_DIR}/boost_patch.sh )
  set( Boost_Patch_Command sh ${Boost_Patch_Script} )

  ExternalProject_Add(Boost
    URL "http://sourceforge.net/projects/boost/files/boost/1.54.0/boost_1_54_0.tar.gz"
    URL_MD5 efbfbff5a85a9330951f243d0a46e4b9
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ${Boost_Patch_Command}
    CONFIGURE_COMMAND ${Boost_Bootstrap_Command} --without-icu --prefix=${CMAKE_BINARY_DIR}/INSTALL
    BUILD_COMMAND ${Boost_b2_Command}
      --without-python
      --without-mpi
      link=static
      link=shared
      threading=multi
      -d+2
      -j${NProcessors}
      install
    INSTALL_COMMAND ""
    INSTALL_DIR ${CMAKE_BINARY_DIR}/INSTALL
  )

  set( Boost_LIBRARY_DIR ${CMAKE_BINARY_DIR}/INSTALL/lib/ )
  set( Boost_INCLUDE_DIR ${CMAKE_BINARY_DIR}/INSTALL/include/boost-1_54/ )
endif()
