# - Find SIKULI 

FIND_PROGRAM( SIKULI_EXECUTABLE 
  NAMES sikuli-ide.sh
  PATHS
  "$ENV{ProgramFiles}/Sikuli-IDE/"
  "$ENV{SystemDrive}/Sikuli-IDE/"
  PATH_SUFFIXES "bin"
  DOC "Specify the path to sikuli"
)
INCLUDE( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sikuli DEFAULT_MSG Sikuli_EXECUTABLE)

MARK_AS_ADVANCED( SIKULI_EXECUTABLE )
SET( SIKULI_COMMAND "${SIKULI_EXECUTABLE} -t " )

