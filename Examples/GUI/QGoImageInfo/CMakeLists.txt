ADD_EXECUTABLE( QGoImageInfoTest QGoImageInfoTest.cxx )
TARGET_LINK_LIBRARIES( QGoImageInfoTest
  QMegaVTKAddOn2
  ITKCommon
  ITKIO
  ITKBasicFilters
  ITKNumerics
  )
