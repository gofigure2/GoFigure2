SET( QGOIO_EXAMPLES_SRC
  megacaptureimport
  multifilereader
  qgothreadedmultifilereader
)

FOREACH( var ${QGOIO_EXAMPLES_SRC} )
  ADD_EXECUTABLE( ${var} ${var}.cxx )
  TARGET_LINK_LIBRARIES( ${var}
    QGoIO
    ${QT_LIBRARIES}
  )
ENDFOREACH( var ${QGOIO_EXAMPLES_SRC} )