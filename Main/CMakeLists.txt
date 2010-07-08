CONFIGURE_FILE( ${GOFIGURE2_SOURCE_DIR}/Main/gofigure.cxx.in
  ${GOFIGURE2_BINARY_DIR}/Main/gofigure.cxx @ONLY IMMEDIATE
)

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
  ${GOFIGURE2_BINARY_DIR}/Main/gofigure.cxx
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
    GET_FILENAME_COMPONENT( qt_menu_nib
     "@QT_QTGUI_LIBRARY_RELEASE@/Resources/qt_menu.nib"
      REALPATH )
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

  #---------------------------------------------------------------------------
  # This sets the windows build that will need the special winmain@16 call. 
  # Qt provides this for us in the qtmain.lib file. Using this cmake code 
  # will ensure we have it linked into our build. Not needed on 
  # Unix/OS X/Linux which is why we have the IF(WIN32) conditional.
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

TARGET_LINK_LIBRARIES( gofigure
  QGoGui
  QGoIO
  itkQt
 )

#---------------------------------------------------------------------------
# Now the installation stuff below
#---------------------------------------------------------------------------
SET( plugin_dest_dir "." )
SET( qtconf_dest_dir ${GOFIGURE2_INSTALL_BIN_DIR} )
SET( APPS "\${GOFIGURE2_INSTALL_BIN_DIR}/gofigure" )

IF( APPLE )
  SET( plugin_dest_dir gofigure.app/Contents/MacOS )
  SET( qtconf_dest_dir gofigure.app/Contents/Resources )
  SET( APPS "${CMAKE_INSTALL_PREFIX}/${GOFIGURE2_INSTALL_BUNDLE_DIR}/gofigure.app" )
ENDIF( APPLE )

IF( WIN32 )
  SET( APPS "${GOFIGURE2_INSTALL_BIN_DIR}/gofigure.exe" )
ENDIF(WIN32)

IF( UNIX AND NOT APPLE )
  SET( APPS "${GOFIGURE2_INSTALL_BIN_DIR}/gofigure" )
ENDIF( UNIX AND NOT APPLE )

#---------------------------------------------------------------------------
# Install gofigure, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.

INSTALL( TARGETS gofigure
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Application
  BUNDLE  DESTINATION ${GOFIGURE2_INSTALL_BUNDLE_DIR} COMPONENT Application
  )

#-----------------------------------------------------
# Here Let's take care of License files
# copying them into the build's folder
# and during the installation process
FILE( GLOB 
  GOFIGURE2_LICENSE_FILES 
  "${GOFIGURE2_SOURCE_DIR}/Licenses/*.txt" 
  )

IF( UNIX AND NOT APPLE )
  ADD_CUSTOM_COMMAND( TARGET gofigure POST_BUILD
    COMMENT "Copying License files..."
    COMMAND mkdir ARGS -p
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Licenses
    COMMAND cp ARGS -rf
      ${GOFIGURE2_SOURCE_DIR}/Licenses/*.txt
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Licenses
  )
ELSE( UNIX AND NOT APPLE )
  IF( WIN32 )
    FILE( TO_NATIVE_PATH ${GOFIGURE2_SOURCE_DIR} GOFIGURE2_SRC_DIR_WIN )
    FILE( TO_NATIVE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} OUTPUT_DIR_WIN )

    ADD_CUSTOM_COMMAND( TARGET gofigure POST_BUILD
      COMMENT "Copying License files..."
      COMMAND if not exist \"${OUTPUT_DIR_WIN}\\Licenses\" 
        mkdir \"${OUTPUT_DIR_WIN}\\Licenses\"
      COMMAND xcopy ARGS /Y
        \"${GOFIGURE2_SRC_DIR_WIN}\\Licenses\\*.txt\"
        \"${OUTPUT_DIR_WIN}\\Licenses\\\"
     )
  ELSE( WIN32 )
    IF( APPLE )
      ADD_CUSTOM_COMMAND( TARGET gofigure POST_BUILD
        COMMENT "Copying License files..."
        COMMAND mkdir ARGS -p
          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${plugin_dest_dir}/Licenses
        COMMAND cp ARGS -rf
          ${GOFIGURE2_SOURCE_DIR}/Licenses/*.txt
          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${plugin_dest_dir}/Licenses
        )
    ENDIF( APPLE )
  ENDIF( WIN32 )
ENDIF( UNIX AND NOT APPLE )

INSTALL( FILES ${GOFIGURE2_LICENSE_FILES} 
  DESTINATION ${GOFIGURE2_INSTALL_LICENSE_DIR} COMPONENT Application 
  )
#-----------------------------------------------------

#---------------------------------------------------------------------------
# install a qt.conf file
# this inserts some cmake code into the install script to write the file
IF( APPLE )
  INSTALL( CODE "
    file( WRITE \"\${GOFIGURE2_INSTALL_BUNDLE_DIR}/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Application 
    )
ENDIF( APPLE )

#---------------------------------------------------------------------------
# Use BundleUtilities to get all other dependencies for the application to 
# work. It takes a bundle or executable along with possible plugins and 
# inspects it for dependencies.  If they are not system dependencies, 
# they are copied.

# directories to look for dependencies
SET( DIRS 
  ${QT_LIBRARY_DIRS}
  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
  ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  ${GOFIGURE2_INSTALL_LIB_DIR}
  ${GOFIGURE2_INSTALL_BIN_DIR}
  ${GOFIGURE2_INSTALL_BUNDLE_DIR}
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
    \"\${GOFIGURE2_INSTALL_BIN_DIR}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\" )
    include( BundleUtilities )
    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
  " COMPONENT Runtime 
  ) 

IF( APPLE )
  INSTALL( FILES ${GOFIGURE2_LICENSE_FILES}
    DESTINATION ${plugin_dest_dir}/Licenses
    COMPONENT Runtime
    )
ENDIF( APPLE )
