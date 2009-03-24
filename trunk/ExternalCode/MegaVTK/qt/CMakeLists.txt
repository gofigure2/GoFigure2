SET( QMegaVTKAddOn_NAME
  QGoLUTDialog
  QColorImagePageView
  QImagePageView
  QImagePageViewTracer
  QSplitterchild 
  )

SET( QMegaVTKAddOn_SRC
  QGoLUTDialog.cxx
  QColorImagePageView.cxx
  QImagePageView.cxx
  QImagePageViewTracer.cxx
  QSplitterchild.cxx 
  )

FOREACH( src ${QMegaVTKAddOn_NAME} )
  QT4_WRAP_CPP( ${src}_MOC_SRC ${src}.h )
ENDFOREACH( src )

ADD_LIBRARY( QMegaVTKAddOn2 
  ${MegaVTK_LIBS_STYLE}
  ${QGoLUTDialog_MOC_SRC}
  ${QImagePageView_MOC_SRC}
  ${QImagePageViewTracer_MOC_SRC}
  ${QColorImagePageView_MOC_SRC}
  ${QMegaVTKAddOn_SRC}
  ${QSplitterchild_MOC_SRC} 
  )

TARGET_LINK_LIBRARIES( QMegaVTKAddOn2
  vtkRenderingAddOn2
  QVTK
  ${QT_LIBRARIES}
  )

INSTALL_TARGETS(/lib QMegaVTKAddOn2)

INSTALL_FILES(/include "(\\.h|\\.txx)$")
