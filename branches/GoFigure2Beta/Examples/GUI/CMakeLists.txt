ADD_SUBDIRECTORY( lib )

SET( QGoLIB_EXAMPLES_SRC
  qgolutdialog
  qgotabelementbase
  qgotabimageviewelementbase

)

FOREACH( var ${QGoLIB_EXAMPLES_SRC} )

  ADD_EXECUTABLE( ${var} ${var}.cxx )
  TARGET_LINK_LIBRARIES( ${var}
    QGoLib
  )

ENDFOREACH( var ${QGoLIB_EXAMPLES_SRC} )

ADD_TEST( qgotabelementbaseTest
  ${EXECUTABLE_OUTPUT_PATH}/qgotabelementbase
)

ADD_TEST( qgotabimageviewelementbaseTest
  ${EXECUTABLE_OUTPUT_PATH}/qgotabimageviewelementbase
)

ADD_TEST( qgolutdialogtest
  ${EXECUTABLE_OUTPUT_PATH}/qgolutdialog
  1
)