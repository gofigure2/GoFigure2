#----------------------------------------------------------
INCLUDE ( CTest )

#----------------------------------------------------------
# Build name
SET( BUILDNAME "${BUILDNAME}"
               CACHE STRING "Name of build on the dashboard"
              )
MARK_AS_ADVANCED( BUILDNAME )

#----------------------------------------------------------
# Sikuli
OPTION( TESTING_USING_SIKULI "Use Sikuli for testing" OFF )
MARK_AS_ADVANCED( TESTING_USING_SIKULI )

IF( TESTING_USING_SIKULI )
  FIND_PACKAGE( Sikuli )
  ADD_SUBDIRECTORY( Testing )
ENDIF( TESTING_USING_SIKULI )

#----------------------------------------------------------  
CONFIGURE_FILE( CMake/CTestCustom.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake
  @ONLY
          )
