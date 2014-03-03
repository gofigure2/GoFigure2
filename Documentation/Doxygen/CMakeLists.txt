# important note to link to external documentation
#  * qt.tags is located in ${QT_DOC_DIR}/html/qt.tags
#  * VTK Nightly doxygen tag file can be downloaded here:
#     http://www.vtk.org/files/Nightly/vtkNightlyDoc.tag.gz
#     http://www.vtk.org/files/Nightly/vtkNightlyDoc.tag
#  * ITK can be downloaded here:
#

set( VTK_DOX_TAG )
set( VTK_URL )
set( VTK_TAG_STRING )

set( ITK_DOC_TAG )
set( ITK_URL )
set( ITK_TAG_STRING )

set( QT_TAG_STRING )

if( LINK_EXTERNAL_DOC )
  option( DOXYGEN_NIGHTLY_LINK
    "Link doxygen to the nightly doxygen of VTK and ITK" ON )

  if( DOXYGEN_NIGHTLY_LINK )
    file( DOWNLOAD http://www.vtk.org/files/nightly/vtkNightlyDoc.tag
      ${GOFIGURE2_BINARY_DIR}/Documentation/vtkNightlyDoc.tag
      )

    set( VTK_DOX_TAG
      ${GOFIGURE2_BINARY_DIR}/Documentation/vtkNightlyDoc.tag
      CACHE FILEPATH "" FORCE
      )

    set( VTK_URL "http://www.vtk.org/doc/nightly/html" )
    set( VTK_TAG_STRING "${VTK_DOX_TAG}=${VTK_URL}" )

    file( DOWNLOAD
      http://public.kitware.com/pub/itk/NightlyDoxygen/InsightDoxygen.tag
      ${GOFIGURE2_BINARY_DIR}/Documentation/InsightDoxygen.tag
      )

    set( ITK_DOX_TAG
      ${GOFIGURE2_BINARY_DIR}/Documentation/InsightDoxygen.tag
      CACHE FILEPATH "" FORCE
      )

    set( ITK_URL "http://www.itk.org/Doxygen/html" )
    set( ITK_TAG_STRING "${ITK_DOX_TAG}=${ITK_URL}" )

  else( DOXYGEN_NIGHTLY_LINK )
    find_file( VTK_DOX_TAG
      NAMES vtkNightlyDoc.tag
      HINTS /usr/share
      DOC "Tag file for release of VTK"
      )

    set( VTK_URL
      "http://www.vtk.org/doc/release/${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}/html" )

    set( VTK_TAG_STRING "${VTK_DOX_TAG}=${VTK_URL}" )

    find_file( ITK_DOX_TAG
      NAMES InsightDoxygen.tag
      HINTS /usr/share
      DOC "Tag file for release of ITK"
      )

    set( ITK_URL
      "http://www.itk.org/Doxygen${ITK_VERSION_MAJOR}${ITK_VERSION_MINOR}/html" )

    set( ITK_TAG_STRING "${ITK_DOX_TAG}=${ITK_URL}" )

  endif( DOXYGEN_NIGHTLY_LINK )

  file( COPY ${QT_DOC_DIR}/html/qt.tags
    DESTINATION ${GOFIGURE2_BINARY_DIR}/Documentation/
  )

  set( QT_DOX_TAG ${GOFIGURE2_BINARY_DIR}/Documentation/qt.tags )

  set( QT_URL
      "http://doc.trolltech.com/4.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}"
     )
  set( QT_TAG_STRING "${QT_DOX_TAG}=${QT_URL}" )

endif( LINK_EXTERNAL_DOC )

CONFIGURE_FILE (
  ${GOFIGURE2_SOURCE_DIR}/Documentation/Doxygen/Doxyfile.txt.in
  ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
)

add_custom_target(doc ALL
  ${DOXYGEN_EXECUTABLE} ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
  DEPENDS ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
)

