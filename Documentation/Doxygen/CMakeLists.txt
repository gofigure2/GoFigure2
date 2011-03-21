# important note to link to external documentation
#  * qt.tags is located in ${QT_DOC_DIR}/html/qt.tags
#  * VTK Nightly doxygen tag file can be downloaded here:
#     http://www.vtk.org/files/Nightly/vtkNightlyDoc.tag.gz
#     http://www.vtk.org/files/Nightly/vtkNightlyDoc.tag
#  * ITK can be downloaded here:
#

SET( VTK_DOX_TAG )
SET( VTK_URL )

SET( ITK_DOC_TAG )
SET( ITK_URL )

IF( LINK_EXTERNAL_DOC )
  OPTION( DOXYGEN_NIGHTLY_LINK
    "Link doxygen to the nightly doxygen of VTK and ITK" OFF )

  IF( DOXYGEN_NIGHTLY_LINK )
    FILE( DOWNLOAD http://www.vtk.org/files/nightly/vtkNightlyDoc.tag
      ${GOFIGURE2_BINARY_DIR}/Documentation/vtkNightlyDoc.tag
      )

    SET( VTK_DOX_TAG
      ${GOFIGURE2_BINARY_DIR}/Documentation/vtkNightlyDoc.tag
      CACHE FILEPATH "" FORCE
      )

    SET( VTK_URL "http://www.vtk.org/doc/nightly/html" )

    FILE( DOWNLOAD
      http://public.kitware.com/pub/itk/NightlyDoxygen/InsightDoxygen.tag
      ${GOFIGURE2_BINARY_DIR}/Documentation/InsightDoxygen.tag
      )

    SET( ITK_DOX_TAG
      ${GOFIGURE2_BINARY_DIR}/Documentation/InsightDoxygen.tag
      CACHE FILEPATH "" FORCE
      )

    SET( ITK_URL "http://www.itk.org/Doxygen/html" )

  ELSE( DOXYGEN_NIGHTLY_LINK )
    FIND_FILE( VTK_DOX_TAG
      NAMES vtkNightlyDoc.tag
      HINTS /usr/share
      DOC "Tag file for release of VTK"
      )

    SET( VTK_URL
      "http://www.vtk.org/doc/release/${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}/html" )

    FIND_FILE( ITK_DOX_TAG
      NAMES InsightDoxygen.tag
      HINTS /usr/share
      DOC "Tag file for release of ITK"
      )

    SET( ITK_URL
      "http://www.itk.org/Doxygen${ITK_VERSION_MAJOR}${ITK_VERSION_MINOR}/html" )

  ENDIF( DOXYGEN_NIGHTLY_LINK )

  FILE( COPY ${QT_DOC_DIR}/html/qt.tags
    DESTINATION ${GOFIGURE2_BINARY_DIR}/Documentation/
  )

  SET( QT_DOX_TAG ${GOFIGURE2_BINARY_DIR}/Documentation/qt.tags )

  SET( QT_URL
      "http://doc.trolltech.com/4.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}"
     )
ENDIF( LINK_EXTERNAL_DOC )

CONFIGURE_FILE (
  ${GOFIGURE2_SOURCE_DIR}/Documentation/Doxygen/Doxyfile.txt.in
  ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
)

ADD_CUSTOM_TARGET(doc ALL
  ${DOXYGEN_EXECUTABLE} ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
  DEPENDS ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
)

