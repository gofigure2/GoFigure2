IF( DOXYGEN_FOUND )
      IF( DOXYGEN_DOT_FOUND )

        OPTION( LINK_EXTERNAL_DOC
          "Should the doc be linked with external sources (VTK, ITK...)?" 
          OFF 
        )

        ADD_SUBDIRECTORY( Documentation )

      ELSE( DOXYGEN_DOT_FOUND )
        MESSAGE( STATUS 
          "Warning: Dot (from GraphViz) not found - Documentation will not ce generated" 
        )
      ENDIF( DOXYGEN_DOT_FOUND )
    ELSE( DOXYGEN_FOUND )
      MESSAGE( STATUS 
        "Warning: Doxygen not found - Documentation will not be created"
      )
    ENDIF( DOXYGEN_FOUND )
