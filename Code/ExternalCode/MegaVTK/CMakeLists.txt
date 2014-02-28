if( WIN32 )
  if( NOT CYGWIN )
    if( NOT MINGW )
      if( BUILD_SHARED_LIBS )
        add_definitions( -DvtkRenderingAddOn2_EXPORTS )
      endif( BUILD_SHARED_LIBS )
    endif( NOT MINGW )
  endif( NOT CYGWIN )
endif( WIN32 )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/MegaVTK2Configure.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/MegaVTK2Configure.h @ONLY IMMEDIATE)

subdirs( vtkRenderingAddOn )
subdirs( vtkItk )

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/MegaVTK2Configure.h
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)

