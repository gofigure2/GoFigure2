QT4_WRAP_CPP( itkQtAdaptor_MOC itkQtAdaptor.h )

ADD_LIBRARY( itkQt
  itkQtProgressBar.cxx
  itkQtAdaptor.h
  ${itkQtAdaptor_MOC}
)

INSTALL_TARGETS(/lib itkQt)

INSTALL_FILES(/include "(\\.h|\\.txx)$")

INSTALL( TARGETS itkQt
  ARCHIVE
  DESTINATION lib
  COMPONENT libraries
  )
