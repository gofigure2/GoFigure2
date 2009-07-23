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
  50
  50
  10
  1
)

ADD_TEST( ChanAndVeseSegmentationFilterTest3D
  ${EXECUTABLE_OUTPUT_PATH}/ChanAndVeseSegmentationFilterTest3D
  ${TESTING_DATA_PATH}/Circle3D.mhd
  15
  15
  15
  14
  1
)

ADD_TEST( ChanAndVeseSegmentationFilterTest2D
  ${EXECUTABLE_OUTPUT_PATH}/ChanAndVeseSegmentationFilterTest2D
  ${TESTING_DATA_PATH}/Multiphase2D.png
  48
  48
  30
  1
)

ADD_TEST( ChanAndVeseSegmentationFilterTest2D
  ${EXECUTABLE_OUTPUT_PATH}/ChanAndVeseSegmentationFilterTest2D
  ${TESTING_DATA_PATH}/Multiphase2D.png
  66
  77
  30
  1
)

ADD_TEST( ChanAndVeseSegmentationFilterTest2D
  ${EXECUTABLE_OUTPUT_PATH}/ChanAndVeseSegmentationFilterTest2D
  ${TESTING_DATA_PATH}/Multiphase2D.png
  80
  40
  30
  1
)
