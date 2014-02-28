# - Find SIKULI

if( UNIX )
  find_program( SH_EXECUTABLE
    NAMES bash
    )

  find_file( SIKULI_EXECUTABLE
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

else( UNIX ) # Windows

  find_program( SIKULI_EXECUTABLE
    NAMES sikuli-ide.exe
    PATHS
     "$ENV{ProgramFiles}/Sikuli-IDE/"
     "$ENV{SystemDrive}/Sikuli-IDE/"
     PATH_SUFFIXES "bin"
    DOC "Specify the path to Sikuli"
  )

endif( UNIX )

include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sikuli DEFAULT_MSG Sikuli_EXECUTABLE)

mark_as_advanced( SIKULI_EXECUTABLE )

function( add_sikuli_test testname sikuli_test )

  set( SIKULI_RUNNING_DIR )

  # message( "argv2: " ${ARGV2} )
  set( image_lib_dir ${ARGV2} )

  if( image_lib_dir )
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/${image_lib_dir}/
          ${CMAKE_CURRENT_BINARY_DIR}/${sikuli_test}/
        COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/${sikuli_test}
          ${CMAKE_CURRENT_BINARY_DIR}/${sikuli_test}
      )
    set( SIKULI_RUNNING_DIR ${CMAKE_CURRENT_BINARY_DIR} )
  else( image_lib_dir )
    set( SIKULI_RUNNING_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
  endif( image_lib_dir )

  if( UNIX )
    add_test( ${testname}
      ${SH_EXECUTABLE} ${SIKULI_EXECUTABLE} -t ${SIKULI_RUNNING_DIR}/${sikuli_test} )
  else( UNIX )

    add_test( ${testname}
      ${SIKULI_EXECUTABLE} -t ${SIKULI_RUNNING_DIR}/${sikuli_test} )
  endif( UNIX )
endfunction( add_sikuli_test )
