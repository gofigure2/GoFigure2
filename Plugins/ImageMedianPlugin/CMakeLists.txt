set( CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins
)

add_definitions( -DQT_PLUGIN )

if( ${CMAKE_BUILD_TYPE} MATCHES "Debug" )
  add_definitions( -DQT_DEBUG )
else( ${CMAKE_BUILD_TYPE} MATCHES "Debug" )
  add_definitions( -DQT_NO_DEBUG )
endif( ${CMAKE_BUILD_TYPE} MATCHES "Debug" )

include_directories( BEFORE
  ${GOFIGURE2_SOURCE_DIR}/Interfaces
  ${CMAKE_CURRENT_BINARY_DIR}
)

set( MEDIANPLUGIN_HDRS
  medianplugin.h
  QCellPreprocess.h
  ${QGoGUIINTERFACES_HDRS}
  )

set( MEDIANPLUGIN_SRC
  medianplugin.cxx
  QCellPreprocess.cxx
  ${QGoGUIINTERFACES_SRC}
)

QT4_WRAP_UI( MEDIANPLUGIN_UI_HDRS
 CellPreprocess.ui
 )

QT4_WRAP_CPP( MEDIANPLUGIN_MOC
  ${MEDIANPLUGIN_HDRS}
  )

set_source_files_properties( ${MEDIANPLUGIN_SRC} PROPERTIES
  OBJECT_DEPENDS "${MEDIANPLUGIN_UI_HDRS}"
  )

set_source_files_properties( ${MEDIANPLUGIN_MOC} PROPERTIES
  OBJECT_DEPENDS "${MEDIANPLUGIN_UI_HDRS}"
  )

add_library( ImageMedianPlugin MODULE
  ${MEDIANPLUGIN_MOC}
  ${MEDIANPLUGIN_SRC}
)

add_dependencies( ImageMedianPlugin QGoGui )

target_link_libraries( ImageMedianPlugin
  QGoGui
  ITKBasicFilters
)

#add_executable( medianpluginExample medianpluginMain.cxx )
#target_link_libraries( medianpluginExample ${QT_LIBRARIES} )

file( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
file( GLOB __source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/*.txx" )
file( GLOB __binary_file_h "${CMAKE_CURRENT_BINARY_DIR}/*.h" )
file( GLOB __binary_file_txx "${CMAKE_CURRENT_BINARY_DIR}/*.txx" )

install(
  FILES ${__source_file_h} ${__source_file_txx}
  ${__binary_file_h} ${__binary_file_txx}
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)
