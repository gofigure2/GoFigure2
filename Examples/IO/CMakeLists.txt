SET( QGOIO_EXAMPLES_SRC
  megacaptureimport
  gofigurefileinfomultiindexcontainerhelper
  MegaCaptureHeaderReaderTest
  lsm2mha
  lsmtomegacapture
  itkmegacapturereader
  GenerateImagingSessionPartInMegaCaptureHeaderFromMHA
  mhatomegacapture
  vtkPolyDataMySQLContourReader
  vtkPolyDataMySQLMeshReader
  vtkPolyDataMySQLContourWriter
  vtkPolyDataMySQLMeshWriter
  vtkPolyDataMySQLTrack
  kishmeshtextfilereader
  trackTextFilesReader
)

SET( GODB_SRC
  GoDBTraceRowTest
  GoDBContourRowTest
  GoDBMeshRowTest
  GoDBTrackRowTest
  GoDBLineageRowTest
  GoDBColorRowTest
  GoDBChannelRowTest
  GoDBBookmarkRowTest
  GoDBImageRowTest
  GoDBIntensityRowTest
  GoDBProjectRowTest
  GoDBCoordinateRowTest
  GoDBImgSessionRowTest
)

SET( QGOIO_EXAMPLES_SRC
  ${QGOIO_EXAMPLES_SRC}
  ${GODB_SRC}
)

FOREACH( var ${QGOIO_EXAMPLES_SRC} )
  ADD_EXECUTABLE( ${var} ${var}.cxx )
  TARGET_LINK_LIBRARIES( ${var}
    QGoIO
  )
ENDFOREACH( var ${QGOIO_EXAMPLES_SRC} )

ADD_TEST( gofigurefileinfomultiindexcontainerhelperTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gofigurefileinfomultiindexcontainerhelper )

ADD_TEST( vtkPolyDataMySQLContourReaderTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLContourReader
  ${TESTING_DATA_PATH}/contour.vtk
)

ADD_TEST( vtkPolyDataMySQLMeshReaderTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLMeshReader
  ${TESTING_DATA_PATH}/sphere.vtk
)

ADD_TEST( vtkPolyDataMySQLTrackTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLTrack
)

ADD_TEST( vtkPolyDataMySQLContourWriterTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLContourWriter
  ${TESTING_DATA_PATH}/contour.vtk
)

ADD_TEST( vtkPolyDataMySQLMeshWriterTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLMeshWriter
  ${TESTING_DATA_PATH}/sphere.vtk
)

ADD_TEST( MegaCaptureHeaderReaderTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MegaCaptureHeaderReaderTest
  ${GOFIGURE2_BINARY_DIR}/Testing/earpax2isl11/earpax2isl11.lsm.meg  
)

ADD_TEST( lsmtomegacaptureTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lsmtomegacapture
  ${TESTING_DATA_PATH}/earpax2isl11.lsm
  ${GOFIGURE2_BINARY_DIR}/Testing/earpax2isl11_lsmtomegacapture/
  png
  )
  
FOREACH( var ${GODB_SRC} )
  ADD_TEST( ${var} 
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${var}
  )
ENDFOREACH( var ${GODB_SRC} )

