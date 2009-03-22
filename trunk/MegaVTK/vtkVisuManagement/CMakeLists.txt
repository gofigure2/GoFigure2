SET(vtkVisuManagement_SRCS
 # vtkLimitFibersToVOI.cxx
 #vtkLimitFibersToROI.cxx
 #vtkSqueezeFibers.cxx
 #vtkFibersManager.cxx
 #vtkImageToIsosurface.cxx
 #vtkIsosurfaceManager.cxx
 #vtkPolyDataManager.cxx
 #vtkImageMerge.cxx
 #vtkFlipTensorImageFilter.cxx
 #vtkCompareImageManager.cxx
 vtkLookupTableManager.cxx
 #vtkTensorVisuManager.cxx
 #vtkTensorManager.cxx
 #vtkVectorVisuManager.cxx
 #vtkLimitVectorsToVOI.cxx
 #vtkDatasetToImageGenerator.cxx
 #vtkImageClamp.cxx
 #vtkImageFuse.cxx
 #vtkImageClippingPlane.cxx
 #vtkDisplacementFieldToGridFilter.cxx
 #vtkMeshCropManager.cxx
)

# C++ library for the InterfaceManagers
ADD_LIBRARY( vtkVisuManagement 
  ${MegaVTK_LIBS_STYLE}
  ${vtkVisuManagement_SRCS}
  )

TARGET_LINK_LIBRARIES( vtkVisuManagement
 vtkCommon
 vtkFiltering
 vtkRendering
 vtkWidgets
 vtkRenderingAddOn
)

INSTALL_FILES(/include "(\\.h|\\.txx)$")
INSTALL_TARGETS(/lib vtkVisuManagement)


