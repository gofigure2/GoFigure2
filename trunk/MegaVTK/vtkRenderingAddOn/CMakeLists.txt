SET( vtkRenderingAddOn_SRCS 
  vtkViewImage.cxx 
  vtkImageBlendWithMask.cxx
  vtkViewImage2D.cxx
  vtkViewImage2DCommand.cxx
  vtkViewImage2DWithContourWidget.cxx
  vtkViewImage2DWithContourWidgetCollection.cxx
  vtkViewImage3D.cxx
  vtkInteractorStyleImage2D.cxx
  vtkImage3DCroppingBoxCallback.cxx
  vtkOrientationAnnotation.cxx
  vtkFillImageWithPolyData.cxx
  vtkOrientedBoxWidget.cxx
  )

ADD_LIBRARY( vtkRenderingAddOn 
  ${MegaVTK_LIBS_STYLE}
  ${vtkRenderingAddOn_SRCS}
  )

TARGET_LINK_LIBRARIES( vtkRenderingAddOn
  vtkCommon
  vtkRendering
  vtkWidgets
  vtkVolumeRendering
  vtkHybrid
  vtkImaging
  )

INSTALL_TARGETS(/lib vtkRenderingAddOn)

INSTALL_FILES(/include "(\\.h|\\.txx)$")
