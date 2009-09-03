SET( MEGAVTK_EXAMPLE_SRC
  imageview2d
  imageview3d
  imageviewcollection
)

FOREACH( var ${MEGAVTK_EXAMPLE_SRC} )

  ADD_EXECUTABLE( ${var} ${var}.cxx )
  TARGET_LINK_LIBRARIES( ${var} vtkRenderingAddOn2 )

ENDFOREACH( var ${MEGAVTK_EXAMPLE_SRC} )


ADD_TEST( imageview2dTEST
  ${EXECUTABLE_OUTPUT_PATH}/imageview2d
  ${TESTING_DATA_PATH}/Circle.png
  1
)

ADD_TEST( imageview3dTEST
  ${EXECUTABLE_OUTPUT_PATH}/imageview3d
  ${TESTING_DATA_PATH}/Circle3d.mhd
  1
)

ADD_TEST( imageviewcollectionTEST
  ${EXECUTABLE_OUTPUT_PATH}/imageviewcollection
  ${TESTING_DATA_PATH}/Circle3d.mhd
  1
)