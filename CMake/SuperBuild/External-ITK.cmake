#---------------------------------------------------------------------------
# Get and build itk

set(proj ITK)

ExternalProject_Add(${proj}
  GIT_REPOSITORY "${git_protocol}://itk.org/ITK.git"
  GIT_TAG "origin/release"
  SOURCE_DIR ${proj}
  BINARY_DIR ${proj}-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DITK_INSTALL_LIB_DIR:PATH=${GOFIGURE2_INSTALL_LIB_DIR}
    -DITK_USE_REVIEW:BOOL=ON
    -DITK_USE_REVIEW_STATISTICS:BOOL=ON      
  INSTALL_COMMAND ""
  DEPENDS
    ${ITK_DEPENDENCIES}
)

set(ITK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
