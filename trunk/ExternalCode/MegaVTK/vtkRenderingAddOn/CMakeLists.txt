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
  vtkLookupTableManager.cxx
  )

ADD_LIBRARY( vtkRenderingAddOn2 
  ${MegaVTK_LIBS_STYLE}
  ${vtkRenderingAddOn_SRCS}
  )

TARGET_LINK_LIBRARIES( vtkRenderingAddOn2
  vtkCommon
  vtkRendering
  vtkWidgets
  vtkVolumeRendering
  vtkHybrid
  vtkImaging
  )

INSTALL_TARGETS(/lib vtkRenderingAddOn2)

INSTALL_FILES(/include "(\\.h|\\.txx)$")
