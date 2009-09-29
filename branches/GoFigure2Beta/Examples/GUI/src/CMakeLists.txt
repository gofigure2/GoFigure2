SET( QGoLIB_EXAMPLES_SRC
  qgotabelementbase
  qgotabimageviewelementbase
  qgotabimageview2d
)

INCLUDE_DIRECTORIES( ${GOFIGURE2_BINARY_DIR}/Examples/GUI/src/  )

QT4_WRAP_UI( QGoMainWindow_UI_H ${QGoMainWindow_UI} )

QT4_WRAP_CPP( QGoGUISRC_MOC ${QGoGUISRC_HDRS} )

SET_SOURCE_FILES_PROPERTIES( ${QGoGUISRC_CXX} PROPERTIES
  OBJECT_DEPENDS "${QGoMainWindow_UI_H}"
  )

SET_SOURCE_FILES_PROPERTIES( ${QGoGUISRC_MOC} PROPERTIES
  OBJECT_DEPENDS "${QGoMainWindow_UI_H}"
  )

QT4_ADD_RESOURCES( QGoMainWindow_QRC
  ${GOFIGURE2_SOURCE_DIR}/Code/GUI/src/resources/axes.qrc
  )

FOREACH( var ${QGoLIB_EXAMPLES_SRC} )

#   MESSAGE( SEND_ERROR ${QGoGUI_SRC} )
  ADD_EXECUTABLE( ${var}
    ${var}.cxx
    ${QGoGUI_SRC}
    ${QGoGUISRC_MOC}
    ${QGoMainWindow_QRC}
  )
  TARGET_LINK_LIBRARIES( ${var}
    QGoGui
    QGoIO
    ITKBasicFilters
    ITKNumerics
    ITKCommon
    ITKIO
    itkQt
  )

ENDFOREACH( var ${QGoLIB_EXAMPLES_SRC} )

ADD_TEST( qgotabelementbaseTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgotabelementbase
)

ADD_TEST( qgotabimageviewelementbaseTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgotabimageviewelementbase
)

