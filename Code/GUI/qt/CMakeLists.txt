INCLUDE_DIRECTORIES( BEFORE
  ${MEGAVTK2_SOURCE_DIR}
  ${MEGAVTK2_BINARY_DIR}
  ${MEGAVTK2_SOURCE_DIR}/vtkRenderingAddOn
  ${MEGAVTK2_BINARY_DIR}/vtkRenderingAddOn
)

SET(QGOPRINTDATABASE_UIS
QGoPrintDatabase.ui
)

QT4_WRAP_UI(QGOPRINTDATABASE_UIS_H ${QGOPRINTDATABASE_UIS})
#QT4_WRAP_CPP(QGOPRINTDATABASE_MOC_UI ${QGOPRINTDATABASE_UIS_H})

SET( QMegaVTKAddOn_NAME2
  QGoLUTDialog
  QGoCreateDBDialog
  QGoImageInfo
  QImagePageView
  QImagePageViewTracer
  QImagePageView4DTracer
  QSplitterchild
  QGoPrintDatabase
  QGoWizardDB	
  )

SET( QMegaVTKAddOn_SRC2
  QGoLUTDialog.cxx
  QGoCreateDBDialog.cxx
  QGoImageInfo.cxx
  QImagePageView.cxx
  QImagePageViewTracer.cxx
  QImagePageView4DTracer.cxx
  QSplitterchild.cxx
  QGoPrintDatabase.cxx
  QGoWizardDB.cxx
  )

FOREACH( src ${QMegaVTKAddOn_NAME2} )
  QT4_WRAP_CPP( ${src}_MOC_SRC2 ${src}.h )
ENDFOREACH( src )

ADD_LIBRARY( QMegaVTKAddOn2
  ${LIBS_STYLE}
  ${QGoLUTDialog_MOC_SRC2}
  ${QGoCreateDBDialog_MOC_SRC2}
  ${QGoImageInfo_MOC_SRC2}
  ${QImagePageView_MOC_SRC2}
  ${QImagePageViewTracer_MOC_SRC2}
  ${QImagePageView4DTracer_MOC_SRC2}
  ${QMegaVTKAddOn_SRC2}
  ${QSplitterchild_MOC_SRC2}
  ${QGoPrintDatabase_MOC_SRC2}
  ${QGOPRINTDATABASE_UIS_H}
  ${QGoWizardDB_MOC_SRC2}		
  ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/vtkLSM/vtkLSMReader.cxx
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
