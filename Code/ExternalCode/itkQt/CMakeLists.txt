IF( WIN32 )
  IF( NOT CYGWIN )
    IF( NOT MINGW )
      IF( BUILD_SHARED_LIBS )
        ADD_DEFINITIONS( -DitkQt_EXPORT )
      ENDIF( BUILD_SHARED_LIBS )
    ENDIF( NOT MINGW )
  ENDIF( NOT CYGWIN )
ENDIF( WIN32 )


CONFIGURE_FILE( ${GOFIGURE2_SOURCE_DIR}/Code/ExternalCode/itkQt/itkQtConfigure.h.in
  ${GOFIGURE2_BINARY_DIR}/Code/ExternalCode/itkQt/itkQtConfigure.h @ONLY IMMEDIATE )
  
QT4_WRAP_CPP( itkQtAdaptor_MOC itkQtAdaptor.h )

ADD_LIBRARY( itkQt
  itkQtProgressBar.cxx
  itkQtAdaptor.h
  ${itkQtAdaptor_MOC}
)

TARGET_LINK_LIBRARIES( itkQt 
  ITKCommon
  ${QT_LIBRARIES}
  )

INSTALL( TARGETS itkQt
  ARCHIVE DESTINATION lib COMPONENT libraries
  LIBRARY DESTINATION lib COMPONENT libraries
  RUNTIME DESTINATION bin COMPONENT Runtime
  )

