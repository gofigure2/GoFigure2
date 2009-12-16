ADD_LIBRARY( vtkLSMReader
  vtkLSMReader.cxx )

TARGET_LINK_LIBRARIES( vtkLSMReader
  vtkIO
  vtkFiltering
)