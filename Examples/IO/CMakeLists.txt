set( QGOIO_EXAMPLES_SRC
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
  GoDBImportTracksTest
  GoDBExportContourTest
#  kishmeshtextfilereader
#  trackTextFilesReader
)

set( GODB_SRC
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

set( QGOIO_EXAMPLES_SRC
  ${QGOIO_EXAMPLES_SRC}
  ${GODB_SRC}
)

foreach( var ${QGOIO_EXAMPLES_SRC} )
  add_executable( ${var} ${var}.cxx )
  target_link_libraries( ${var}
    QGoIO
  )
endforeach( var ${QGOIO_EXAMPLES_SRC} )

add_test( gofigurefileinfomultiindexcontainerhelperTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gofigurefileinfomultiindexcontainerhelper )

add_test( vtkPolyDataMySQLContourReaderTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLContourReader
  ${TESTING_DATA_PATH}/contour.vtk
)

add_test( vtkPolyDataMySQLMeshReaderTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLMeshReader
  ${TESTING_DATA_PATH}/sphere.vtk
)

add_test( vtkPolyDataMySQLTrackTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLTrack
)

add_test( vtkPolyDataMySQLContourWriterTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLContourWriter
  ${TESTING_DATA_PATH}/contour.vtk
)

add_test( vtkPolyDataMySQLMeshWriterTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLMeshWriter
  ${TESTING_DATA_PATH}/sphere.vtk
)

add_test( MegaCaptureHeaderReaderTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MegaCaptureHeaderReaderTest
  ${GOFIGURE2_BINARY_DIR}/Testing/earpax2isl11/earpax2isl11.lsm.meg
)

add_test( lsmtomegacaptureTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lsmtomegacapture
  ${TESTING_DATA_PATH}/earpax2isl11.lsm
  ${GOFIGURE2_BINARY_DIR}/Testing/earpax2isl11_lsmtomegacapture/
  png
  )

foreach( var ${GODB_SRC} )
  add_test( ${var}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${var}
  )
endforeach( var ${GODB_SRC} )

