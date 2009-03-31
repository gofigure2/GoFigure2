INCLUDE_DIRECTORIES( BEFORE
  ${MEGAVTK2_SOURCE_DIR}
  ${MEGAVTK2_BINARY_DIR}
  ${MEGAVTK2_SOURCE_DIR}/vtkRenderingAddOn
  ${MEGAVTK2_BINARY_DIR}/vtkRenderingAddOn
)


SET( QMegaVTKAddOn_NAME2
  QGoLUTDialog
  QImagePageView
  QImagePageViewTracer
  QSplitterchild
  )

SET( QMegaVTKAddOn_SRC2
  QGoLUTDialog.cxx
  QImagePageView.cxx
  QImagePageViewTracer.cxx
  QSplitterchild.cxx
  )

FOREACH( src ${QMegaVTKAddOn_NAME2} )
  QT4_WRAP_CPP( ${src}_MOC_SRC2 ${src}.h )
ENDFOREACH( src )

ADD_LIBRARY( QMegaVTKAddOn2
  ${LIBS_STYLE}
  ${QGoLUTDialog_MOC_SRC2}
  ${QImagePageView_MOC_SRC2}
  ${QImagePageViewTracer_MOC_SRC2}
  ${QMegaVTKAddOn_SRC2}
  ${QSplitterchild_MOC_SRC2}
  )

SET( QMegaVTKAddOn2_LIBS
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
