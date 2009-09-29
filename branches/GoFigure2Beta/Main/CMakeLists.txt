SET( GOFIGURE_APPLICATION_SOURCE_LIST
    gofigure.cxx
    )

#ADD_EXECUTABLE( gofigure
#  ${GOFIGURE_APPLICATION_SOURCE_LIST}
#  ${QGoGUI_SRC}
#  )
#
#TARGET_LINK_LIBRARIES( gofigure
#  QGoGui
#  ITKBasicFilters
#  ITKNumerics
#  ITKIO
#  vtkImaging
#  )
#
#INSTALL( TARGETS gofigure
#      RUNTIME DESTINATION bin
#      BUNDLE  DESTINATION bin
#      LIBRARY DESTINATION lib
#      ARCHIVE DESTINATION lib/static
#      COMPONENT applications
#      )
#
