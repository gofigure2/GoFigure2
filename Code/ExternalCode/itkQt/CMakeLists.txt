QT4_WRAP_CPP( itkQtAdaptor_MOC itkQtAdaptor.h )

ADD_LIBRARY( itkQt
  # STATIC
  itkQtProgressBar.cxx
  itkQtAdaptor.h
  ${itkQtAdaptor_MOC}
)

TARGET_LINK_LIBRARIES( itkQt 
  ${QT_LIBRARIES}
  ITKCommon
  )

INSTALL( TARGETS itkQt
  ARCHIVE DESTINATION lib COMPONENT libraries
  LIBRARY DESTINATION lib COMPONENT libraries
  RUNTIME DESTINATION bin COMPONENT Runtime
  )

