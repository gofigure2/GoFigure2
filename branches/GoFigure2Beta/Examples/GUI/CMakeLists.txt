ADD_SUBDIRECTORY( lib )

SET( QGoLIB_EXAMPLES_SRC
  qgotabelementbase
  qgolutdialog
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

ADD_TEST( qgolutdialogtest
  ${EXECUTABLE_OUTPUT_PATH}/qgolutdialog
  1
)