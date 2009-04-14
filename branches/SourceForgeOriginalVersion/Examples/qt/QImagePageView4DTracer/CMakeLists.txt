ADD_EXECUTABLE( ImagePageView4DTracer 
  ImagePageView4DTracer.cxx 
  ../../../ExternalCode/vtkLSM/vtkLSMReader.cxx
  )
TARGET_LINK_LIBRARIES( ImagePageView4DTracer 
  QMegaVTKAddOn2 
  ITKCommon
  ITKIO
  ITKBasicFilters
  ITKNumerics
)
