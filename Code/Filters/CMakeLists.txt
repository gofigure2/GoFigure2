# for the installation
file( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
file( GLOB __source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/*.txx" )

file( GLOB __contour_source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/Contour/*.h" )
file( GLOB __contour_source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/Contour/*.txx" )

file( GLOB __mesh_source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/*.h" )
file( GLOB __mesh_source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/*.txx" )

file( GLOB __mesh_att_source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Attributes/*.h" )
file( GLOB __mesh_att_source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Attributes/*.txx" )

file( GLOB __mesh_split_source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Split/*.h" )
file( GLOB __mesh_split_source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Split/*.txx" )

file( GLOB __mesh_merge_source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Merge/*.h" )
file( GLOB __mesh_merge_source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Merge/*.txx" )

install( FILES ${__source_file_h} ${__source_file_txx}
  ${__contour_source_file_h} ${__contour_source_file_txx}
  ${__mesh_source_file_h} ${__mesh_source_file_txx}
  ${__mesh_att_source_file_h} ${__mesh_att_source_file_txx}
  ${__mesh_split_source_file_h} ${__mesh_split_source_file_txx}
  ${__mesh_merge_source_file_h} ${__mesh_merge_source_file_txx}
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)

