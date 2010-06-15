ADD_LIBRARY( PoissonReconstruction
  vtkPoissonReconstruction.cxx
  Factor.cpp
  Geometry.cpp
  MarchingCubes.cpp
  MultiGridOctest.cpp
  MultiGridOctreeData.cpp
  )
TARGET_LINK_LIBRARIES( PoissonReconstruction vtkHybrid )

# ADD_TEST( PoissonReconstructionTest 
#   ${PoissonReconstruction_BINARY_DIR}/PoissonReconstruction 
#   ${PoissonReconstruction_SOURCE_DIR}/data/horsePoints.vtp
#   5 
#   temp.vtp 
# )
# 
# INCLUDE( CTest )
