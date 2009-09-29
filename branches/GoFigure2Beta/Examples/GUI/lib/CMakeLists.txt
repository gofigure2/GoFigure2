SET( QGOGUILIB_EXAMPLE_SRC
  qgoimageview2d
  qgoimageview3d
  qgolutdialog
)

FOREACH( var ${QGOGUILIB_EXAMPLE_SRC} )

  ADD_EXECUTABLE( ${var} ${var} )
  TARGET_LINK_LIBRARIES( ${var}
    QGoGui
  )

ENDFOREACH( var ${QGOGUILIB_EXAMPLE_SRC} )

ADD_TEST( qgoimageview2dTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgoimageview2d
  ${TESTING_DATA_PATH}/Circle.png
  1
)

ADD_TEST( qgoimageview3dTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgoimageview3d
  ${TESTING_DATA_PATH}/Circle3D.mhd
  1
)

ADD_TEST( qgolutdialogTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgolutdialog
  1
)
