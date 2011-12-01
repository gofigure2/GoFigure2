#-----------------------------------------------------------------------------
# Get and build VTK
#
#-----------------------------------------------------------------------------
set(proj VTK)

ExternalProject_Add(${proj}
  DEPENDS ${VTK_DEPENDENCIES}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
  BINARY_DIR ${proj}-build
  GIT_REPOSITORY "${git_protocol}://vtk.org/VTK.git"
  GIT_TAG "origin/release"
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DBUILD_SHARED_LIBS:BOOL=ON
    -DVTK_DEBUG_LEAKS:BOOL=${USE_VTK_DEBUG_LEAKS}
    -DVTK_INSTALL_LIB_DIR:PATH=${GoFigure2_INSTALL_LIB_DIR}
    -DVTK_USE_QT:BOOL=ON
    -DQT_QMAKE_EXECUTABLE:PATH=${QT_QMAKE_EXECUTABLE}
    -DVTK_USE_MYSQL:BOOL=ON
    -DMYSQL_INCLUDE_DIRECTORIES:PATH=${MYSQL_INCLUDE_DIR}
    -DMYSQL_LIBRARY:FILEPATH=${MYSQL_LIBRARIES}
    -DVTK_USE_BOOST:BOOL=ON
    ${VIDEO_SUPPORT}
    #${WINDOWS_FLAGS}
  INSTALL_COMMAND ""
)

set(VTK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
