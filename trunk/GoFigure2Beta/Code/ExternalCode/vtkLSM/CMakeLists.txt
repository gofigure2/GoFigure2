ADD_LIBRARY( vtkLSMReader
  vtkLSMReader.cxx )

TARGET_LINK_LIBRARIES( vtkLSMReader
  vtkIO
  vtkFiltering
)

INSTALL( TARGETS vtkLSMReader
  ARCHIVE
  DESTINATION lib
  COMPONENT libraries
  )

