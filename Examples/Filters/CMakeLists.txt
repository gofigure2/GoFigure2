ADD_EXECUTABLE( itkvtkPolyDatatoBinaryMaskImageFilterTest
  itkvtkPolyDatatoBinaryMaskImageFilterTest.cxx )
TARGET_LINK_LIBRARIES( itkvtkPolyDatatoBinaryMaskImageFilterTest
  ITKIO
  ITKCommon
  vtkGraphics
  vtkHybrid
)
