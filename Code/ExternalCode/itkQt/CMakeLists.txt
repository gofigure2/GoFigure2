ADD_LIBRARY( itkQt
  itkQtProgressBar.cxx
  itkQtAdaptor.h
)

#INSTALL_FILES(/include "(\\.h|\\.txx)$")
IF( NOT APPLE )
INSTALL( TARGETS itkQt
  ARCHIVE
  DESTINATION lib
  COMPONENT libraries
  )
ENDIF( NOT APPLE )
