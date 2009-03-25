FILE( GLOB vtkRenderingAddOn_SRCS2 *.cxx )

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

INSTALL_TARGETS(/lib vtkRenderingAddOn2)

INSTALL_FILES(/include "(\\.h|\\.txx)$")
