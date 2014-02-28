if( WIN32 )
  if( NOT CYGWIN )
    if( NOT MINGW )
      if( BUILD_SHARED_LIBS )
        add_definitions( -DPoissonReconstruction_EXPORTS )
      endif( BUILD_SHARED_LIBS )
    endif( NOT MINGW )
  endif( NOT CYGWIN )
endif( WIN32 )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/PoissonReconstructionConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/PoissonReconstructionConfigure.h
  @ONLY IMMEDIATE
)

add_library( PoissonReconstruction
  vtkNormalEstimationFilter.cxx
  vtkPoissonReconstruction.cxx
  Factor.cpp
  Geometry.cpp
  MarchingCubes.cpp
  MultiGridOctest.cpp
  MultiGridOctreeData.cpp
  )
target_link_libraries( PoissonReconstruction vtkHybrid )

# add_test( PoissonReconstructionTest
#   ${PoissonReconstruction_BINARY_DIR}/PoissonReconstruction
#   ${PoissonReconstruction_SOURCE_DIR}/data/horsePoints.vtp
#   5
#   temp.vtp
# )
#
# include( CTest )

set_target_properties( PoissonReconstruction PROPERTIES VERSION ${GOFIGURE2_LIB_VERSION} SOVERSION ${GOFIGURE2_LIB_VERSION} )

# Runtime
install( TARGETS PoissonReconstruction
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} NAMELINK_SKIP COMPONENT Libraries
  )

# Development
install( TARGETS PoissonReconstruction
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries NAMELINK_ONLY
  )

file( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
file( GLOB __source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/*.inl" )

install( FILES ${__source_file_h} ${__source_file_txx}
  ${CMAKE_CURRENT_BINARY_DIR}/PoissonReconstructionConfigure.h
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)
