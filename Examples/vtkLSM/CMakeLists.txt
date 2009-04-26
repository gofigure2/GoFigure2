ADD_EXECUTABLE( lsm2jpg
   lsm2jpg.cxx
   ${QGOFIGURE_SOURCE_DIR}/ExternalCode/vtkLSM/vtkLSMReader.cxx
  )
TARGET_LINK_LIBRARIES( lsm2jpg
  vtkIO
  vtkImaging
  vtkFiltering
  vtkCommon
  )

ADD_EXECUTABLE( lsm2vtk
   lsm2vtk.cxx
   ${QGOFIGURE_SOURCE_DIR}/ExternalCode/vtkLSM/vtkLSMReader.cxx
  )
TARGET_LINK_LIBRARIES( lsm2vtk
  vtkIO
  vtkImaging
  vtkFiltering
  vtkCommon
  )

ADD_EXECUTABLE( lsm2mha
   lsm2mha.cxx
   ${QGOFIGURE_SOURCE_DIR}/ExternalCode/vtkLSM/vtkLSMReader.cxx
  )
TARGET_LINK_LIBRARIES( lsm2mha
  vtkIO
  vtkImaging
  vtkFiltering
  vtkCommon
  )

IF( QT_FOUND )
  ADD_EXECUTABLE( lsmColorVizu
    lsmColorVizu.cxx
    ${QGOFIGURE_SOURCE_DIR}/ExternalCode/vtkLSM/vtkLSMReader.cxx
    )
  TARGET_LINK_LIBRARIES( lsmColorVizu
    vtkIO
    vtkImaging
    vtkRendering
    vtkRenderingAddOn2
    vtkFiltering
    QMegaVTKAddOn2
    ${QT_LIBRARIES}
    QVTK
    )

  ADD_EXECUTABLE( lsmColorVizuCollection
    lsmColorVizuCollection.cxx
    ${QGOFIGURE_SOURCE_DIR}/ExternalCode/vtkLSM/vtkLSMReader.cxx
    )
  TARGET_LINK_LIBRARIES( lsmColorVizuCollection
    vtkIO
    vtkImaging
    vtkRendering
    vtkRenderingAddOn2
    vtkFiltering
    QMegaVTKAddOn2
    ${QT_LIBRARIES}
    QVTK
    )
ENDIF( QT_FOUND )

