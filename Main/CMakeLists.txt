configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/gofigure.cxx.in
  ${CMAKE_CURRENT_BINARY_DIR}/gofigure.cxx @ONLY IMMEDIATE
)

set( QGoMainWindow_UI
  go.ui
)

set( QGoGUISRC_HDRS
  QGoTabManager.h
  QGoMainWindow.h
  QGoPluginManager.h
  ${QGoGUIINTERFACES_HDRS}
)

set( GOFIGURE_MAIN
  ${CMAKE_CURRENT_BINARY_DIR}/gofigure.cxx
)

set( GOFIGURE_APPLICATION_SOURCE_LIST
  QGoTabManager.cxx
  QGoMainWindow.cxx
  QGoPluginManager.cxx
  )

if( BUILD_COMPARETOOL )
  set( QGoGUISRC_HDRS
    ${QGoGUISRC_HDRS}
    QGoSynchronizedViewMainWindow.h
  )

  set( GOFIGURE_APPLICATION_SOURCE_LIST
    ${GOFIGURE_APPLICATION_SOURCE_LIST}
    ${QGoSynchronizedViewMainWindow.cxx}
  )
endif( BUILD_COMPARETOOL )

include_directories( ${GOFIGURE2_BINARY_DIR}/Main/  )

QT4_WRAP_UI( QGoMainWindow_UI_H ${QGoMainWindow_UI} )

QT4_WRAP_CPP( QGoGUISRC_MOC ${QGoGUISRC_HDRS} )

set_source_files_properties(
  ${GOFIGURE_APPLICATION_SOURCE_LIST}
  PROPERTIES
  OBJECT_DEPENDS "${QGoMainWindow_UI_H}"
  )

set_source_files_properties(
  ${QGoGUISRC_MOC}
  PROPERTIES
  OBJECT_DEPENDS "${QGoMainWindow_UI_H}"
  )

QT4_ADD_RESOURCES( QGoMainWindow_QRC
  ${QGoResourceFile}
  )

set( QGoGUISRC_SRC
  ${QGoGUISRC_CXX}
  ${QGoGUISRC_MOC}
)

set( qt_menu_nib_sources )

if( APPLE )

  if( QT_MAC_USE_COCOA )
    get_filename_component( qt_menu_nib
     "@QT_QTGUI_LIBRARY_RELEASE@/Resources/qt_menu.nib"
      REALPATH )

    set( qt_menu_nib_sources
      "${qt_menu_nib}/classes.nib"
      "${qt_menu_nib}/info.nib"
      "${qt_menu_nib}/keyedobjects.nib"
      )

    set_source_files_properties(
      ${qt_menu_nib_sources}
      PROPERTIES
      MACOSX_PACKAGE_LOCATION Resources/qt_menu.nib
      )
  endif( QT_MAC_USE_COCOA )

  set( MACOSX_BUNDLE_ICON_FILE
    ${GOFIGURE2_SOURCE_DIR}/Resources/myapp.icns
    )

  set_source_files_properties(
    ${MACOSX_BUNDLE_ICON_FILE}
    PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
    )

  set( GOFIGURE_APPLICATION_SOURCE_LIST
    ${GOFIGURE_APPLICATION_SOURCE_LIST}
    ${MACOSX_BUNDLE_ICON_FILE}
    )

  set( MACOSX_BUNDLE_SHORT_VERSION_STRING
    ${GOFIGURE2_VERSION}
    )

  set( MACOSX_BUNDLE_VERSION
    ${GOFIGURE2_VERSION}
    )

  set( MACOSX_BUNDLE_LONG_VERSION_STRING
    Version ${GOFIGURE2_VERSION}
    )

  set( GOFIGURE2_INSTALL_LICENSE_DIR    ${plugin_dest_dir}/Licenses   )
  set( GOFIGURE2_INSTALL_RESOURCE_DIR  ${plugin_dest_dir}/Resources  )

endif( APPLE )

if( WIN32 )
  if( MINGW )
    add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/myapp.o
      COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
        -i ${GOFIGURE2_SOURCE_DIR}/Resources/myapp.rc
        -o ${CMAKE_CURRENT_BINARY_DIR}/myapp.o
        )

    set( ${GOFIGURE_APPLICATION_SOURCE_LIST}
      ${GOFIGURE_APPLICATION_SOURCE_LIST}
      ${CMAKE_CURRENT_BINARY_DIR}/myapp.o
      )
  else( MINGW )
    set( GOFIGURE_APPLICATION_SOURCE_LIST
      ${GOFIGURE_APPLICATION_SOURCE_LIST}
      ${GOFIGURE2_SOURCE_DIR}/Resources/myapp.rc
      )
  endif( MINGW )

  #-------------------------------------------------------------------------
  # This sets the windows build that will need the special winmain@16 call.
  # Qt provides this for us in the qtmain.lib file. Using this cmake code
  # will ensure we have it linked into our build. Not needed on
  # Unix/OS X/Linux which is why we have the if(WIN32) conditional.
  set( QT_USE_QTMAIN TRUE )

endif( WIN32 )

# --------
# gofigure
add_executable( gofigure
  MACOSX_BUNDLE
  ${GOFIGURE_MAIN}
  ${GOFIGURE_APPLICATION_SOURCE_LIST}
  ${QGoGUIINTERFACES_SRC}
  ${QGoGUISRC_MOC}
  ${QGoMainWindow_QRC}
  ${qt_menu_nib_sources}
)

target_link_libraries( gofigure
  QGoGui
  QGoIO
  itkQt
  ${Boost_LIBRARIES}
  ${ITK_LIBRARIES}
 )

if( BUILD_COMPARETOOL )
  # comparegui

  add_executable( comparegui
    comparegui.cxx
    ${GOFIGURE_APPLICATION_SOURCE_LIST}
    ${QGoGUIINTERFACES_SRC}
    ${QGoGUISRC_MOC}
    ${QGoMainWindow_QRC}
    ${qt_menu_nib_sources}
  )

  target_link_libraries( comparegui
    QGoGui
    QGoIO
    itkQt
  )
endif( BUILD_COMPARETOOL )


#---------------------------------------------------------------------------
# Now the installation stuff below
#---------------------------------------------------------------------------
set( plugin_dest_dir ${GOFIGURE2_INSTALL_BIN_DIR} )
set( qtconf_dest_dir ${GOFIGURE2_INSTALL_BIN_DIR} )

if( APPLE )
  set( plugin_dest_dir gofigure.app/Contents/MacOS )
  set( qtconf_dest_dir gofigure.app/Contents/Resources )
  set( APPS
    "\${CMAKE_INSTALL_PREFIX}/gofigure.app" )
#    "\${CMAKE_INSTALL_PREFIX}/${GOFIGURE2_INSTALL_BUNDLE_DIR}/gofigure.app" )
endif( APPLE )

if( WIN32 )
  set( APPS
    "\${CMAKE_INSTALL_PREFIX}/${GOFIGURE2_INSTALL_BIN_DIR}/gofigure.exe" )
endif(WIN32)

if( UNIX AND NOT APPLE )
  set( APPS
    "\${CMAKE_INSTALL_PREFIX}/${GOFIGURE2_INSTALL_BIN_DIR}/gofigure" )

  if( NOT DESKTOP_ENTRY )
    set( DESKTOP_ENTRY gofigure2.desktop )
  endif( NOT DESKTOP_ENTRY )

  configure_file( ${GOFIGURE2_SOURCE_DIR}/Main/gf2-desktop.sh.in
    ${GOFIGURE2_BINARY_DIR}/Main/gf2-desktop.sh @ONLY IMMEDIATE
  )

  add_custom_command( OUTPUT ${DESKTOP_ENTRY}
    COMMAND touch ${DESKTOP_ENTRY}
    COMMAND sh ${GOFIGURE2_BINARY_DIR}/Main/gf2-desktop.sh
      ${CMAKE_INSTALL_PREFIX} >${DESKTOP_ENTRY}
    DEPENDS ${GOFIGURE2_BINARY_DIR}/Main/gf2-desktop.sh
    COMMENT "Generating desktop entry file"
  )

  add_custom_target( DESKTOP_ENTRY_FILE ALL
    DEPENDS ${DESKTOP_ENTRY}
  )

  set( GOFIGURE2_ICON ${GOFIGURE2_SOURCE_DIR}/Resources/fig/Myapp2.png )

  install( FILES ${GOFIGURE2_ICON}
    DESTINATION share/gofigure2/icons COMPONENT Runtime )
endif( UNIX AND NOT APPLE )

# message( ${APPS} )

#---------------------------------------------------------------------------
# Install gofigure, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.

install( TARGETS gofigure
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  BUNDLE  DESTINATION ${GOFIGURE2_INSTALL_BUNDLE_DIR} COMPONENT Runtime
  )

#---------------------------------------------------------------------------

if( WIN32 OR APPLE )
  set( USE_BUNDLE_UTILITIES TRUE )
else()
  option( INSTALLED_3RD_PART "Are Qt, VTK, ITK... installed?" TRUE )
  set( USE_BUNDLE_UTILITIES FALSE )
endif()

if( USE_BUNDLE_UTILITIES )
  #---------------------------------------------------------------------------
  # Use BundleUtilities to get all other dependencies for the application to
  # work. It takes a bundle or executable along with possible plugins and
  # inspects it for dependencies.  If they are not system dependencies,
  # they are copied.

  # directories to look for dependencies
  set( DIRS
#    ${QT_LIBRARY_DIR}
#    ${QT_PLUGINS_DIR}
#    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
#    ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
#    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#    ${ITK_LIBRARY_DIRS}
#    ${VTK_LIBRARY_DIRS}
#    ${FFMPEG_LIBRARY_DIRS}
    " "
  )

  # Now the work of copying dependencies into the bundle/package
  # The quotes are escaped and variables to use at install time have their $ i# escaped
  # An alternative is the do a configure_file() on a script and use
  # install(SCRIPT  ...).
  # Note that the image plugins depend on QtSvg and QtXml, and it got those
  # copied over.
  install( CODE "
    file( GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\" )
      include( BundleUtilities )
      fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
    "
    COMPONENT Runtime
    )

endif( USE_BUNDLE_UTILITIES )

if( APPLE )
  #---------------------------------------------------------------------------
  # install a qt.conf file
  # this inserts some cmake code into the install script to write the file
  install( CODE "
    file( WRITE \"\${CMAKE_INSTALL_PREFIX}/${GOFIGURE2_INSTALL_BUNDLE_DIR}/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Runtime
    )
endif( APPLE )

#-----------------------------------------------------
# Here Let's take care of License and Resoources files
file( GLOB
  GOFIGURE2_LICENSE_FILES
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Licenses/*.txt"
  )

file( GLOB
  GOFIGURE2_RESOURCES_FILE
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Resources/*.txt"
  )
  
# licence files
install( FILES ${GOFIGURE2_LICENSE_FILES}
  DESTINATION ${GOFIGURE2_INSTALL_LICENSE_DIR}
  COMPONENT Runtime
)

# resources files
install( FILES ${GOFIGURE2_RESOURCES_FILE}
  DESTINATION ${GOFIGURE2_INSTALL_RESOURCE_DIR}
  COMPONENT Runtime
)

file( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )

install( FILES ${__source_file_h} ${QGoMainWindow_UI_H}
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)
