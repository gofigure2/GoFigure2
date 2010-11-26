# important note to link to external documentation
#  * qt.tags is located in ${QT_DOC_DIR}/html/qt.tags
#  * VTK Nightly doxygen tag file can be downloaded here:
#     http://www.vtk.org/files/Nightly/vtkNightlyDoc.tag.gz
#     http://www.vtk.org/files/Nightly/vtkNightlyDoc.tag
#  * ITK can be downloaded here:
#


CONFIGURE_FILE (
  ${GOFIGURE2_SOURCE_DIR}/Documentation/Doxygen/Doxyfile.txt.in
  ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
)

ADD_CUSTOM_TARGET(doc ALL
  ${DOXYGEN_EXECUTABLE} ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
  DEPENDS ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
)

