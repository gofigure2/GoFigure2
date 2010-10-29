# bonito.megason-lab.hms
# Maintainer: Arnaud Gelas

set( CTEST_SITE "bonito.megason-lab.hms" )
set( CTEST_BUILD_NAME "FC12-g++-x86_64-static-dbg-develop" )
set( CTEST_BUILD_CONFIGURATION "Debug" )
set( CTEST_CMAKE_GENERATOR "Unix Makefiles" )

set( CTEST_BUILD_FLAGS -j2 )
set( CTEST_TEST_ARGS PARALLEL_LEVEL 2 )

#--------------------------------------------
# Note about branches
# By default nightly builds use master
# and continuous ones use develop
#--------------------------------------------

# To specify the branch you want to test
# set dashboard_git_branch
set( dashboard_git_branch develop )

set( dashboard_model Continuous )

set( CTEST_DASHBOARD_ROOT "${CTEST_SCRIPT_DIRECTORY}/${dashboard_git_branch}" )
set( CTEST_SOURCE_DIRECTORY "${CTEST_DASHBOARD_ROOT}/GoFigure2" )
set( CTEST_BINARY_DIRECTORY "${CTEST_DASHBOARD_ROOT}/GoFigure2_Build" )

set( dashboard_cache "
  VTK_DIR:PATH=/home/dv12/Dashboard/VTK/Nightly/Static/Debug/
  ITK_DIR:PATH=/home/dv12/Dashboard/ITK/ITK_Static_Debug/ITK-build/
  BUILD_EXAMPLES:BOOL=ON
  BUILD_TESTING:BOOL=ON
  QT_QMAKE_EXECUTABLE:FILEPATH=/usr/bin/qmake-qt4
  "
  )


include(${CTEST_SCRIPT_DIRECTORY}/gofigure2_common.cmake)
