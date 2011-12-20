#---------------------------------------------------------------------------
# Get and build itk

set(proj ITK)

#if( WIN32 )
  set( ITK_TAG "v3.20.1" )
#else()
#  set( ITK_TAG "4c21663d41017824a21f25cbacd9c3bdf00c72a0" )
#endif()

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
    -DITK_INSTALL_LIB_DIR:PATH=${GOFIGURE2_INSTALL_LIB_DIR}
     # should change soon
    -DITK_USE_REVIEW:INTERNAL=ON
    -DITK_USE_REVIEW_STATISTICS:INTERNAL=ON
  INSTALL_COMMAND ""
  DEPENDS
    ${ITK_DEPENDENCIES}
)

set(ITK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
