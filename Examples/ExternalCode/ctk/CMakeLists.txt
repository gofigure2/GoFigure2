set( CTK_EXAMPLE_SRC
  ctkDoubleSlider
  ctkCollapsibleGroupBoxTest
)

foreach( var ${CTK_EXAMPLE_SRC} )

  add_executable( ${var} ${var}.cxx )
  target_link_libraries( ${var} ctk )

endforeach( var ${CTK_EXAMPLE_SRC} )


add_test( ctkDoubleSliderTest1
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ctkDoubleSlider
  1
)

add_test( ctkCollapsibleGroupBoxTest1
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ctkCollapsibleGroupBoxTest
  1
)
