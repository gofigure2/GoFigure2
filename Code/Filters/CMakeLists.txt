# for the installation
FILE( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
FILE( GLOB __source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/*.txx" )

FILE( GLOB __contour_source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/Contour/*.h" )
FILE( GLOB __contour_source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/Contour/*.txx" )

FILE( GLOB __mesh_source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/*.h" )
FILE( GLOB __mesh_source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/*.txx" )

FILE( GLOB __mesh_att_source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Attributes/*.h" )
FILE( GLOB __mesh_att_source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Attributes/*.txx" )

FILE( GLOB __mesh_split_source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Split/*.h" )
FILE( GLOB __mesh_split_source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Split/*.txx" )

FILE( GLOB __mesh_merge_source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Merge/*.h" )
FILE( GLOB __mesh_merge_source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Merge/*.txx" )

INSTALL( FILES ${__source_file_h} ${__source_file_txx}
  ${__contour_source_file_h} ${__contour_source_file_txx}
  ${__mesh_source_file_h} ${__mesh_source_file_txx}
  ${__mesh_att_source_file_h} ${__mesh_att_source_file_txx}
  ${__mesh_split_source_file_h} ${__mesh_split_source_file_txx}
  ${__mesh_merge_source_file_h} ${__mesh_merge_source_file_txx}
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)

