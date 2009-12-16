QT4_WRAP_CPP( itkQtAdaptor_MOC itkQtAdaptor.h )

ADD_LIBRARY( itkQt
  itkQtProgressBar.cxx
  itkQtAdaptor.h
  ${itkQtAdaptor_MOC}
)

TARGET_LINK_LIBRARIES( itkQt
  itkQt
  ${QT_LIBRARIES}
  ITKCommon
)

#INSTALL_FILES(/include "(\\.h|\\.txx)$")

IF( NOT APPLE )
  INSTALL( TARGETS itkQt
    ARCHIVE DESTINATION ${GOFIGURE2_LIB_INSTALL_DIR}
  COMPONENT libraries
  )
