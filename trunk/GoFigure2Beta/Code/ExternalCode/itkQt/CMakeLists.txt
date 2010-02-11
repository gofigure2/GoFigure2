QT4_WRAP_CPP( itkQtAdaptor_MOC itkQtAdaptor.h )

ADD_LIBRARY( itkQt
  itkQtProgressBar.cxx
  itkQtAdaptor.h
  ${itkQtAdaptor_MOC}
)

INSTALL( TARGETS itkQt
  ARCHIVE DESTINATION lib
  COMPONENT libraries
  )
