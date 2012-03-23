if( WIN32 )
  if( NOT CYGWIN )
    if( NOT MINGW )
      if( BUILD_SHARED_LIBS )
        add_definitions( -DitkQt_EXPORT )
      endif( BUILD_SHARED_LIBS )
    endif( NOT MINGW )
  endif( NOT CYGWIN )
endif( WIN32 )


configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/itkQtConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/itkQtConfigure.h @ONLY IMMEDIATE )

QT4_WRAP_CPP( itkQtAdaptor_MOC itkQtAdaptor.h )

add_library( itkQt
  itkQtProgressBar.cxx
  itkQtProgressDialog.cxx
  itkQtAdaptor.h
  ${itkQtAdaptor_MOC}
)

target_link_libraries( itkQt
  ${ITK_LIBRARIES}
  ${QT_LIBRARIES}
  )

set_target_properties( itkQt PROPERTIES VERSION ${GOFIGURE2_LIB_VERSION} SOVERSION ${GOFIGURE2_LIB_VERSION} )

# Runtime
install( TARGETS itkQt
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} NAMELINK_SKIP COMPONENT Libraries
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  )

# Development
install( TARGETS itkQt
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} NAMELINK_ONLY COMPONENT Libraries
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  )

file( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
file( GLOB __binary_file_h "${CMAKE_CURRENT_BINARY_DIR}/*.h" )

install( FILES ${__source_file_h} ${__binary_file_h}
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)
