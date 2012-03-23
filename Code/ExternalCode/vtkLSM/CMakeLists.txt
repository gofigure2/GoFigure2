if( WIN32 )
  if( NOT CYGWIN )
    if( NOT MINGW )
      if( BUILD_SHARED_LIBS )
        add_definitions( -DvtkLSM_EXPORT )
      endif( BUILD_SHARED_LIBS )
    endif( NOT MINGW )
  endif( NOT CYGWIN )
endif( WIN32 )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/vtkLSMConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/vtkLSMConfigure.h
  @ONLY IMMEDIATE
)

add_library( vtkLSMReader
  vtkLSMReader.cxx
)

target_link_libraries( vtkLSMReader
  vtkIO
  vtkFiltering
)

set_target_properties( vtkLSMReader PROPERTIES VERSION ${GOFIGURE2_LIB_VERSION} SOVERSION ${GOFIGURE2_LIB_VERSION} )

# Runtime
install( TARGETS vtkLSMReader
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} NAMELINK_SKIP COMPONENT Libraries
  )

# Development
install( TARGETS vtkLSMReader
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries NAMELINK_ONLY
  )

file( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )

install( FILES ${__source_file_h}
  ${GOFIGURE2_BINARY_DIR}/Code/ExternalCode/vtkLSM/vtkLSMConfigure.h
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)

