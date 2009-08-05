SET( PREPROCESSING_EXAMPLES 
  CellPreprocess2D 
  CellPreprocess3D
)

FOREACH( var ${PREPROCESSING_EXAMPLES} )
  ADD_EXECUTABLE( ${var} ${var}.cxx )
  TARGET_LINK_LIBRARIES( ${var} ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO )
ENDFOREACH( var ${PREPROCESSING_EXAMPLES} )

#ADD_TEST( CellPreprocess2DTest 
#  ${EXECUTABLE_OUTPUT_PATH}/CellPreprocess2D 
#  ${TESTING_DATA_PATH}/30.png 
#  temp.mha 
#)
#
#ADD_TEST( CellPreprocess3DTest
#  ${EXECUTABLE_OUTPUT_PATH}/CellPreprocess3D
#  ${TESTING_DATA_PATH}/
#  temp.mha
#)

