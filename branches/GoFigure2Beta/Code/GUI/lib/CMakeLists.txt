ADD_SUBDIRECTORY( QGoImageView )

# FILE( GLOB QGoLIB_SRC *.cxx )
# FILE( GLOB QGoLIB_HDRS *.h )

SET( QGoLIB_SRC
  QGoLUTDialog.cxx
#   QGoImageFilterPluginBase.cxx
  QGoTabElementBase.cxx
  QGoTabImageView2D.cxx
  QGoTabImageView3D.cxx
  QGoMainWindow.cxx
)

SET( QGoLIB_HDRS
  QGoLUTDialog.h
#   QGoImageFilterPluginBase.h
  QGoTabElementBase.h
  QGoTabImageViewElementBase.h
  QGoTabImageView2D.h
  QGoTabImageView3D.h
  QGoMainWindow.h
)

SET( QGoMainWindow_UI
  go.ui
)

QT4_WRAP_UI( QGoMainWindow_UI_H ${QGoMainWindow_UI} )

QT4_WRAP_CPP( QGoLIB_MOC ${QGoLIB_HDRS} )

SET_SOURCE_FILES_PROPERTIES( ${QGoLIB_SRC} PROPERTIES
  OBJECT_DEPENDS "${QGoMainWindow_UI_H}"
  )

SET_SOURCE_FILES_PROPERTIES( ${QGoLIB_MOC} PROPERTIES
  OBJECT_DEPENDS "${QGoMainWindow_UI_H}"
  )

QT4_ADD_RESOURCES( QGoMainWindow_QRC
  ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/resources/axes.qrc
  )

ADD_LIBRARY( QGoLib
  ${QGoLIB_SRC}
  ${QGoLIB_MOC}
  ${QGoMainWindow_QRC}
)

TARGET_LINK_LIBRARIES( QGoLib
  QGoLib
  QGoIO
  QGoImageView
  itkQt
)
