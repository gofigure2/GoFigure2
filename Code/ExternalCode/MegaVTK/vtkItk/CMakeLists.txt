file( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
file( GLOB __source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/*.txx" )

install(
  FILES ${__source_file_h} ${__source_file_txx}
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)
