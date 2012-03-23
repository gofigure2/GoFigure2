#----------------------------------------------------------
# FIND DOXYGEN

find_package( Doxygen )

if( DOXYGEN_FOUND )
  if( DOXYGEN_DOT_FOUND )
    option( LINK_EXTERNAL_DOC
      "Should the doc be linked with external sources (VTK, ITK...)?"
      OFF
    )
    add_subdirectory( Documentation )

  else( DOXYGEN_DOT_FOUND )
    message( STATUS
      "Warning: Dot (from GraphViz) not found - Documentation will not ce generated"
    )
  endif( DOXYGEN_DOT_FOUND )

else( DOXYGEN_FOUND )
  message( STATUS
  "Warning: Doxygen not found - Documentation will not be created"
  )
endif( DOXYGEN_FOUND )
