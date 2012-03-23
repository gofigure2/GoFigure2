# ------------------------------------------------------------------------- 
# This CMake code installs the needed support libraries on NON OSX platforms
# ------------------------------------------------------------------------- 

include( InstallRequiredSystemLibraries )

# ------------------------------------------------------------------------- 
# Find/Install the needed Qt4 libraries. 
set( QTLIBLIST QtCore QtGui QtNetwork )
if( NOT Q_WS_MAC )
  foreach( qtlib ${QTLIBLIST} )
    if( WIN32 )
      get_filename_component( QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH )
      install( FILES ${QT_DLL_PATH_tmp}/${qtlib}d4.dll 
          DESTINATION bin 
          CONFIGURATIONS Debug 
          COMPONENT Runtime )
      install( FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll 
          DESTINATION bin 
          CONFIGURATIONS Release 
          COMPONENT Runtime )
    endif( WIN32 )
  endforeach( qtlib )
endif( NOT Q_WS_MAC )

