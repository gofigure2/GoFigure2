ADD_EXECUTABLE( ImagePageView ImagePageView.cxx )
TARGET_LINK_LIBRARIES( ImagePageView 
  QMegaVTKAddOn2 
  ITKCommon
  ITKIO
  ITKBasicFilters
  ITKNumerics
)
