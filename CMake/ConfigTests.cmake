#----------------------------------------------------------
INCLUDE ( CTest )

#----------------------------------------------------------
# Build name
set( BUILDNAME "${BUILDNAME}"
               CACHE STRING "Name of build on the dashboard"
              )
mark_as_advanced( BUILDNAME )

#----------------------------------------------------------
# Sikuli
option( TESTING_USING_SIKULI "Use Sikuli for testing" OFF )
mark_as_advanced( TESTING_USING_SIKULI )

if( TESTING_USING_SIKULI )
  find_package( Sikuli )
  add_subdirectory( Testing )
endif( TESTING_USING_SIKULI )

#----------------------------------------------------------  
configure_file( CMake/CTestCustom.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake
  @ONLY
          )
