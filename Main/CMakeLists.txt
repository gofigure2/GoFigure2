SET( QGoMainWindow_UI
  go.ui
)

SET( QGoGUISRC_HDRS
  QGoTabManager.h
  QGoMainWindow.h
  QGoPluginManager.h
  ${QGoGUIINTERFACES_HDRS}
)


SET( GOFIGURE_APPLICATION_SOURCE_LIST
  gofigure.cxx
  QGoTabManager.cxx
  QGoMainWindow.cxx
  QGoPluginManager.cxx
  )

INCLUDE_DIRECTORIES( ${GOFIGURE2_BINARY_DIR}/Main/  )

QT4_WRAP_UI( QGoMainWindow_UI_H ${QGoMainWindow_UI} )

QT4_WRAP_CPP( QGoGUISRC_MOC ${QGoGUISRC_HDRS} )

SET_SOURCE_FILES_PROPERTIES( 
  ${GOFIGURE_APPLICATION_SOURCE_LIST} 
  PROPERTIES
  OBJECT_DEPENDS "${QGoMainWindow_UI_H}"
  )

SET_SOURCE_FILES_PROPERTIES( 
  ${QGoGUISRC_MOC} 
  PROPERTIES
  OBJECT_DEPENDS "${QGoMainWindow_UI_H}"
  )

QT4_ADD_RESOURCES( QGoMainWindow_QRC
  ${QGoResourceFile}
  )

SET( QGoGUISRC_SRC
  ${QGoGUISRC_CXX}
  ${QGoGUISRC_MOC}
)

set( qt_menu_nib_sources )

IF( APPLE )

  IF(QT_MAC_USE_COCOA)
    GET_FILENAME_COMPONENT(qt_menu_nib
     "@QT_QTGUI_LIBRARY_RELEASE@/Resources/qt_menu.nib"
      REALPATH)
    set( qt_menu_nib_sources
      "${qt_menu_nib}/classes.nib"
      "${qt_menu_nib}/info.nib"
      "${qt_menu_nib}/keyedobjects.nib"
      )
    SET_SOURCE_FILES_PROPERTIES(
      ${qt_menu_nib_sources}
      PROPERTIES
      MACOSX_PACKAGE_LOCATION Resources/qt_menu.nib
      )
  # ELSE(QT_MAC_USE_COCOA)
  ENDIF(QT_MAC_USE_COCOA)

  SET( MACOSX_BUNDLE_ICON_FILE
    ${GOFIGURE2_SOURCE_DIR}/Resources/myapp.icns
    )

  SET_SOURCE_FILES_PROPERTIES( 
    ${MACOSX_BUNDLE_ICON_FILE} 
    PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
    )
  
  SET( GOFIGURE_APPLICATION_SOURCE_LIST
    ${GOFIGURE_APPLICATION_SOURCE_LIST}
    ${MACOSX_BUNDLE_ICON_FILE}
    )

  SET( MACOSX_BUNDLE_SHORT_VERSION_STRING
    ${GOFIGURE2_VERSION}
    )

  SET( MACOSX_BUNDLE_VERSION
    ${GOFIGURE2_VERSION}
    )

  SET( MACOSX_BUNDLE_LONG_VERSION_STRING
    Version ${GOFIGURE2_VERSION}
    )

ENDIF( APPLE )

IF( WIN32 )
  IF( MINGW )
    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/myapp.o
      COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
        -i ${GOFIGURE2_SOURCE_DIR}/Resources/myapp.rc
        -o ${CMAKE_CURRENT_BINARY_DIR}/myapp.o
        )

    SET( ${GOFIGURE_APPLICATION_SOURCE_LIST}
      ${GOFIGURE_APPLICATION_SOURCE_LIST}
      ${CMAKE_CURRENT_BINARY_DIR}/myapp.o
      )
  ELSE( MINGW )
    SET( GOFIGURE_APPLICATION_SOURCE_LIST
      ${GOFIGURE_APPLICATION_SOURCE_LIST} 
      ${GOFIGURE2_SOURCE_DIR}/Resources/myapp.rc 
      )
  ENDIF( MINGW )
ENDIF( WIN32 )

#---------------------------------------------------------------------------
# This sets the windows build that will need the special winmain@16 call. 
# Qt provides this for us in the qtmain.lib file. Using this cmake code 
# will ensure we have it linked into our build. Not needed on 
# Unix/OS X/Linux which is why we have the IF(WIN32) conditional.
IF( WIN32 )
  SET( QT_USE_QTMAIN TRUE )
ENDIF( WIN32 )

ADD_EXECUTABLE( gofigure
  MACOSX_BUNDLE 
  ${GOFIGURE_APPLICATION_SOURCE_LIST}
  ${QGoGUIINTERFACES_SRC}
  ${QGoGUISRC_MOC}
  ${QGoMainWindow_QRC}
  ${qt_menu_nib_sources} 
)

ADD_DEPENDENCIES( gofigure 
  QGoGUI
  QGoIO
  itkQt
)

TARGET_LINK_LIBRARIES( gofigure
  QGoGui
  QGoIO
  itkQt
 )

#---------------------------------------------------------------------------
# Now the installation stuff below
#---------------------------------------------------------------------------
SET( plugin_dest_dir bin )
SET( qtconf_dest_dir bin )
SET( APPS "\${CMAKE_INSTALL_PREFIX}/bin/gofigure" )

IF(APPLE)
  SET( plugin_dest_dir gofigure.app/Contents/MacOS )
  SET( qtconf_dest_dir gofigure.app/Contents/Resources )
  SET( APPS "\${CMAKE_INSTALL_PREFIX}/gofigure.app" )
ENDIF(APPLE)

IF( WIN32 )
  SET( APPS "\${CMAKE_INSTALL_PREFIX}/bin/gofigure.exe" )
ENDIF(WIN32)

IF( UNIX AND NOT APPLE )
  SET( APPS "\${CMAKE_INSTALL_PREFIX}/bin/gofigure" )
ENDIF( UNIX AND NOT APPLE )

#---------------------------------------------------------------------------
# Install gofigure, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.

INSTALL( TARGETS gofigure
  ARCHIVE DESTINATION lib COMPONENT libraries
  LIBRARY DESTINATION lib COMPONENT libraries
  RUNTIME DESTINATION bin COMPONENT Runtime
  BUNDLE  DESTINATION .   COMPONENT Runtime
  )

#-----------------------------------------------------
# Here Let's take care of License files
# copying them into the build's folder
# and during the installation process
FILE( GLOB GOFIGURE2_LICENSE_FILES "Licenses/*.txt" )
# MESSAGE( SEND_ERROR "${GOFIGURE2_LICENSE_FILES}" )
ADD_CUSTOM_COMMAND( TARGET gofigure PRE_BUILD
  COMMENT "Copying License files..."
  COMMAND cp ARGS -R
    ${GOFIGURE2_SOURCE_DIR}/Licenses/
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
INSTALL( FILES ${GOFIGURE2_LICENSE_FILES} 
  DESTINATION bin/Licenses )
#-----------------------------------------------------

#---------------------------------------------------------------------------
# install a qt.conf file
# this inserts some cmake code into the install script to write the file
INSTALL( CODE "
  file( WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
  " COMPONENT Runtime 
  )

#---------------------------------------------------------------------------
# Use BundleUtilities to get all other dependencies for the application to 
# work. It takes a bundle or executable along with possible plugins and 
# inspects it for dependencies.  If they are not system dependencies, 
# they are copied.

# directories to look for dependencies
SET( DIRS 
  ${QT_LIBRARY_DIRS}
  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
  ${CMAKE_INSTALL_PREFIX}/lib/
  ${ITK_LIBRARY_DIRS}
  ${VTK_LIBRARY_DIRS}
)

# Now the work of copying dependencies into the bundle/package
# The quotes are escaped and variables to use at install time have their $ i# escaped
# An alternative is the do a configure_file() on a script and use 
# install(SCRIPT  ...).
# Note that the image plugins depend on QtSvg and QtXml, and it got those 
# copied over.

INSTALL( CODE "
  file( GLOB_RECURSE QTPLUGINS
    \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\" )
    include( BundleUtilities )
    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
  " COMPONENT Runtime 
  ) 

