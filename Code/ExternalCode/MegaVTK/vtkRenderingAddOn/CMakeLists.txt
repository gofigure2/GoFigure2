#Define sources
set( vtkRenderingAddOn_SRCS2
  vtkFillImageWithPolyData.cxx
  vtkImage3DCroppingBoxCallback.cxx
  vtkImage3DImagePlaneCallback.cxx
  vtkImageBlendWithMask.cxx
  vtkInteractorStyleImage2D.cxx
  vtkInteractorStyleImage3D.cxx
  vtkLookupTableManager.cxx
  vtkOrientationAnnotation.cxx
  vtkOrientedBoxWidget.cxx
  vtkPolylineDecimation.cxx
  vtkViewImage.cxx
  vtkViewImage2D.cxx
  vtkViewImage2DCollection.cxx
  vtkViewImage2DCollectionCommand.cxx
  vtkViewImage2DCommand.cxx
  vtkViewImage3D.cxx
  vtkViewImage3DCommand.cxx
  )

include_directories( BEFORE
  ${MEGAVTK2_BINARY_DIR}
  ${MEGAVTK2_BINARY_DIR}/vtkRenderingAddOn
)

add_library( vtkRenderingAddOn2
  ${vtkRenderingAddOn_SRCS2}
  )

set(vtkRenderingAddOn_LIBS2
  vtkCommon
  vtkRendering
  vtkWidgets
  vtkVolumeRendering
  vtkHybrid
  vtkImaging
  )

target_link_libraries( vtkRenderingAddOn2
  ${vtkRenderingAddOn_LIBS2}
  )

set_target_properties( vtkRenderingAddOn2 PROPERTIES VERSION ${GOFIGURE2_LIB_VERSION} SOVERSION ${GOFIGURE2_LIB_VERSION} )

# Runtime
install( TARGETS vtkRenderingAddOn2
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} NAMELINK_SKIP COMPONENT Libraries
  )

# Development
install( TARGETS vtkRenderingAddOn2
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries NAMELINK_ONLY
  )

file( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
file( GLOB __source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/*.txx" )
file( GLOB __lut_file_h "${CMAKE_CURRENT_SOURCE_DIR}/lut/*.h" )

install(
  FILES ${__source_file_h} ${__source_file_txx} ${__lut_file_h}
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)

