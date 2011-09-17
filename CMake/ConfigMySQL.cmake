FIND_PACKAGE( MySQL REQUIRED )
IF( MYSQL_FOUND )    
  SET( MYSQL_LIBRARIES ${MYSQL_LIBRARIES} )
  SET( MYSQL_INCLUDE_DIR ${MYSQL_INCLUDE_DIR} )
ELSE( MYSQL_FOUND )
  MESSAGE( SEND_ERROR "MySQL NOT FOUND")
ENDIF( MYSQL_FOUND )