set( VTK_DOX_TAG )
set( VTK_URL )
set( VTK_TAG_STRING )

set( ITK_DOC_TAG )
set( ITK_URL )
set( ITK_TAG_STRING )

set( QT_TAG_STRING )

configure_file(
  ${GOFIGURE2_SOURCE_DIR}/Documentation/Doxygen/Doxyfile.txt.in
  ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
)

add_custom_target(doc ALL
  ${DOXYGEN_EXECUTABLE} ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
  DEPENDS ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
)

