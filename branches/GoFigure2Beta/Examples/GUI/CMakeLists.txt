ADD_SUBDIRECTORY( lib )

SET( QGoLIB_EXAMPLES_SRC
  qgotabelementbase
)

FOREACH( var ${QGoLIB_EXAMPLES_SRC} )

  ADD_EXECUTABLE( ${var} ${var}.cxx )
  TARGET_LINK_LIBRARIES( ${var}
    QGoLib
  )
ENDFOREACH( var ${QGoLIB_EXAMPLES_SRC} )