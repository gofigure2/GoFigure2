SET (CTEST_SITE "gelas.megason-lab.hms" )
SET (CTEST_BUILD_NAME "Linux-g++-4.3.2-x86_64")
SET (CTEST_NOTES_FILES "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}")

SET (CTEST_DASHBOARD_ROOT "$ENV{HOME}/SVNROOT/")
SET (CTEST_SOURCE_DIRECTORY "${CTEST_DASHBOARD_ROOT}/gofigure2/trunk/")
SET (CTEST_BINARY_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/BUILD/Debug/")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND
  "/usr/bin/ctest -D Nightly -A
${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME} -V"
  )

# what cmake command to use for configuring this dashboard
SET (CTEST_CMAKE_COMMAND
  "/usr/bin/cmake"
  )

# should ctest wipe the binary tree before running
SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)

# this is the initial cache to use for the binary tree, be careful to escape
# any quotes inside of this string if you use it
SET (CTEST_INITIAL_CACHE "
//Name of the computer/site where compile is being run
SITE:STRING=gelas.megason-lab.hms
//Name of the build
BUILDNAME:STRING=Linux-g++-4.3.2-x86_64
//Command used to build entire project from the command line.
MAKECOMMAND:STRING=/usr/bin/make -j 4
//Build Doxygen Documentation
BUILD_DOCUMENTATION:BOOL=ON
//Software packaging
BUILD_PACKAGE:BOOL=OFF
//Build the testing tree.
BUILD_TESTING:BOOL=ON
CMAKE_BUILD_TYPE:STRING=Debug
//CXX compiler.
CMAKE_CXX_COMPILER:FILEPATH=/usr/lib64/ccache/c++
CMAKE_C_COMPILER:FILEPATH=/usr/lib64/ccache/gcc
//Flags used by the compiler during debug builds.
CMAKE_C_FLAGS_DEBUG:STRING=-g
//Flags used by the compiler during release minsize builds.
CMAKE_C_FLAGS_MINSIZEREL:STRING=-Os -DNDEBUG
//Flags used by the compiler during release builds (/MD /Ob1 /Oi
// /Ot /Oy /Gs will produce slightly less optimized but smaller
// files).
CMAKE_C_FLAGS_RELEASE:STRING=-O3 -DNDEBUG
//Flags used by the compiler during Release with Debug Info builds.
CMAKE_C_FLAGS_RELWITHDEBINFO:STRING=-O2 -g
//Install path prefix, prepended onto install directories.
CMAKE_INSTALL_PREFIX:PATH=/usr/local
//Path to a program.
CMAKE_LINKER:FILEPATH=/usr/bin/ld
//Path to a program.
CMAKE_MAKE_PROGRAM:FILEPATH=/usr/bin/gmake
//Path to a program.
CMAKE_NM:FILEPATH=/usr/bin/nm
//Path to a program.
CMAKE_OBJCOPY:FILEPATH=/usr/bin/objcopy
//Path to a program.
CMAKE_OBJDUMP:FILEPATH=/usr/bin/objdump
//Path to a program.
CMAKE_RANLIB:FILEPATH=/usr/bin/ranlib
//If set, runtime paths are not added when using shared libraries.
CMAKE_SKIP_RPATH:BOOL=NO
//Path to a program.
CMAKE_STRIP:FILEPATH=/usr/bin/strip
//If true, cmake will use relative paths in makefiles and projects.
CMAKE_USE_RELATIVE_PATHS:BOOL=OFF
//If this value is on, makefiles will be generated without the
// .SILENT directive, and all commands will be echoed to the console
// during the make.  This is useful for debugging only. With Visual
// Studio IDE projects all commands are done without /nologo.
CMAKE_VERBOSE_MAKEFILE:BOOL=FALSE
//Path to the coverage program that CTest uses for performing coverage
// inspection
COVERAGE_COMMAND:FILEPATH=/usr/bin/gcov
//Path to a program.
CVSCOMMAND:FILEPATH=/usr/bin/cvs
//Options passed to the cvs update command.
CVS_UPDATE_OPTIONS:STRING=-d -A -P
//Maximum time allowed before CTest will kill the test.
DART_TESTING_TIMEOUT:STRING=1500
//Graphviz Dot tool for using Doxygen
DOXYGEN_DOT_EXECUTABLE:FILEPATH=/usr/bin/dot
//Missing description
DOXYGEN_DOT_PATH:FILEPATH=/usr/bin
//Doxygen documentation generation tool (http://www.doxygen.org)
DOXYGEN_EXECUTABLE:FILEPATH=/usr/bin/doxygen
//Single output directory for building all executables.
EXECUTABLE_OUTPUT_PATH:PATH=/home/ajg23/SVNROOT/gofigure2/trunk/BUILD/Debug/bin
//Value Computed by CMake
GOFIGURE2_BINARY_DIR:STATIC=/home/ajg23/SVNROOT/gofigure2/trunk/BUILD/Debug
//Value Computed by CMake
GOFIGURE2_SOURCE_DIR:STATIC=/home/ajg23/SVNROOT/gofigure2/trunk
//The directory containing ITKConfig.cmake.  This is either the
// root of the build tree, or PREFIX/lib/InsightToolkit for an
// installation.
ITK_DIR:PATH=/home/ajg23/CVSROOT/Insight/bin/Debug
//The directory containing VTKConfig.cmake.  This is either the
// root of the build tree, or PREFIX/lib/vtk for an installation.
//  For VTK 4.0, this is the location of UseVTK.cmake.  This is
// either the root of the build tree or PREFIX/include/vtk for
// an installation.
VTK_DIR:PATH=/home/ajg23/CVSROOT/VTK/bin/Debug
//Dependencies for the target
vtkRenderingAddOn2_LIB_DEPENDS:STATIC=general;vtkCommon;general;vtkRendering;general;vtkWidgets;general;vtkVolumeRendering;general;vtkHybrid;general;vtkImaging;
//Single output directory for building all libraries.
LIBRARY_OUTPUT_PATH:PATH=/home/ajg23/SVNROOT/gofigure2/trunk/BUILD/Debug/lib
//Command used to build entire project from the command line.
MAKECOMMAND:STRING=/usr/bin/gmake -i
//Path to the memory checking command, used for memory error detection.
MEMORYCHECK_COMMAND:FILEPATH=/usr/bin/valgrind
//Path to a file.
OPENSSL_INCLUDE_DIR:PATH=/usr/include
//Path to a library.
OPENSSL_LIBRARIES:FILEPATH=/usr/lib64/libssl.so
//Dependencies for the target
QMegaVTKAddOn2_LIB_DEPENDS:STATIC=general;vtkRenderingAddOn2;general;QVTK;optimized;/usr/lib64/libQt3Support.so;debug;/usr/lib64/libQt3Support_debug.so;optimized;/usr/lib64/libQtGui.so;debug;/usr/lib64/libQtGui_debug.so;general;/usr/lib64/libpng.so;general;/usr/lib64/libSM.so;general;/usr/lib64/libICE.so;general;/usr/lib64/libXi.so;general;/usr/lib64/libXrender.so;general;/usr/lib64/libXrandr.so;general;/usr/lib64/libXcursor.so;general;/usr/lib64/libXinerama.so;general;/usr/lib64/libXfixes.so;general;/usr/lib64/libfreetype.so;general;/usr/lib64/libfontconfig.so;general;/usr/lib64/libXext.so;general;/usr/lib64/libX11.so;general;/usr/lib64/libm.so;optimized;/usr/lib64/libQtXml.so;debug;/usr/lib64/libQtXml_debug.so;optimized;/usr/lib64/libQtSql.so;debug;/usr/lib64/libQtSql_debug.so;optimized;/usr/lib64/libQtNetwork.so;debug;/usr/lib64/libQtNetwork_debug.so;general;/usr/lib64/libssl.so;optimized;/usr/lib64/libQtCore.so;debug;/usr/lib64/libQtCore_debug.so;general;/usr/lib64/libz.so;general;/usr/lib64/libgthread-2.0.so;general;/usr/lib64/libglib-2.0.so;general;/usr/lib64/librt.so;general;-lpthread;general;-ldl;
//Path to a program.
QT_DBUSCPP2XML_EXECUTABLE:FILEPATH=/usr/lib64/qt4/bin/qdbuscpp2xml
//Path to a program.
QT_DBUSXML2CPP_EXECUTABLE:FILEPATH=/usr/lib64/qt4/bin/qdbusxml2cpp
//Path to a library.
QT_DBUS_LIBRARY:FILEPATH=/lib64/libdbus-1.so
//The location of the Qt docs
QT_DOC_DIR:PATH=/usr/share/doc/qt4
//Path to a library.
QT_FONTCONFIG_LIBRARY:FILEPATH=/usr/lib64/libfontconfig.so
//Path to a library.
QT_FREETYPE_LIBRARY:FILEPATH=/usr/lib64/libfreetype.so
//Path to a library.
QT_GLIB_LIBRARY:FILEPATH=/usr/lib64/libglib-2.0.so
//Path to a library.
QT_GTHREAD_LIBRARY:FILEPATH=/usr/lib64/libgthread-2.0.so
//Qt library dir
QT_LIBRARY_DIR:PATH=/usr/lib64
//Path to a program.
QT_LRELEASE_EXECUTABLE:FILEPATH=/usr/lib64/qt4/bin/lrelease
//Path to a program.
QT_LUPDATE_EXECUTABLE:FILEPATH=/usr/lib64/qt4/bin/lupdate
//The location of the Qt mkspecs containing qconfig.pri
QT_MKSPECS_DIR:PATH=/usr/lib64/qt4/mkspecs
//Missing description
QT_MOC_EXECUTABLE:FILEPATH=/usr/lib64/qt4/bin/moc
//Path to a file.
QT_PHONON_INCLUDE_DIR:PATH=/usr/include/phonon
//The location of the Qt plugins
QT_PLUGINS_DIR:PATH=/usr/lib64/qt4/plugins
//Path to a library.
QT_PNG_LIBRARY:FILEPATH=/usr/lib64/libpng.so
//Missing description
QT_QMAKE_EXECUTABLE:FILEPATH=/usr/bin/qmake-qt4
//Path to a file.
QT_QT3SUPPORT_INCLUDE_DIR:PATH=/usr/include/Qt3Support
//The Qt QT3SUPPORT library
QT_QT3SUPPORT_LIBRARY:FILEPATH=/usr/lib64/libQt3Support.so
//Path to a library.
QT_QT3SUPPORT_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQt3Support_debug.so
//Path to a library.
QT_QT3SUPPORT_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQt3Support.so
//Path to a file.
QT_QTASSISTANTCLIENT_INCLUDE_DIR:PATH=/usr/include/QtAssistant
//The Qt QTASSISTANTCLIENT library
QT_QTASSISTANTCLIENT_LIBRARY:FILEPATH=/usr/lib64/libQtAssistantClient.so
//Path to a library.
QT_QTASSISTANTCLIENT_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtAssistantClient_debug.so
//Path to a library.
QT_QTASSISTANTCLIENT_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtAssistantClient.so
//Path to a file.
QT_QTASSISTANT_INCLUDE_DIR:PATH=/usr/include/QtAssistant
//The Qt QTASSISTANT library
QT_QTASSISTANT_LIBRARY:FILEPATH=/usr/lib64/libQtAssistantClient.so
//Path to a library.
QT_QTASSISTANT_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtAssistantClient_debug.so
//Path to a library.
QT_QTASSISTANT_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtAssistantClient.so
//The Qt QTCLUCENE library
QT_QTCLUCENE_LIBRARY:FILEPATH=/usr/lib64/libQtCLucene.so
//Path to a library.
QT_QTCLUCENE_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtCLucene_debug.so
//Path to a library.
QT_QTCLUCENE_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtCLucene.so
//Path to a file.
QT_QTCORE_INCLUDE_DIR:PATH=/usr/include/QtCore
//The Qt QTCORE library
QT_QTCORE_LIBRARY:FILEPATH=/usr/lib64/libQtCore.so
//Path to a library.
QT_QTCORE_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtCore_debug.so
//Path to a library.
QT_QTCORE_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtCore.so
//Path to a file.
QT_QTDBUS_INCLUDE_DIR:PATH=/usr/include/QtDBus
//The Qt QTDBUS library
QT_QTDBUS_LIBRARY:FILEPATH=/usr/lib64/libQtDBus.so
//Path to a library.
QT_QTDBUS_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtDBus_debug.so
//Path to a library.
QT_QTDBUS_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtDBus.so
//Path to a file.
QT_QTDESIGNERCOMPONENTS_INCLUDE_DIR:PATH=/usr/include/QtDesigner
//The Qt QTDESIGNERCOMPONENTS library
QT_QTDESIGNERCOMPONENTS_LIBRARY:FILEPATH=/usr/lib64/libQtDesignerComponents.so
//Path to a library.
QT_QTDESIGNERCOMPONENTS_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtDesignerComponents_debug.so
//Path to a library.
QT_QTDESIGNERCOMPONENTS_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtDesignerComponents.so
//Path to a file.
QT_QTDESIGNER_INCLUDE_DIR:PATH=/usr/include/QtDesigner
//The Qt QTDESIGNER library
QT_QTDESIGNER_LIBRARY:FILEPATH=/usr/lib64/libQtDesigner.so
//Path to a library.
QT_QTDESIGNER_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtDesigner_debug.so
//Path to a library.
QT_QTDESIGNER_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtDesigner.so
//Path to a file.
QT_QTGUI_INCLUDE_DIR:PATH=/usr/include/QtGui
//The Qt QTGUI library
QT_QTGUI_LIBRARY:FILEPATH=/usr/lib64/libQtGui.so
//Path to a library.
QT_QTGUI_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtGui_debug.so
//Path to a library.
QT_QTGUI_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtGui.so
//Path to a file.
QT_QTHELP_INCLUDE_DIR:PATH=/usr/include/QtHelp
//The Qt QTHELP library
QT_QTHELP_LIBRARY:FILEPATH=/usr/lib64/libQtHelp.so
//Path to a library.
QT_QTHELP_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtHelp_debug.so
//Path to a library.
QT_QTHELP_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtHelp.so
//Path to a file.
QT_QTNETWORK_INCLUDE_DIR:PATH=/usr/include/QtNetwork
//The Qt QTNETWORK library
QT_QTNETWORK_LIBRARY:FILEPATH=/usr/lib64/libQtNetwork.so
//Path to a library.
QT_QTNETWORK_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtNetwork_debug.so
//Path to a library.
QT_QTNETWORK_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtNetwork.so
//Path to a file.
QT_QTOPENGL_INCLUDE_DIR:PATH=/usr/include/QtOpenGL
//The Qt QTOPENGL library
QT_QTOPENGL_LIBRARY:FILEPATH=/usr/lib64/libQtOpenGL.so
//Path to a library.
QT_QTOPENGL_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtOpenGL_debug.so
//Path to a library.
QT_QTOPENGL_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtOpenGL.so
//Path to a file.
QT_QTSCRIPT_INCLUDE_DIR:PATH=/usr/include/QtScript
//The Qt QTSCRIPT library
QT_QTSCRIPT_LIBRARY:FILEPATH=/usr/lib64/libQtScript.so
//Path to a library.
QT_QTSCRIPT_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtScript_debug.so
//Path to a library.
QT_QTSCRIPT_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtScript.so
//Path to a file.
QT_QTSQL_INCLUDE_DIR:PATH=/usr/include/QtSql
//The Qt QTSQL library
QT_QTSQL_LIBRARY:FILEPATH=/usr/lib64/libQtSql.so
//Path to a library.
QT_QTSQL_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtSql_debug.so
//Path to a library.
QT_QTSQL_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtSql.so
//Path to a file.
QT_QTSVG_INCLUDE_DIR:PATH=/usr/include/QtSvg
//The Qt QTSVG library
QT_QTSVG_LIBRARY:FILEPATH=/usr/lib64/libQtSvg.so
//Path to a library.
QT_QTSVG_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtSvg_debug.so
//Path to a library.
QT_QTSVG_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtSvg.so
//Path to a file.
QT_QTTEST_INCLUDE_DIR:PATH=/usr/include/QtTest
//The Qt QTTEST library
QT_QTTEST_LIBRARY:FILEPATH=/usr/lib64/libQtTest.so
//Path to a library.
QT_QTTEST_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtTest_debug.so
//Path to a library.
QT_QTTEST_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtTest.so
//Path to a file.
QT_QTUITOOLS_INCLUDE_DIR:PATH=/usr/include/QtUiTools
//The Qt QTUITOOLS library
QT_QTUITOOLS_LIBRARY:FILEPATH=/usr/lib64/libQtUiTools.a
//Path to a library.
QT_QTUITOOLS_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtUiTools_debug.a
//Path to a library.
QT_QTUITOOLS_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtUiTools.a
//Path to a file.
QT_QTWEBKIT_INCLUDE_DIR:PATH=/usr/include/QtWebKit
//The Qt QTWEBKIT library
QT_QTWEBKIT_LIBRARY:FILEPATH=/usr/lib64/libQtWebKit.so
//Path to a library.
QT_QTWEBKIT_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtWebKit_debug.so
//Path to a library.
QT_QTWEBKIT_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtWebKit.so
//Path to a file.
QT_QTXMLPATTERNS_INCLUDE_DIR:PATH=/usr/include/QtXmlPatterns
//The Qt QTXMLPATTERNS library
QT_QTXMLPATTERNS_LIBRARY:FILEPATH=/usr/lib64/libQtXmlPatterns.so
//Path to a library.
QT_QTXMLPATTERNS_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtXmlPatterns_debug.so
//Path to a library.
QT_QTXMLPATTERNS_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtXmlPatterns.so
//Path to a file.
QT_QTXML_INCLUDE_DIR:PATH=/usr/include/QtXml
//The Qt QTXML library
QT_QTXML_LIBRARY:FILEPATH=/usr/lib64/libQtXml.so
//Path to a library.
QT_QTXML_LIBRARY_DEBUG:FILEPATH=/usr/lib64/libQtXml_debug.so
//Path to a library.
QT_QTXML_LIBRARY_RELEASE:FILEPATH=/usr/lib64/libQtXml.so
//Path to a program.
QT_RCC_EXECUTABLE:FILEPATH=/usr/lib64/qt4/bin/rcc
//Path to a library.
QT_RT_LIBRARY:FILEPATH=/usr/lib64/librt.so
//The location of the Qt translations
QT_TRANSLATIONS_DIR:PATH=/usr/share/qt4/translations
//Path to a program.
QT_UIC3_EXECUTABLE:FILEPATH=/usr/lib64/qt4/bin/uic3
//Missing description
QT_UIC_EXECUTABLE:FILEPATH=/usr/lib64/qt4/bin/uic
//Path to a library.
QT_X11_ICE_LIBRARY:FILEPATH=/usr/lib64/libICE.so
//Path to a library.
QT_X11_SM_LIBRARY:FILEPATH=/usr/lib64/libSM.so
//Path to a library.
QT_X11_X11_LIBRARY:FILEPATH=/usr/lib64/libX11.so
//Path to a library.
QT_X11_Xext_LIBRARY:FILEPATH=/usr/lib64/libXext.so
//Path to a library.
QT_X11_m_LIBRARY:FILEPATH=/usr/lib64/libm.so
//Path to a library.
QT_XCURSOR_LIBRARY:FILEPATH=/usr/lib64/libXcursor.so
//Path to a library.
QT_XFIXES_LIBRARY:FILEPATH=/usr/lib64/libXfixes.so
//Path to a library.
QT_XINERAMA_LIBRARY:FILEPATH=/usr/lib64/libXinerama.so
//Path to a library.
QT_XI_LIBRARY:FILEPATH=/usr/lib64/libXi.so
//Path to a library.
QT_XRANDR_LIBRARY:FILEPATH=/usr/lib64/libXrandr.so
//Path to a library.
QT_XRENDER_LIBRARY:FILEPATH=/usr/lib64/libXrender.so
//Path to a library.
QT_ZLIB_LIBRARY:FILEPATH=/usr/lib64/libz.so
//Path to scp command, used by CTest for submitting results to
// a Dart server
SCPCOMMAND:FILEPATH=/usr/bin/scp
//Path to a program.
SVNCOMMAND:FILEPATH=/usr/bin/svn
")

# what svn command to use for configuring this dashboard
SET (CTEST_SVN_COMMAND "/usr/bin/svn")

# set any extra envionment varibles here
SET (CTEST_ENVIRONMENT
 "DISPLAY=:0"
 "CXX=/usr/lib64/ccache/g++"
 "CC=/usr/lib64/ccache/gcc"
)

#ctest_memcheck(BUILD “${CTEST_BINARY_DIRECTORY}”)
#
#set( CTEST_MEMORYCHECK_COMMAND “/usr/bin/valgrind” )
#set( CTEST_MEMORYCHECK_SUPPRESSIONS_FILE
#“${CTEST_SCRIPT_DIRECTORY}/CMake.supp”)
#set( CTEST_MEMORYCHECK_COMMAND_OPTIONS
#“--gen-suppressions=all --trace-children=yes
#-q --leak-check=yes --show-reachable=yes
#--workaround-gcc296-bugs=yes
#--num-callers=100 -v” )
