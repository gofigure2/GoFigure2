#---------------------------------------------------------------------------
# Get and build itk

set(proj ITK)

set (ITK-IO  "-DITK-IO-Base:INTERNAL=ON"
             "-DITK-IO-JPEG:INTERNAL=ON"
             "-DITK-IO-BMP:INTERNAL=ON"
             "-DITK-IO-GDCM:INTERNAL=ON"
             "-DITK-IO-LSM:INTERNAL=ON"
             "-DITK-IO-PNG:INTERNAL=ON"
             "-DITK-IO-TIFF:INTERNAL=ON"
             "-DITK-IO-VTK:INTERNAL=ON"
             "-DITK-IO-Stimulate:INTERNAL=ON"
             "-DITK-IO-BioRad:INTERNAL=ON"
             "-DITK-IO-Meta:INTERNAL=ON"
             "-DITK-IO-NIFTI:INTERNAL=ON"
             "-DITK-IO-NRRD:INTERNAL=ON"
             "-DITK-IO-GIPL:INTERNAL=ON"
           )

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
    -DModule_ITK-Common:INTERNAL=ON
    -DModule_ITK-Statistics:INTERNAL=ON
     # should change soon
    -DITK_USE_REVIEW:INTERNAL=ON
    ${ITK-IO}
  INSTALL_COMMAND ""
  DEPENDS
    ${ITK_DEPENDENCIES}
)

set(ITK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
