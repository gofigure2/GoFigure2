SET( QGOIMAGEVIEW_EXAMPLE_SRC
  qgoimageview2d
  qgoimageview3d
)

FOREACH( var ${QGOIMAGEVIEW_EXAMPLE_SRC} )

  ADD_EXECUTABLE( ${var} ${var} )
  TARGET_LINK_LIBRARIES( ${var}
    QGoImageView
  )

ENDFOREACH( var ${QGOIMAGEVIEW_EXAMPLE_SRC} )

ADD_TEST( qgoimageview2dTest
  ${EXECUTABLE_OUTPUT_PATH}/qgoimageview2d
  ${TESTING_DATA_PATH}/Circle.png
)

ADD_TEST( qgoimageview3dTest
  ${EXECUTABLE_OUTPUT_PATH}/qgoimageview3d
  ${TESTING_DATA_PATH}/Circle3d.mhd
)