ADD_EXECUTABLE( itkvtkPolyDatatoBinaryMaskImageFilterTest
  itkvtkPolyDatatoBinaryMaskImageFilterTest.cxx )

TARGET_LINK_LIBRARIES( itkvtkPolyDatatoBinaryMaskImageFilterTest
  ITKIO
  ITKCommon
  vtkGraphics
  vtkHybrid
)

ADD_EXECUTABLE( itkBinaryMaskImageToGoFigureMeshAttributesTest
 itkBinaryMaskImageToGoFigureMeshAttributesTest.cxx )
TARGET_LINK_LIBRARIES( itkBinaryMaskImageToGoFigureMeshAttributesTest
  ITKIO
  ITKCommon
  ITKStatistics
)
