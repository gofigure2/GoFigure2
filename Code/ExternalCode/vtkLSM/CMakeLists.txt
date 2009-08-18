ADD_LIBRARY( vtkLSMReader
  ${LIBS_STYLE}
  vtkLSMReader.cxx
  )

TARGET_LINK_LIBRARIES( vtkLSMReader
  vtkCommon 
  vtkFiltering
  )
