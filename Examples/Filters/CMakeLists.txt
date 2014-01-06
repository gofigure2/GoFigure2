add_executable( itkvtkPolyDatatoBinaryMaskImageFilterTest
  itkvtkPolyDatatoBinaryMaskImageFilterTest.cxx )

target_link_libraries( itkvtkPolyDatatoBinaryMaskImageFilterTest
  ${ITK_LIBRARIES}
  vtkGraphics
  vtkHybrid
)

add_test( itkvtkPolyDatatoBinaryMaskImageFilterTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/itkvtkPolyDatatoBinaryMaskImageFilterTest
  ${TESTING_DATA_PATH}/Circle3D.mhd
  temp.mhd
)

add_executable( itkBinaryMaskImageToGoFigureMeshAttributesTest
 itkBinaryMaskImageToGoFigureMeshAttributesTest.cxx )
target_link_libraries( itkBinaryMaskImageToGoFigureMeshAttributesTest
  QGoGui
  ${ITK_LIBRARIES}
)

add_test( itkBinaryMaskImageToGoFigureMeshAttributesTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/itkBinaryMaskImageToGoFigureMeshAttributesTest
  ${TESTING_DATA_PATH}/Circle3D.mhd
  temp.mhd
)

add_executable( itkvtkPolyDataToGoFigureMeshAttributesTest
 itkvtkPolyDataToGoFigureMeshAttributesTest.cxx )
target_link_libraries( itkvtkPolyDataToGoFigureMeshAttributesTest
  QGoGui
  vtkGraphics
  vtkHybrid
  ${ITK_LIBRARIES}
)

add_test( itkvtkPolyDataToGoFigureMeshAttributesTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/itkvtkPolyDataToGoFigureMeshAttributesTest
  ${TESTING_DATA_PATH}/Circle3D.mhd
)

set_tests_properties( itkBinaryMaskImageToGoFigureMeshAttributesTest
  PROPERTIES DEPENDS
  itkvtkPolyDatatoBinaryMaskImageFilterTest
)

add_executable( ContourToMeshFilterTest
  ContourToMeshFilterTest.cxx
  )
target_link_libraries( ContourToMeshFilterTest
  PoissonReconstruction
  vtkHybrid
  vtkRenderingAddOn2
  ${ITK_LIBRARIES}
  )

add_test( ContourToMeshFilterTest
 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ContourToMeshFilterTest
 1
 )

add_executable( MeshToContourFilterTest
  MeshToContourFilterTest.cxx
  ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/MeshToContourFilter.cxx
  )
target_link_libraries( MeshToContourFilterTest
  vtkHybrid
  )

add_test( MeshToContourFilterTest
 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MeshToContourFilterTest
 1
 )

#add_executable( singleCellSplitImageFilter
#  SingleCellSplitImageFilter.cxx
#  )
#target_link_libraries(
#  singleCellSplitImageFilter
#  ITKReview
#  ITKIO
#  )

add_executable( cellMergeImageFilter
  CellMergeImageFilter.cxx
  )

target_link_libraries( cellMergeImageFilter
  vtkHybrid
  )

#add_test( singleCellSplitImageFilterTest
#  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/singleCellSplitImageFilter
#  ${TESTING_DATA_PATH}/MultiphaseCells2D.png
#  split.mhd
#)


add_executable( itkvtkMeshSplitterDanielssonImageFilterTest
  itkvtkMeshSplitterDanielssonImageFilterTest.cxx
)
target_link_libraries( itkvtkMeshSplitterDanielssonImageFilterTest
  QGoGui
  ${ITK_LIBRARIES}
)

add_test( itkvtkMeshSplitterDanielssonImageFilterTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/itkvtkMeshSplitterDanielssonImageFilterTest
  ${TESTING_DATA_PATH}/Circle3D.mhd
)

add_executable( ExtractMeshesFromLabelImageFilter
  ExtractMeshesFromLabelImageFilter.cxx
)
target_link_libraries( ExtractMeshesFromLabelImageFilter
  #ITKCommon
  #ITKIO
  #ITKReview
  ${ITK_LIBRARIES}
)

add_executable(ConvertMeshesToLabelImageFilter ConvertMeshesToLabelImageFilter.cxx )
target_link_libraries( ConvertMeshesToLabelImageFilter
  # ITKIO ITKReview
  ${ITK_LIBRARIES}
)

add_executable( itkQuadEdgeMeshTovtkPolyDataTest itkQuadEdgeMeshTovtkPolyDataTest.cxx )
target_link_libraries( itkQuadEdgeMeshTovtkPolyDataTest
  #ITKCommon
  ${ITK_LIBRARIES}
  vtkHybrid
)

add_executable( itkvtkPolyDataToitkQuadEdgeMeshTest
  itkvtkPolyDataToitkQuadEdgeMeshTest.cxx )
target_link_libraries( itkvtkPolyDataToitkQuadEdgeMeshTest
  #ITKCommon
  ${ITK_LIBRARIES}
  vtkHybrid
)

add_test( itkvtkPolyDataToitkQuadEdgeMeshTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/itkvtkPolyDataToitkQuadEdgeMeshTest
)

add_executable( itkvtkMeshMergeConvexHullFilterTest
  itkvtkMeshMergeConvexHullFilterTest.cxx )
target_link_libraries( itkvtkMeshMergeConvexHullFilterTest
  QGoGui
  ${ITK_LIBRARIES}
)

add_test( itkvtkMeshMergeConvexHullFilterTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/itkvtkMeshMergeConvexHullFilterTest
  ${TESTING_DATA_PATH}/Circle3D.mhd
)
