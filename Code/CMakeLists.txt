SET( GOFIGURE2_FILTER_SOURCE_DIR
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters
)

SET( GOFIGURE2_FILTER_CONTOUR_DIR
  ${GOFIGURE2_FILTER_SOURCE_DIR}/Contour
)

SET( GOFIGURE2_FILTER_MESH_DIR
  ${GOFIGURE2_FILTER_SOURCE_DIR}/Mesh
)

SET( GOFIGURE2_FILTER_SOURCE
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkCellFeatureGenerator.h
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkCellFeatureGenerator.txx
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkCellForegroundExtraction.h
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkCellForegroundExtraction.txx
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkChanAndVeseSegmentationFilter.h
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkGaussianProfileMatchingImageFilter.h
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkGaussianProfileMatchingImageFilter.txx
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkGradientWeightedDistanceImageFilter.h
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkGradientWeightedDistanceImageFilter.txx
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkGradientWeightedIntensityImageFilter.h
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkGradientWeightedIntensityImageFilter.txx
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkMorphologicalWatershedFromMarkersImageFilter2.h
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkMorphologicalWatershedFromMarkersImageFilter2.txx
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkMorphologicalWatershedImageFilter2.h
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkMorphologicalWatershedImageFilter2.txx
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkMultiScaleLoGDistanceImageFilter.h
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkMultiScaleLoGDistanceImageFilter.txx
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkMultiScaleLoGImageFilter.h
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkMultiScaleLoGImageFilter.txx
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkPreprocessImageFilter.h
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkPreprocessImageFilter.txx
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkViscousWatershedTransform.h
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkViscousWatershedTransform.txx
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkWatershedBasedCellSegmentation.h
  ${GOFIGURE2_FILTER_SOURCE_DIR}/itkWatershedBasedCellSegmentation.txx

#
# Contour
#
  ${GOFIGURE2_FILTER_CONTOUR_DIR}/ContourToMeshFilter.h
  ${GOFIGURE2_FILTER_CONTOUR_DIR}/ContourToMeshFilter.txx

#
# Mesh
#
  ${GOFIGURE2_FILTER_MESH_DIR}/itkConvertMeshesToLabelImageFilter.h
  ${GOFIGURE2_FILTER_MESH_DIR}/itkConvertMeshesToLabelImageFilter.txx
  ${GOFIGURE2_FILTER_MESH_DIR}/itkExtractMeshesFromLabelImageFilter.h
  ${GOFIGURE2_FILTER_MESH_DIR}/itkExtractMeshesFromLabelImageFilter.txx
  ${GOFIGURE2_FILTER_MESH_DIR}/itkQuadEdgeMeshTovtkPolyData.h
  ${GOFIGURE2_FILTER_MESH_DIR}/itkQuadEdgeMeshTovtkPolyData.txx
  ${GOFIGURE2_FILTER_MESH_DIR}/itkvtkPolyDataToBinaryMaskImageFilter.h
  ${GOFIGURE2_FILTER_MESH_DIR}/itkvtkPolyDataToBinaryMaskImageFilter.txx

  # Attributes
  ${GOFIGURE2_FILTER_MESH_DIR}/Attributes/itkBinaryMaskImageToGoFigureMeshAttributes.h
  ${GOFIGURE2_FILTER_MESH_DIR}/Attributes/itkBinaryMaskImageToGoFigureMeshAttributes.txx
  ${GOFIGURE2_FILTER_MESH_DIR}/Attributes/itkvtkPolyDataToGoFigureMeshAttributes.h
  ${GOFIGURE2_FILTER_MESH_DIR}/Attributes/itkvtkPolyDataToGoFigureMeshAttributes.txx

  # Merge
  ${GOFIGURE2_FILTER_MESH_DIR}/Merge/itkvtkMeshMergeFilterBase.h
  ${GOFIGURE2_FILTER_MESH_DIR}/Merge/itkvtkMeshMergeFilterBase.txx
  ${GOFIGURE2_FILTER_MESH_DIR}/Merge/itkvtkMeshMergeConvexHullFilter.h
  ${GOFIGURE2_FILTER_MESH_DIR}/Merge/itkvtkMeshMergeConvexHullFilter.txx

  # Split
  ${GOFIGURE2_FILTER_MESH_DIR}/Split/itkvtkMeshSplitterDanielssonDistanceImageFilter.h
  ${GOFIGURE2_FILTER_MESH_DIR}/Split/itkvtkMeshSplitterDanielssonDistanceImageFilter.txx
  ${GOFIGURE2_FILTER_MESH_DIR}/Split/itkvtkMeshSplitterFilterBase.txx
  ${GOFIGURE2_FILTER_MESH_DIR}/Split/itkvtkMeshSplitterFilterBase.h
  ${GOFIGURE2_FILTER_MESH_DIR}/Split/itkvtkMeshSplitterImageFilterBase.h
  ${GOFIGURE2_FILTER_MESH_DIR}/Split/itkvtkMeshSplitterImageFilterBase.txx
)

ADD_SUBDIRECTORY( ExternalCode )
ADD_SUBDIRECTORY( Database )
ADD_SUBDIRECTORY( IO )
ADD_SUBDIRECTORY( Filters )
ADD_SUBDIRECTORY( GUI )


