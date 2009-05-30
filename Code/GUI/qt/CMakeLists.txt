INCLUDE_DIRECTORIES( BEFORE
  ${MEGAVTK2_SOURCE_DIR}
  ${MEGAVTK2_BINARY_DIR}
  ${MEGAVTK2_SOURCE_DIR}/vtkRenderingAddOn
  ${MEGAVTK2_BINARY_DIR}/vtkRenderingAddOn
)

SET(QPRINTDATABASE_UIS
QPrintDatabase.ui
)

QT4_WRAP_UI(QPRINTDATABASE_UIS_H ${QPRINTDATABASE_UIS})
QT4_WRAP_CPP(QPRINTDATABASE_MOC_UI ${QPRINTDATABASE_UIS_H})

SET( QMegaVTKAddOn_NAME2
  QGoImageInfo
  QGoLUTDialog
  QGoCreateDBDialog
  QImagePageView
  QImagePageViewTracer
  QImagePageView4DTracer
  QSplitterchild
  QPrintDatabase
  QGoWizardDB	
  )

SET( QMegaVTKAddOn_SRC2
  QGoImageInfo.cxx
  QGoLUTDialog.cxx
  QGoCreateDBDialog.cxx
  QImagePageView.cxx
  QImagePageViewTracer.cxx
  QImagePageView4DTracer.cxx
  QSplitterchild.cxx
  QPrintDatabase.cxx
  QGoWizardDB.cxx
  )

FOREACH( src ${QMegaVTKAddOn_NAME2} )
  QT4_WRAP_CPP( ${src}_MOC_SRC2 ${src}.h )
ENDFOREACH( src )

ADD_LIBRARY( QMegaVTKAddOn2
  ${LIBS_STYLE}
  ${QGoImageInfo_MOC_SRC2}
  ${QGoCreateDBDialog_MOC_SRC2}
  ${QGoLUTDialog_MOC_SRC2}
  ${QImagePageView_MOC_SRC2}
  ${QImagePageViewTracer_MOC_SRC2}
  ${QImagePageView4DTracer_MOC_SRC2}
  ${QMegaVTKAddOn_SRC2}
  ${QSplitterchild_MOC_SRC2}
  ${QPrintDatabase_MOC_SRC2}
  ${QPRINTDATABASE_MOC_UI}
  ${QGoWizardDB_MOC_SRC2}		
  ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/vtkLSM/vtkLSMReader.cxx
#   ${GOFIGURE2_SOURCE_DIR}/Code/IO/CreateDataBaseHelper.cxx
  )

SET( QMegaVTKAddOn2_LIBS
  QGoIO
  vtkIO
  ITKIO
  ITKCommon
  QMegaVTKAddOn2
  vtkRenderingAddOn2
  QVTK
  ${QT_LIBRARIES}
 )

TARGET_LINK_LIBRARIES( QMegaVTKAddOn2
  ${QMegaVTKAddOn2_LIBS}
  )

INSTALL_TARGETS(/lib QMegaVTKAddOn2)

INSTALL_FILES(/include "(\\.h|\\.txx)$")
