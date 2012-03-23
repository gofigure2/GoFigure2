#----------------------------------------------------------
find_package( Qt4 REQUIRED QtCore QtGui )
  if( QT4_FOUND )

  #----------------------------------------------------------
  # Check version of QT
  if("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}" VERSION_LESS "${minimum_required_qt_version}")
    message(FATAL_ERROR "error: GoFigure2 requires Qt >= ${minimum_required_qt_version} -- you cannot use Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}.")
  endif()

  #----------------------------------------------------------
  # How does QT_USE_FILE work??
  if( QT_USE_FILE )
    set( QT_USE_QT3SUPPORT FALSE )
    set( QT_USE_QTCORE TRUE )
    set( QT_USE_QTGUI TRUE )
    include( ${QT_USE_FILE} )
  else( QT_USE_FILE )
    set( QT_LIBRARIES ${QT_QT_LIBRARY} )
  endif( QT_USE_FILE )

  set( QT_PLUGIN_LIBS ${QT_LIBRARIES} )

  add_definitions( ${QT_DEFINITIONS} )
  #
  # add_definitions( -DQT_GUI_LIBS )
  # add_definitions( -DQT_CORE_LIB )

  # Try to extract Qt variable from VTK
  set( QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "" )
  set( QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "" )
  set( QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "" )

endif( QT4_FOUND )
