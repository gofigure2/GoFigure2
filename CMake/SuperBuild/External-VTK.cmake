#-----------------------------------------------------------------------------
# Get and build VTK
#
set(proj VTK)

set(VTK_QT_ARGS)

if(NOT APPLE)
    set(VTK_QT_ARGS
      -DDESIRED_QT_VERSION:STRING=4
      -DVTK_USE_GUISUPPORT:BOOL=ON
      -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
      -DVTK_USE_QT:BOOL=ON
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      -DVTK_USE_MYSQL:BOOL=ON
      )
  if( UNIX )
    set(VTK_QT_ARGS
        ${VTK_QT_ARGS}
        -DVTK_USE_FFMPEG:BOOL=ON
       )
  endif()
else()
    set(VTK_QT_ARGS
      -DVTK_USE_CARBON:BOOL=OFF
      -DVTK_USE_COCOA:BOOL=ON # Default to Cocoa, VTK/CMakeLists.txt will enable Carbon and disable cocoa if needed
      -DVTK_USE_X:BOOL=OFF
      -DVTK_USE_RPATH:BOOL=ON
      -DDESIRED_QT_VERSION:STRING=4
      -DVTK_USE_GUISUPPORT:BOOL=ON
      -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
      -DVTK_USE_QT:BOOL=ON
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      -DVTK_USE_MYSQL:BOOL=ON
      )
endif()

ExternalProject_Add(${proj}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
  BINARY_DIR ${proj}-build
  GIT_REPOSITORY "${git_protocol}://vtk.org/VTK.git"
  GIT_TAG "origin/release"
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_SHARED_LIBS:BOOL=ON
    -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
    -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
    -DVTK_USE_PARALLEL:BOOL=ON
    -DVTK_DEBUG_LEAKS:BOOL=${USE_VTK_DEBUG_LEAKS}
    -DVTK_USE_RPATH:BOOL=ON
    -DVTK_INSTALL_LIB_DIR:PATH=${GoFigure2_INSTALL_LIB_DIR}
    ${VTK_QT_ARGS}
    ${VTK_MAC_ARGS}
  INSTALL_COMMAND ""
  DEPENDS
    ${VTK_DEPENDENCIES}
)

set(VTK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
