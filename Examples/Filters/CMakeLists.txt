SET( EXAMPLES_FILTER 
  ChanAndVeseSegmentationFilterTest2D 
  ChanAndVeseSegmentationFilterTest3D 
  )

FOREACH( var ${EXAMPLES_FILTER} )
  ADD_EXECUTABLE( ${var} ${var}.cxx )
  TARGET_LINK_LIBRARIES( ${var} ITKIO ITKQuadEdgeMesh ITKCommon )
ENDFOREACH( var ${EXAMPLES_FILTER} )
