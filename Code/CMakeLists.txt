SET( GOFIGURE2_FILTER_SOURCE
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkCellFeatureGenerator.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkCellFeatureGenerator.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkCellForegroundExtraction.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkCellForegroundExtraction.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkChanAndVeseSegmentationFilter.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkGaussianProfileMatchingImageFilter.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkGaussianProfileMatchingImageFilter.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkGradientWeightedDistanceImageFilter.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkGradientWeightedDistanceImageFilter.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkGradientWeightedIntensityImageFilter.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkGradientWeightedIntensityImageFilter.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkMorphologicalWatershedFromMarkersImageFilter2.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkMorphologicalWatershedFromMarkersImageFilter2.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkMorphologicalWatershedImageFilter2.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkMorphologicalWatershedImageFilter2.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkMultiScaleLoGDistanceImageFilter.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkMultiScaleLoGDistanceImageFilter.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkMultiScaleLoGImageFilter.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkMultiScaleLoGImageFilter.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkPreprocessImageFilter.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkPreprocessImageFilter.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkViscousWatershedTransform.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkViscousWatershedTransform.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkWatershedBasedCellSegmentation.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/itkWatershedBasedCellSegmentation.txx

  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Contour/ContourToMeshFilter.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Contour/ContourToMeshFilter.txx

  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/itkConvertMeshesToLabelImageFilter.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/itkConvertMeshesToLabelImageFilter.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/itkExtractMeshesFromLabelImageFilter.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/itkExtractMeshesFromLabelImageFilter.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/itkQuadEdgeMeshTovtkPolyData.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/itkQuadEdgeMeshTovtkPolyData.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/itkvtkPolyDataToBinaryMaskImageFilter.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/itkvtkPolyDataToBinaryMaskImageFilter.txx

  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Merge/itkvtkMeshMergeFilterBase.h

#  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Split/itkSingleCellSplitImageFilter.h
#  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Split/itkSingleCellSplitImageFilter.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Split/itkvtkMeshSplitterDanielssonDistanceImageFilter.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Split/itkvtkMeshSplitterDanielssonDistanceImageFilter.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Split/itkvtkMeshSplitterFilterBase.txx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Split/itkvtkMeshSplitterFilterBase.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Split/itkvtkMeshSplitterImageFilterBase.h
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Split/itkvtkMeshSplitterImageFilterBase.txx
)

ADD_SUBDIRECTORY( ExternalCode )
ADD_SUBDIRECTORY( Database )
ADD_SUBDIRECTORY( IO )
ADD_SUBDIRECTORY( Filters )
ADD_SUBDIRECTORY( GUI )


