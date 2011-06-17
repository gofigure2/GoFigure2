# create library
IF( WIN32 )
  IF( NOT CYGWIN )
    IF( NOT MINGW )
      IF( BUILD_SHARED_LIBS )
        ADD_DEFINITIONS( -DGoFilters_EXPORTS )
      ENDIF( BUILD_SHARED_LIBS )
    ENDIF( NOT MINGW )
  ENDIF( NOT CYGWIN )
ENDIF( WIN32 )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/GoFiltersConfigure.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/GoFiltersConfigure.h @ONLY IMMEDIATE)

SET( GoFilters_SRC
  itkCellFeatureGenerator.h
  itkCellFeatureGenerator.txx
  itkCellForegroundExtraction.h
  itkCellForegroundExtraction.txx
  itkChanAndVeseSegmentationFilter.h
  itkGaussianProfileMatchingImageFilter.h
  itkGaussianProfileMatchingImageFilter.txx
  itkGradientWeightedDistanceImageFilter.h
  itkGradientWeightedDistanceImageFilter.txx
  itkGradientWeightedIntensityImageFilter.h
  itkGradientWeightedIntensityImageFilter.txx
  itkMorphologicalWatershedFromMarkersImageFilter2.h
  itkMorphologicalWatershedFromMarkersImageFilter2.txx
  itkMorphologicalWatershedImageFilter2.h
  itkMorphologicalWatershedImageFilter2.txx
  itkMultiScaleLoGDistanceImageFilter.h
  itkMultiScaleLoGDistanceImageFilter.txx
  itkMultiScaleLoGImageFilter.h
  itkMultiScaleLoGImageFilter.txx
  itkPreprocessImageFilter.h
  itkPreprocessImageFilter.txx
  itkViscousWatershedTransform.h
  itkViscousWatershedTransform.txx
  itkWatershedBasedCellSegmentation.h
  itkWatershedBasedCellSegmentation.txx

#
# Contour
#
  Contour/ContourToMeshFilter.h
  Contour/ContourToMeshFilter.txx

#
# Mesh
#
  Mesh/itkConvertMeshesToLabelImageFilter.h
  Mesh/itkConvertMeshesToLabelImageFilter.txx
  Mesh/itkExtractMeshesFromLabelImageFilter.h
  Mesh/itkExtractMeshesFromLabelImageFilter.txx
  Mesh/itkQuadEdgeMeshTovtkPolyData.h
  Mesh/itkQuadEdgeMeshTovtkPolyData.txx
  Mesh/itkvtkPolyDataToBinaryMaskImageFilter.h
  Mesh/itkvtkPolyDataToBinaryMaskImageFilter.txx

  Mesh/itkvtkMeshFilterBase.txx
  Mesh/itkvtkMeshFilterBase.h

  Mesh/MeshToContourFilter.h
  Mesh/MeshToContourFilter.cxx

  # Attributes
  Mesh/Attributes/itkBinaryMaskImageToGoFigureMeshAttributes.h
  Mesh/Attributes/itkBinaryMaskImageToGoFigureMeshAttributes.txx
  Mesh/Attributes/itkvtkPolyDataToGoFigureMeshAttributes.h
  Mesh/Attributes/itkvtkPolyDataToGoFigureMeshAttributes.txx

  # Merge
  Mesh/Merge/itkvtkMeshMergeFilterBase.h
  Mesh/Merge/itkvtkMeshMergeFilterBase.txx
  Mesh/Merge/itkvtkMeshMergeConvexHullFilter.h
  Mesh/Merge/itkvtkMeshMergeConvexHullFilter.txx

  # Split
  Mesh/Split/itkvtkMeshSplitterDanielssonDistanceImageFilter.h
  Mesh/Split/itkvtkMeshSplitterDanielssonDistanceImageFilter.txx
  Mesh/Split/itkvtkMeshSplitterFilterBase.txx
  Mesh/Split/itkvtkMeshSplitterFilterBase.h
  Mesh/Split/itkvtkMeshSplitterImageFilterBase.h
  Mesh/Split/itkvtkMeshSplitterImageFilterBase.txx
)

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

ADD_LIBRARY( GoFilters
  ${GoFilters_SRC}
)


SET_TARGET_PROPERTIES( GoFilters PROPERTIES VERSION ${GOFIGURE2_LIB_VERSION} SOVERSION ${GOFIGURE2_LIB_VERSION} )

# Runtime
INSTALL( TARGETS GoFilters
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} NAMELINK_SKIP COMPONENT Libraries
  )

# Development
INSTALL( TARGETS GoFilters
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries NAMELINK_ONLY
  )

# for the installation
FILE( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
FILE( GLOB __source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/*.txx" )

INSTALL( FILES ${__source_file_h} ${__source_file_txx}
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)

