ADD_EXECUTABLE( ColorImagePageView ColorImagePageView.cxx )
TARGET_LINK_LIBRARIES( ColorImagePageView
  QMegaVTKAddOn2 
  ITKCommon
  ITKIO
  ITKBasicFilters
  ITKNumerics
)
