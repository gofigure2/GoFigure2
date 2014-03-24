#----------------------------------------------------------
# FIND DOXYGEN

find_package( Doxygen )

if( DOXYGEN_FOUND )

  if( DOXYGEN_DOT_FOUND )

    add_subdirectory( Documentation )

  else( DOXYGEN_DOT_FOUND )
    message( WARNING
      "Dot (from GraphViz) not found - Documentation will not ce generated"
    )
  endif( DOXYGEN_DOT_FOUND )

else( DOXYGEN_FOUND )
  message( WARNING
    "Doxygen not found - Documentation will not be created"
  )
endif( DOXYGEN_FOUND )
