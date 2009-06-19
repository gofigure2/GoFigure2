ADD_EXECUTABLE( ImagePageView4DTracer 
  ImagePageView4DTracer.cxx 
  )
TARGET_LINK_LIBRARIES( ImagePageView4DTracer 
  QMegaVTKAddOn2 
  QGoIO
  ITKCommon
  ITKIO
  ITKBasicFilters
  ITKNumerics
)
