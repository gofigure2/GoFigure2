# - Find SIKULI

IF( UNIX )
  FIND_PROGRAM( SH_EXECUTABLE
    NAMES bash
    )

  FIND_FILE( SIKULI_EXECUTABLE
    NAMES sikuli-ide.sh
    PATHS
    "$ENV{ProgramFiles}/Sikuli-IDE/"
    "$ENV{SystemDrive}/Sikuli-IDE/"
    "$ENV{HOME}/Sikuli-IDE/"
    "$ENV{HOME}/Sikuli-IDE/Sikuli-IDE"
    "/Applications/Sikuli-IDE.app"
    PATH_SUFFIXES "bin"
    DOC "Specify the path to sikuli"
    )

ELSE( UNIX ) # Windows

  FIND_PROGRAM( SIKULI_EXECUTABLE
    NAMES sikuli-ide.exe
    PATHS
     "$ENV{ProgramFiles}/Sikuli-IDE/"
     "$ENV{SystemDrive}/Sikuli-IDE/"
     PATH_SUFFIXES "bin"
    DOC "Specify the path to Sikuli"
  )

ENDIF( UNIX )

INCLUDE( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sikuli DEFAULT_MSG Sikuli_EXECUTABLE)

MARK_AS_ADVANCED( SIKULI_EXECUTABLE )

FUNCTION( add_sikuli_test testname sikuli_test )
  IF( UNIX )
    add_test( ${testname}
      ${SH_EXECUTABLE} ${SIKULI_EXECUTABLE} -t ${sikuli_test} )
  ELSE( UNIX )
    add_test( ${testname}
      ${SIKULI_EXECUTABLE} -t ${sikuli_test} )
  ENDIF( UNIX )
ENDFUNCTION( add_sikuli_test )
