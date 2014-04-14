#---------------------------------------------------------------------------
# Get and build boost

set( Boost_Bootstrap_Command )
if( UNIX )
  set( Boost_Bootstrap_Command ./bootstrap.sh )
  set( Boost_b2_Command ./b2 )
else()
  if( WIN32 )
    set( Boost_Bootstrap_Command bootstrap.bat )
    set( Boost_b2_Command b2.exe )
  endif()
endif()

set( Boost_Patches_DIR ${Patches_DIR}/boost )
set( Boost_Patch_Script ${Boost_Patches_DIR}/boost_patch.sh )

set( Boost_Patch_Command )
if( NOT WIN32 )
  set( Boost_Patch_Command sh ${Boost_Patch_Script} )
endif()
  
ExternalProject_Add(Boost
  URL "http://sourceforge.net/projects/boost/files/boost/1.54.0/boost_1_54_0.tar.gz"
  URL_MD5 efbfbff5a85a9330951f243d0a46e4b9
  BUILD_IN_SOURCE 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ${Boost_Patch_Command}
  CONFIGURE_COMMAND ${Boost_Bootstrap_Command} 
  BUILD_COMMAND ${Boost_b2_Command}
    --prefix=${CMAKE_BINARY_DIR}/INSTALL
    # --without-icu 
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

if( NOT WIN32 )
  set(Boost_LIBRARY_DIR ${CMAKE_BINARY_DIR}/INSTALL/lib/boost/ )
  set(Boost_INCLUDE_DIR ${CMAKE_BINARY_DIR}/INSTALL/include/ )
else()
  set(Boost_LIBRARY_DIR ${CMAKE_BINARY_DIR}/INSTALL/lib/ )
  set(Boost_INCLUDE_DIR ${CMAKE_BINARY_DIR}/INSTALL/include/boost-1_54/ )
endif()
