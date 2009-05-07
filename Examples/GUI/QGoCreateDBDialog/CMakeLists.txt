QT4_WRAP_CPP( MY_MOC_SRC ${GOFIGURE2_SOURCE_DIR}/Code/GUI/qt/QGoCreateDBDialog.h )

ADD_EXECUTABLE( 
  QGoCreateDBDialogTest
  QGoCreateDBDialogTest.cxx 
  ${GOFIGURE2_SOURCE_DIR}/Code/GUI/qt/QGoCreateDBDialog.cxx
  ${GOFIGURE2_SOURCE_DIR}/Code/IO/CreateDataBaseHelper.cxx
  ${MY_MOC_SRC}
  )
TARGET_LINK_LIBRARIES( 
  QGoCreateDBDialogTest
  QMegaVTKAddOn2 
  ITKCommon
  ITKIO
  ITKBasicFilters
  ITKNumerics
  )
