SET( EXAMPLES_QT_NOT_REQUIRED
  GoDBRecordSetTest
  lsm2jpg
  lsm2vtk
  lsm2mha
  extractChannelAtTimePointFromLSM
  vtkPolyDataMySQLTextWriter
  )

FOREACH( var ${EXAMPLES_QT_NOT_REQUIRED} )

  ADD_EXECUTABLE( ${var}
    ${var}.cxx
  )
  TARGET_LINK_LIBRARIES( ${var}
    QGoIO
  )

ENDFOREACH( var )

ADD_TEST( vtkPolyDataMySQLTextReaderTEST1
  ${EXECUTABLE_OUTPUT_PATH}/vtkPolyDataMySQLTextReader
  ${TESTING_DATA_PATH}/contour.vtk
)

ADD_TEST( vtkPolyDataMySQLTextReaderTEST2
  ${EXECUTABLE_OUTPUT_PATH}/vtkPolyDataMySQLTextReader
  ${TESTING_DATA_PATH}/sphere.vtk
)

ADD_TEST( vtkPolyDataMySQLTextWriterTEST1
  ${EXECUTABLE_OUTPUT_PATH}/vtkPolyDataMySQLTextWriter
  ${TESTING_DATA_PATH}/contour.vtk
)

ADD_TEST( vtkPolyDataMySQLTextWriterTEST2
  ${EXECUTABLE_OUTPUT_PATH}/vtkPolyDataMySQLTextWriter
  ${TESTING_DATA_PATH}/sphere.vtk
)

IF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )
  SET( EXAMPLES_QT_REQUIRED
    lsmColorVizu
    lsmColorVizuCollection
    3DLsmSerie2mha
    ImportMultiFilesInDB
    MegaCapture2mha
  )

  FOREACH( var ${EXAMPLES_QT_REQUIRED} )

     ADD_EXECUTABLE( ${var}
       ${var}.cxx
    )

     TARGET_LINK_LIBRARIES( ${var}
       QGoIO
       QMegaVTKAddOn2
       )

  ENDFOREACH( var )

ENDIF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )

ADD_EXECUTABLE( vtkPolyDataMySQLTextReader
  vtkPolyDataMySQLTextReader.cxx )

TARGET_LINK_LIBRARIES( vtkPolyDataMySQLTextReader
  QGoIO
  vtkRendering
)


