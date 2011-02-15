# - Find SIKULI

IF( UNIX )
  IF( NOT APPLE ) # Linux
    FIND_PROGRAM( SH_EXECUTABLE
      NAMES bash
    )

    FIND_PROGRAM( SIKULI_EXECUTABLE
      NAMES sikuli-ide.sh
      PATHS
      "$ENV{ProgramFiles}/Sikuli-IDE/"
      "$ENV{SystemDrive}/Sikuli-IDE/"
      "$ENV{HOME}/Sikuli-IDE/"
      "$ENV{HOME}/Sikuli-IDE/Sikuli-IDE"
      PATH_SUFFIXES "bin"
      DOC "Specify the path to sikuli"
    )
  ELSE( NOT APPLE ) # Mac OS
    MESSAGE( "Apple" )
  ENDIF( NOT APPLE )

ELSE( UNIX ) # Windows

  FIND_PROGRAM( SIKULI_EXECUTABLE
    NAMES sikuli
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
