ADD_LIBRARY( itkQt
  itkQtProgressBar.cxx
  itkQtAdaptor.h
)

INSTALL_FILES(/include "(\\.h|\\.txx)$")

INSTALL( TARGETS itkQt
  ARCHIVE
  DESTINATION lib
  COMPONENT libraries
  )

