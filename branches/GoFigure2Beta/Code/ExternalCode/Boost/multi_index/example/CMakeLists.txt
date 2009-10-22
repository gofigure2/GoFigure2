SET( EXAMPLE_SRC
  NewMegaCaptureMultiFileExample 
  basic
  complex_structs
  fun_key
  random_access
  sequenced
  bimap
  composite_keys
  hashed
  non_default_ctor
  rearrange
)

FOREACH( var ${EXAMPLE_SRC} )
  ADD_EXECUTABLE( ${var} ${var}.cpp )
ENDFOREACH( var ${EXAMPLE_SRC} )
