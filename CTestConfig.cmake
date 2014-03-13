## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   enable_testing()
##   include(CTest)
set(CTEST_PROJECT_NAME "GoFigure2")
set(CTEST_NIGHTLY_START_TIME "22:00:00 EST")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "www.gofigure2.org/CDash")
set(CTEST_DROP_LOCATION "/submit.php?project=GoFigure2")
set(CTEST_DROP_SITE_CDASH TRUE)
