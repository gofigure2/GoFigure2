#---------------------------------------------------------------------------
# Get and build itk

set(proj ITK)

ExternalProject_Add(${proj}
  GIT_REPOSITORY "${git_protocol}://itk.org/ITK.git"
  GIT_TAG "v4.0a07"
  UPDATE_COMMAND ""
  SOURCE_DIR ${proj}
  BINARY_DIR ${proj}-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DITK_INSTALL_LIB_DIR:PATH=${GOFIGURE2_INSTALL_LIB_DIR}
    #to be modified
    -DITK_BUILD_ALL_MODULES:BOOL=ON
    -DModule_ITK-Review:INTERNAL=ON
  INSTALL_COMMAND ""
  DEPENDS
    ${ITK_DEPENDENCIES}
)

set(ITK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
