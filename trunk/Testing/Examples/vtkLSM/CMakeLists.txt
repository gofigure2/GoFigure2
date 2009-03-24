ADD_EXECUTABLE( lsm2jpg 
   lsm2jpg.cxx
   vtkLSMReader.cxx
  )
TARGET_LINK_LIBRARIES( lsm2jpg 
  vtkIO
  vtkImaging 
  )

ADD_EXECUTABLE( lsm2mha 
   lsm2mha.cxx
   vtkLSMReader.cxx
  )
TARGET_LINK_LIBRARIES( lsm2mha 
  vtkIO
  vtkImaging 
  )

ADD_EXECUTABLE( lsmColorVizu 
  lsmColorVizu.cxx
  vtkLSMReader.cxx
  )
TARGET_LINK_LIBRARIES( lsmColorVizu 
  vtkIO
  vtkImaging 
  vtkRendering
  vtkRenderingAddOn2
  )

