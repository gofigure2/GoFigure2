SET( CTK_EXAMPLE_SRC
  ctkDoubleSlider
  ctkCollapsibleGroupBoxTest
)

FOREACH( var ${CTK_EXAMPLE_SRC} )

  ADD_EXECUTABLE( ${var} ${var}.cxx )
  TARGET_LINK_LIBRARIES( ${var} ctk )

ENDFOREACH( var ${CTK_EXAMPLE_SRC} )


ADD_TEST( ctkDoubleSliderTest1
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ctkDoubleSlider
  1
)

ADD_TEST( ctkCollapsibleGroupBoxTest1
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ctkCollapsibleGroupBoxTest
  1
)
