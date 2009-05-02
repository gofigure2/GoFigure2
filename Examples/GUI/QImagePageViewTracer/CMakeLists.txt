ADD_EXECUTABLE( ImagePageViewTracer ImagePageViewTracer.cxx )
TARGET_LINK_LIBRARIES( ImagePageViewTracer 
  QMegaVTKAddOn2 
  ITKCommon
  ITKIO
  ITKBasicFilters
  ITKNumerics
)
