#----------------------------------------------------------
FIND_PACKAGE( Qt4 REQUIRED )
  IF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )

  #INCLUDE( ${QT_USE_FILE} )

  #----------------------------------------------------------
  # Check version of QT
  IF("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}" VERSION_LESS "${minimum_required_qt_version}")
    MESSAGE(FATAL_ERROR "error: GoFigure2 requires Qt >= ${minimum_required_qt_version} -- you cannot use Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}.")
  ENDIF()

  #----------------------------------------------------------
  # How does QT_USE_FILE work??
  IF( QT_USE_FILE )
    SET( QT_USE_QT3SUPPORT FALSE )
    SET( QT_USE_QTCORE TRUE )
    SET( QT_USE_QTGUI TRUE )
    INCLUDE( ${QT_USE_FILE} )
  ELSE( QT_USE_FILE )
    SET( QT_LIBRARIES ${QT_QT_LIBRARY} )
  ENDIF( QT_USE_FILE )

  SET( QT_PLUGIN_LIBS ${QT_LIBRARIES} )

  ADD_DEFINITIONS( ${QT_DEFINITIONS} )
  #
  # ADD_DEFINITIONS( -DQT_GUI_LIBS )
  # ADD_DEFINITIONS( -DQT_CORE_LIB )

  # Try to extract Qt variable from VTK
  SET( QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "" )
  SET( QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "" )
  SET( QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "" )

ELSE( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )
    MESSAGE( SEND_ERROR "GoFigure2 requires Qt4.5 (or newer version) to be installed" )
ENDIF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )
