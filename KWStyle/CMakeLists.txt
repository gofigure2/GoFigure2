#-----------------------------------------------------------------------------
# uses KWStyle for checking the coding style
include(KWStyle.cmake)


if(UNIX)
  configure_file(${GOFIGURE2_SOURCE_DIR}/KWStyle/doAllStyle.csh.in
                 ${GOFIGURE2_BINARY_DIR}/doAllStyle.csh
                 @ONLY IMMEDIATE)
  exec_program(chmod ARGS 755 \"${GOFIGURE2_BINARY_DIR}/doAllStyle.csh\"
                     OUTPUT_VARIABLE ITK_EXEC_PLEASE_BE_QUIET)
endif(UNIX)

