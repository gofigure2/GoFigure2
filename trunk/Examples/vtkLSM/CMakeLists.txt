ADD_EXECUTABLE( lsm2jpg 
   lsm2jpg.cxx
   ${QGOFIGURE_SOURCE_DIR}/ExternalCode/vtkLSM/vtkLSMReader.cxx
  )
TARGET_LINK_LIBRARIES( lsm2jpg 
  vtkIO
  vtkImaging 
  )

ADD_EXECUTABLE( lsm2mha 
   lsm2mha.cxx
   ${QGOFIGURE_SOURCE_DIR}/ExternalCode/vtkLSM/vtkLSMReader.cxx
  )
TARGET_LINK_LIBRARIES( lsm2mha 
  vtkIO
  vtkImaging 
  )

ADD_EXECUTABLE( lsmColorVizu 
  lsmColorVizu.cxx
  ${QGOFIGURE_SOURCE_DIR}/ExternalCode/vtkLSM/vtkLSMReader.cxx
  )
TARGET_LINK_LIBRARIES( lsmColorVizu 
  vtkIO
  vtkImaging 
  vtkRendering
  vtkRenderingAddOn2
  )

