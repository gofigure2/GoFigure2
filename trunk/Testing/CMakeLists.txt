ADD_EXECUTABLE( QGoLUTDialogTest 
  qgolutdialogtest.cxx ) 
TARGET_LINK_LIBRARIES( QGoLUTDialogTest
  vtkVisuManagement
#  QGOLUT
  QMegaVTKAddOn
  ITKCommon
  ITKIO
  ITKBasicFilters )
