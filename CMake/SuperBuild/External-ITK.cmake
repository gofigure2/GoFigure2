#---------------------------------------------------------------------------
# Get and build itk

set(proj ITK)

option( BUILD_ITK4 "Use ITKv4 (if not, it will use ITKv3)" ON )

if( ${BUILD_ITK4} )
  set( ITK_TAG "v3.20.1" )
else()
  set( ITK_TAG "v4.5.0" )
endif()

ExternalProject_Add(${proj}
  GIT_REPOSITORY "${git_protocol}://itk.org/ITK.git"
  GIT_TAG "${ITK_TAG}"
  UPDATE_COMMAND ""
  SOURCE_DIR ${proj}
  BINARY_DIR ${proj}-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DBUILD_SHARED_LIBS:BOOL=${SUPER_SHARED_LIBS}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/INSTALL
#    -DITK_INSTALL_LIB_DIR:PATH=${GOFIGURE2_INSTALL_LIB_DIR}
     # should change soon
    -DITK_USE_REVIEW:INTERNAL=ON
    -DITK_USE_REVIEW_STATISTICS:INTERNAL=ON
  DEPENDS
    ${ITK_DEPENDENCIES}
)

set(ITK_DIR ${CMAKE_BINARY_DIR}/INSTALL/lib/InsightToolkit )
#${CMAKE_BINARY_DIR}/${proj}-build)
