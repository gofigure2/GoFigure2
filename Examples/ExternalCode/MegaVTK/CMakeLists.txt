SET( EXAMPLES_QT_NOT_REQUIRED
  RenderBoxContent3D
  )

FOREACH( var ${EXAMPLES_QT_NOT_REQUIRED} )

  ADD_EXECUTABLE( ${var}
    ${var}.cxx
  )
  TARGET_LINK_LIBRARIES( ${var}
    QGoIO vtkRenderingAddOn2
  )

ENDFOREACH( var )
