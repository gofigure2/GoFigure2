# add subdirectories
add_subdirectory( ExternalCode )
add_subdirectory( Database )
add_subdirectory( IO )
add_subdirectory( Filters )
add_subdirectory( GUI )


file( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )

install( FILES ${__source_file_h}
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)
