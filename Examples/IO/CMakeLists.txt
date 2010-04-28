SET( QGOIO_EXAMPLES_SRC
  megacaptureimport
  multifilereader
  gofigurefileinfomultiindexcontainerhelper
  MegaCaptureHeaderReaderTest
  lsmtomegacapture
  itkmegacapturereader
  GenerateImagingSessionPartInMegaCaptureHeaderFromMHA
  mhatomegacapture
  vtkPolyDataMySQLTextReader
  vtkPolyDataMySQLTextWriter
  kishmeshtextfilereader
  trackTextFilesReader
  GoDBTraceRowTest
  GoDBContourRowTest
  GoDBMeshRowTest
)

FOREACH( var ${QGOIO_EXAMPLES_SRC} )
  ADD_EXECUTABLE( ${var} ${var}.cxx )
  TARGET_LINK_LIBRARIES( ${var}
    QGoIO
  )
ENDFOREACH( var ${QGOIO_EXAMPLES_SRC} )

ADD_TEST( gofigurefileinfomultiindexcontainerhelperTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gofigurefileinfomultiindexcontainerhelper )

ADD_TEST( vtkPolyDataMySQLTextReaderTest1
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLTextReader
  ${TESTING_DATA_PATH}/contour.vtk
)

ADD_TEST( vtkPolyDataMySQLTextReaderTest2
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLTextReader
  ${TESTING_DATA_PATH}/sphere.vtk
)

ADD_TEST( vtkPolyDataMySQLTextWriterTest1
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLTextWriter
  ${TESTING_DATA_PATH}/contour.vtk
)

ADD_TEST( vtkPolyDataMySQLTextWriterTest2
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkPolyDataMySQLTextWriter
  ${TESTING_DATA_PATH}/sphere.vtk
)

ADD_TEST( GoDBContourRowTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/GoDBContourRowTest
)

ADD_TEST( GoDBMeshRowTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/GoDBMeshRowTest
)

ADD_TEST( GoDBTraceRowTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/GoDBTraceRowTest
)
