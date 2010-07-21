IF( WIN32 )
  IF( NOT CYGWIN )
    IF( NOT MINGW )
      IF( BUILD_SHARED_LIBS )
        ADD_DEFINITIONS( -DPoissonReconstruction_EXPORTS )
      ENDIF( BUILD_SHARED_LIBS )
    ENDIF( NOT MINGW )
  ENDIF( NOT CYGWIN )
ENDIF( WIN32 )

CONFIGURE_FILE( ${GOFIGURE2_SOURCE_DIR}/Code/ExternalCode/PoissonReconstruction/PoissonReconstructionConfigure.h.in
  ${GOFIGURE2_BINARY_DIR}/Code/ExternalCode/PoissonReconstruction/PoissonReconstructionConfigure.h @ONLY IMMEDIATE )
  
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

SET_TARGET_PROPERTIES( PoissonReconstruction PROPERTIES VERSION ${GOFIGURE2_LIB_VERSION} SOVERSION ${GOFIGURE2_LIB_VERSION} )

# Runtime
INSTALL( TARGETS PoissonReconstruction
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} NAMELINK_SKIP COMPONENT Libraries
  )
  
# Development
INSTALL( TARGETS PoissonReconstruction
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries NAMELINK_ONLY
  )
  
  
