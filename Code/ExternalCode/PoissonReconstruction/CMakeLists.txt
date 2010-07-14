ADD_LIBRARY( PoissonReconstruction
  vtkNormalEstimationFilter.cxx
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

SET_TARGET_PROPERTIES( PoissonReconstruction PROPERTIES VERSION ${GOFIGURE2_LIB_VERSION} SOVERSION ${GOFIGURE2_LIB_VERSION} )

# Runtime
INSTALL( TARGETS PoissonReconstruction
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Applications
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} NAMELINK_SKIP COMPONENT Libraries
  )
  
# Development
INSTALL( TARGETS PoissonReconstruction
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Applications
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries NAMELINK_ONLY
  )
  
  
