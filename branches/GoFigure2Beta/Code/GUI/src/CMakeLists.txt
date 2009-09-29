# SET( QGoGUISRC_CXX
#   QGoMainWindow.cxx
#   QGoTabElementBase.cxx
#   QGoPluginManager.cxx
#   QGoTabImageView2D.cxx
#   QGoTabImageView3D.cxx
# )
#
# SET( QGoGUISRC_HDRS
#   QGoMainWindow.h
#   QGoTabElementBase.h
#   QGoTabImageView3D.h
#   QGoPluginManager.h
#   QGoTabImageView2D.h
#   QGoTabImageViewElementBase.h
# )
#
# SET( QGoMainWindow_UI
#   go.ui
# )
#
# QT4_WRAP_UI( QGoMainWindow_UI_H ${QGoMainWindow_UI} )
#
# QT4_WRAP_CPP( QGoGUISRC_MOC ${QGoGUISRC_HDRS} )
#
# SET_SOURCE_FILES_PROPERTIES( ${QGoGUISRC_CXX} PROPERTIES
#   OBJECT_DEPENDS "${QGoMainWindow_UI_H}"
#   )
#
# SET_SOURCE_FILES_PROPERTIES( ${QGoGUISRC_MOC} PROPERTIES
#   OBJECT_DEPENDS "${QGoMainWindow_UI_H}"
#   )
#
# QT4_ADD_RESOURCES( QGoMainWindow_QRC
#   ${GOFIGURE2_SOURCE_DIR}/Code/GUI/src/resources/axes.qrc
#   )
#
# SET( QGoGUISRC_SRC
#   ${QGoGUISRC_CXX}
#   ${QGoMainWindow_QRC}
#   ${QGoGUISRC_MOC}
# )
