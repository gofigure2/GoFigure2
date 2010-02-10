FILE( GLOB vtkRenderingAddOn_SRCS2 *.cxx )

INCLUDE_DIRECTORIES( BEFORE
  ${MEGAVTK2_BINARY_DIR}
  ${MEGAVTK2_BINARY_DIR}/vtkRenderingAddOn
)

ADD_LIBRARY( vtkRenderingAddOn2
  ${LIBS_STYLE}
  ${vtkRenderingAddOn_SRCS2}
  )

SET(vtkRenderingAddOn_LIBS2
  vtkCommon
  vtkRendering
  vtkWidgets
  vtkVolumeRendering
  vtkHybrid
  vtkImaging
  )

TARGET_LINK_LIBRARIES( vtkRenderingAddOn2
  ${vtkRenderingAddOn_LIBS2}
  )

## INSTALL_TARGETS(/lib vtkRenderingAddOn2)
## 
## INSTALL_FILES(/include "(\\.h|\\.txx)$")

INSTALL( TARGETS vtkRenderingAddOn2
  ARCHIVE DESTINATION lib COMPONENT libraries
  LIBRARY DESTINATION lib COMPONENT libraries
#  RUNTIME DESTINATION . COMPONENT Runtime
  )

