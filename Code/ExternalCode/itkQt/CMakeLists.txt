ADD_LIBRARY( itkQt
  ${LIBS_STYLE}
  itkQtProgressBar.cxx
  itkQtAdaptor.h
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
ENDIF( NOT APPLE )
