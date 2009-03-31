ADD_EXECUTABLE( ImagePageViewTracer 
  ImagePageViewTracer.cxx 
  ../../../ExternalCode/vtkLSM/vtkLSMReader.cxx
  )
TARGET_LINK_LIBRARIES( ImagePageViewTracer 
  QMegaVTKAddOn2 
  ITKCommon
  ITKIO
  ITKBasicFilters
  ITKNumerics
)
