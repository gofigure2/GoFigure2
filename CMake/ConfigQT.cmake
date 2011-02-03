#----------------------------------------------------------
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


