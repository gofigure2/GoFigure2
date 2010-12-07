SET( CTK_EXAMPLE_SRC
  ctkDoubleSlider
)

FOREACH( var ${CTK_EXAMPLE_SRC} )

  ADD_EXECUTABLE( ${var} ${var}.cxx )
  TARGET_LINK_LIBRARIES( ${var} ctk )

ENDFOREACH( var ${MEGAVTK_EXAMPLE_SRC} )


ADD_TEST( ctkDoubleSliderTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ctkDoubleSlider
  1
)