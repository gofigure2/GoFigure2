SET (CTEST_SITE "sanchome.megason-lab.hms")
SET (CTEST_BUILD_NAME "Linux-g++-4.3.2-x86_64")
SET (CTEST_NOTES_FILES "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}")
SET (CTEST_DASHBOARD_ROOT "$ENV{HOME}/SVNROOT")
SET (CTEST_SOURCE_DIRECTORY "${CTEST_DASHBOARD_ROOT}/gofigure2/trunk")
SET (CTEST_BINARY_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/BUILD/continuous/")
SET (CTEST_UPDATE_COMMAND "svn")
SET (CTEST_CMAKE_GENERATOR "Unix Makefiles")
SET (CTEST_PROJECT_NAME "GoFigure")

WHILE (${CTEST_ELAPSED_TIME} LESS 36000)
  SET (START_TIME ${CTEST_ELAPSED_TIME})
  CTEST_START (Continuous)
  CTEST_UPDATE (SOURCE "${CTEST_SOURCE_DIRECTORY}" RETURN_VALUE res)

  # force a build if this is the first run
  # and the build dir is empty
  IF ( NOT EXISTS "${CTEST_BINARY_DIRECTORY}/CMakeCache.txt" )
    SET ( res 1 )
    MESSAGE ( "First time build" )

    # this is the initial cache to use for the binary tree
    # be careful to sape quotes inside of this string if you use it.
    FILE ( WRITE "${CTEST_BINARY_DIRECTORY}/CMakeCache.txt" 
      "CMAKE_RUN_LONG_TESTS:BOOL=TRUE" )
  ENDIF ( NOT EXISTS "${CTEST_BINARY_DIRECTORY}/CMakeCache.txt" )

  IF ( ${res} GREATER 0 )
    MESSAGE ( "${res} Found changes run build" )
    CTEST_CONFIGURE ( BUILD "${CTEST_BINARY_DIRECTORY}" )
    CTEST_READ_CUSTOM_FILES ( "${CTEST_BINARY_DIRECTORY}" )
    CTEST_BUILD ( BUILD "${CTEST_BINARY_DIRECTORY}" )
    CTEST_TEST ( BUILD "${CTEST_BINARY_DIRECTORY}" )
    CTEST_SUBMIT ()
  ENDIF ( ${res} GREATER 0 )

  # loop no faster than once every 5 minutes
  MESSAGE ( "wait for 5 minutes" )
  CTEST_SLEEP ( ${START_TIME} 300 ${CTEST_ELAPSED_TIME} )

ENDWHILE (${CTEST_ELAPSED_TIME} LESS 36000)
