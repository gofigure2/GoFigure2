SET( GOFIGURE_APPLICATION_SOURCE_LIST
    gofigure.cxx
    )

ADD_EXECUTABLE( gofigure
  ${GOFIGURE_APPLICATION_SOURCE_LIST}
  )

TARGET_LINK_LIBRARIES( gofigure
  QGoLib
#   QGoImageView
#   ${QT_LIBRARIES}
#   QVTK
  ITKBasicFilters
  ITKNumerics
  ITKIO
  vtkImaging
  )

INSTALL( TARGETS gofigure
      RUNTIME DESTINATION bin
      BUNDLE  DESTINATION bin
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib/static
      COMPONENT applications
      )

