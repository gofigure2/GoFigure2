QT4_WRAP_CPP( itkQtAdaptor_MOC itkQtAdaptor.h )

ADD_LIBRARY( itkQt
  ${LIBS_STYLE}
  itkQtProgressBar.cxx
  itkQtAdaptor.h
  ${itkQtAdaptor_MOC}
)

TARGET_LINK_LIBRARIES( itkQt 
  ${QT_LIBRARIES}
  #ITKBasicFilters
  #ITKNumerics
  ITKCommon
  #ITKIO
  )

INSTALL( TARGETS itkQt
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  COMPONENT libraries
  )
