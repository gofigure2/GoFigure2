#-----------------------------------------------------------------------------
# uses KWStyle for checking the coding style
INCLUDE(KWStyle.cmake)


IF(UNIX)
  CONFIGURE_FILE(${GOFIGURE2_SOURCE_DIR}/KWStyle/doAllStyle.csh.in
                 ${GOFIGURE2_BINARY_DIR}/doAllStyle.csh
                 @ONLY IMMEDIATE)
  EXEC_PROGRAM(chmod ARGS 755 \"${GOFIGURE2_BINARY_DIR}/doAllStyle.csh\"
                     OUTPUT_VARIABLE ITK_EXEC_PLEASE_BE_QUIET)
ENDIF(UNIX)

