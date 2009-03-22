SET( QMegaVTKAddOn_NAME
  QGoLUTDialog
  QImagePageView
  QImagePageViewTracer
  QSplitterchild )

SET( QMegaVTKAddOn_SRC
  QGoLUTDialog.cxx
  QImagePageView.cxx
  QImagePageViewTracer.cxx
  QSplitterchild.cxx )

FOREACH( src ${QMegaVTKAddOn_NAME} )
  QT4_WRAP_CPP( ${src}_MOC_SRC ${src}.h )
ENDFOREACH( src )

ADD_LIBRARY( QMegaVTKAddOn ${MegaVTK_LIBS_STYLE}
  ${QGoLUTDialog_MOC_SRC}
  ${QImagePageView_MOC_SRC}
  ${QImagePageViewTracer_MOC_SRC}
  ${QMegaVTKAddOn_SRC}
  ${QSplitterchild_MOC_SRC} )

TARGET_LINK_LIBRARIES(QMegaVTKAddOn
    QMegaVTKAddOn
    vtkRenderingAddOn
    QVTK
    ${QT_LIBRARIES})

INSTALL_TARGETS(/lib QMegaVTKAddOn)

INSTALL_FILES(/include "(\\.h|\\.txx)$")
