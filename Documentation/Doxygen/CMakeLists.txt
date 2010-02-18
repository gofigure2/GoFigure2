CONFIGURE_FILE ( 
  ${GOFIGURE2_SOURCE_DIR}/Documentation/Doxygen/Doxyfile.txt.in 
  ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt 
) 

ADD_CUSTOM_TARGET(doc ALL 
  ${DOXYGEN_EXECUTABLE} ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt 
  DEPENDS ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt 
) 

