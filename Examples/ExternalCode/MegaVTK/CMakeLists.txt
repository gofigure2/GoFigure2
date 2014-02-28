set( MEGAVTK_EXAMPLE_SRC
  imageview2d
  imageview3d
  imageviewcollection
  vtkviewimage3dtest
  SeedWidget
)

foreach( var ${MEGAVTK_EXAMPLE_SRC} )

  add_executable( ${var} ${var}.cxx )
  target_link_libraries( ${var} vtkRenderingAddOn2 )

endforeach( var ${MEGAVTK_EXAMPLE_SRC} )


add_test( imageview2dTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageview2d
  ${TESTING_DATA_PATH}/Circle.png
  1
)

add_test( imageview3dTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageview3d
  ${TESTING_DATA_PATH}/Circle3D.mhd
  1
)

add_test( vtkviewimage3dtest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkviewimage3dtest
  ${TESTING_DATA_PATH}/Circle3D.mhd
  1
)

add_test( imageviewcollectionTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageviewcollection
  ${TESTING_DATA_PATH}/Circle3D.mhd
  1
)

add_test( SeedWidgetTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SeedWidget
  ${TESTING_DATA_PATH}/Circle3D.mhd
  1
)
