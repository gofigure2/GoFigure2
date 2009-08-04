INCLUDE_DIRECTORIES(
${GOFIGURE2_SOURCE_DIR}/../branch/Unstable/3DCellSegmentation/CellPreprocess
)

SET( EXAMPLES_FILTER
  ChanAndVeseSegmentationFilterTest2D
  ChanAndVeseSegmentationFilterTest3D
  )

FOREACH( var ${EXAMPLES_FILTER} )
  ADD_EXECUTABLE( ${var} ${var}.cxx )
  TARGET_LINK_LIBRARIES( ${var} ITKIO ITKQuadEdgeMesh ITKCommon
    ITKBasicFilters vtkGraphics vtkRendering)
ENDFOREACH( var ${EXAMPLES_FILTER} )

ADD_TEST( ChanAndVeseSegmentationFilterTest2D
  ${EXECUTABLE_OUTPUT_PATH}/ChanAndVeseSegmentationFilterTest2D
  ${TESTING_DATA_PATH}/Circle.png
  temp.vtk
  49
  49
  49
  1
)

ADD_TEST( ChanAndVeseSegmentationFilterTest3D
  ${EXECUTABLE_OUTPUT_PATH}/ChanAndVeseSegmentationFilterTest3D
  ${TESTING_DATA_PATH}/Circle3D.mhd
  temp.vtk
  15
  15
  15
  14
  1
)

ADD_TEST( ChanAndVeseSegmentationFilterReal1Test2D
  ${EXECUTABLE_OUTPUT_PATH}/ChanAndVeseSegmentationFilterTest2D
  ${TESTING_DATA_PATH}/MultiphaseCells2D.png
  temp.vtk
  48
  48
  30
  1
)

ADD_TEST( ChanAndVeseSegmentationFilterReal2Test2D
  ${EXECUTABLE_OUTPUT_PATH}/ChanAndVeseSegmentationFilterTest2D
  ${TESTING_DATA_PATH}/MultiphaseCells2D.png
  temp.vtk
  66
  77
  30
  1
)

ADD_TEST( ChanAndVeseSegmentationFilterReal3Test2D
  ${EXECUTABLE_OUTPUT_PATH}/ChanAndVeseSegmentationFilterTest2D
  ${TESTING_DATA_PATH}/MultiphaseCells2D.png
  temp.vtk
  80
  40
  30
  1
)
