#----------------------------------------------------------
#INCLUDE( ${QT_USE_FILE} )

#----------------------------------------------------------
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
