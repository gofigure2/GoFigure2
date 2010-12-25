SET( SIKULI_SCRIPTS_DIR
     ${GOFIGURE2_SOURCE_DIR}/Testing/Sikuli/gofigure2_images )

SET( SIKULI_SCRIPTS_BINARY_DIR
    ${GOFIGURE2_BINARY_DIR}/Testing/Sikuli/gofigure2_images )

CONFIGURE_FILE( ${SIKULI_SCRIPTS_DIR}/sikuli_test_all.sh.in
        ${GOFIGURE2_BINARY_DIR}/sikuli_test_all.sh
        @ONLY
        )


ADD_TEST( SikuliTest1
  "/bin/bash" "${GOFIGURE2_BINARY_DIR}/sikuli_test_all.sh"
)
