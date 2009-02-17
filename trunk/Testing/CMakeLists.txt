ADD_EXECUTABLE( QGoLUTDialogTest qgolutdialogtest.cxx )
TARGET_LINK_LIBRARIES( QGoLUTDialogTest
  vtkVisuManagement
  QGO
  QMegaVTKAddOn
  ITKCommon
  ITKIO
  ITKBasicFilters )