ADD_EXECUTABLE( qgolutdialog qgolutdialogtest.cxx )
TARGET_LINK_LIBRARIES( qgolutdialog 
  QMegaVTKAddOn2 
  ITKCommon
  ITKIO
  ITKBasicFilters
  ITKNumerics
  )
