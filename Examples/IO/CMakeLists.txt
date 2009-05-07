SET( EXAMPLES_QT_NOT_REQUIRED
  ImportMultiFilesInDB
  GoDBRecordSetTest
  3DLsmSerie2mha
  MegaCapture2mha
  lsm2jpg 
  lsm2vtk
  lsm2mha 
  )

FOREACH( var ${EXAMPLES_QT_NOT_REQUIRED} )

  ADD_EXECUTABLE( ${var} 
    ${var}.cxx 
    ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/vtkLSM/vtkLSMReader.cxx
    ${GOFIGURE2_SOURCE_DIR}/Code/IO/CreateDataBaseHelper.cxx )
  TARGET_LINK_LIBRARIES( ${var}
    ITKCommon 
    ITKIO
    vtkIO
    vtkImaging
    vtkFiltering
    vtkCommon )

ENDFOREACH( var )

IF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )
  SET( EXAMPLES_QT_REQUIRED 
    lsmColorVizu 
    lsmColorVizuCollection )

  FOREACH( var ${EXAMPLES_QT_REQUIRED} )

     ADD_EXECUTABLE( ${var} 
       ${var}.cxx
       ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/vtkLSM/vtkLSMReader.cxx )

     TARGET_LINK_LIBRARIES( ${var}
       vtkIO
       vtkImaging
       vtkRendering
       vtkRenderingAddOn2
       vtkFiltering
       QMegaVTKAddOn2
       ${QT_LIBRARIES}
       QVTK )

  ENDFOREACH( var )

ENDIF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )

