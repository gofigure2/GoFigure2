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

  SET( SIKULI_RUNNING_DIR )

  MESSAGE( ${ARGV2} )
  SET( image_lib_dir ${ARGV2} )

  IF( image_lib_dir )
    EXECUTE_PROCESS(
        COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/${image_lib_dir}/
          ${CMAKE_CURRENT_BINARY_DIR}/${sikuli_test}/
        COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/${sikuli_test}
          ${CMAKE_CURRENT_BINARY_DIR}/${sikuli_test}
      )
    SET( SIKULI_RUNNING_DIR ${CMAKE_CURRENT_BINARY_DIR} )
  ELSE( image_lib_dir )
    SET( SIKULI_RUNNING_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
  ENDIF( image_lib_dir )

  IF( UNIX )
    add_test( ${testname}
      ${SH_EXECUTABLE} ${SIKULI_EXECUTABLE} -t ${SIKULI_RUNNING_DIR}/${sikuli_test} )
  ELSE( UNIX )

    add_test( ${testname}
      ${SIKULI_EXECUTABLE} -t ${SIKULI_RUNNING_DIR}/${sikuli_test} )
  ENDIF( UNIX )
ENDFUNCTION( add_sikuli_test )
