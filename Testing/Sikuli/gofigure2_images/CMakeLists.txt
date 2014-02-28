set( SIKULI_SCRIPTS_DIR
     ${GOFIGURE2_SOURCE_DIR}/Testing/Sikuli/gofigure2_images )

set( SIKULI_SCRIPTS_BINARY_DIR
    ${GOFIGURE2_BINARY_DIR}/Testing/Sikuli/gofigure2_images )

configure_file( ${SIKULI_SCRIPTS_DIR}/sikuli_test_all.sh.in
        ${GOFIGURE2_BINARY_DIR}/sikuli_test_all.sh
        @ONLY
        )

if( UNIX AND NOT APPLE )

  add_test( SikuliTest1
    ${SH_EXECUTABLE} ${GOFIGURE2_BINARY_DIR}/sikuli_test_all.sh
  )

endif( UNIX AND NOT APPLE )

