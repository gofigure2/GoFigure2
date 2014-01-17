#-----------------------------------------------------------------------------
# Get and build VTK
#
#-----------------------------------------------------------------------------
set(proj VTK)
set(proj_DEPENDENCIES)

if( SUPER_BOOST )
  set( proj_DEPENDENCIES Boost )
endif( SUPER_BOOST )

ExternalProject_Add(${proj}
  DEPENDS
    ${proj_DEPENDENCIES}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
  BINARY_DIR ${proj}-build
  GIT_REPOSITORY "${git_protocol}://vtk.org/VTK.git"
  GIT_TAG v5.10.1
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DBUILD_SHARED_LIBS:BOOL=${SUPER_SHARED_LIBS}
    -DVTK_DEBUG_LEAKS:BOOL=${USE_VTK_DEBUG_LEAKS}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/INSTALL
#    -DVTK_INSTALL_LIB_DIR:PATH=${GoFigure2_INSTALL_LIB_DIR}
    -DVTK_USE_QT:BOOL=ON
    -DQT_QMAKE_EXECUTABLE:PATH=${QT_QMAKE_EXECUTABLE}
    -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
    -DVTK_USE_MYSQL:BOOL=ON
    -DMYSQL_INCLUDE_DIRECTORIES:PATH=${MYSQL_INCLUDE_DIR}
    -DMYSQL_LIBRARY:FILEPATH=${MYSQL_LIBRARIES}
    -DVTK_USE_BOOST:BOOL=ON
    ${VIDEO_SUPPORT}
    ${Boost_SUPPORT}
    #${WINDOWS_FLAGS}
  INSTALL_COMMAND ""
)

set(VTK_DIR ${CMAKE_BINARY_DIR}/INSTALL/VTK)
