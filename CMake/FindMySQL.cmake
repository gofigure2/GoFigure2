# - Find MySQL
# This module finds MySQL libraries.
#
#
# MYSQL_INCLUDE_DIR - Directories to include to use MySQL
# MYSQL_LIBRARIES   - Files to link against to use MySQL
# MYSQL_EXTRA_LIBRARIES - Additional files to link against to use MySQL
# MYSQL_FOUND, If false, you cannot build anything that requires MySQL.

find_path( MYSQL_INCLUDE_DIR NAMES mysql.h
  PATHS
  "$ENV{ProgramFiles}/MySQL/*/"
  "$ENV{SystemDrive}/MySQL/*/"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\*;Location]/"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\*;Location]/"
  "C:/Program Files/MySQL/include"
  "C:/Program Files/MySQL/MySQL Server 5.*/include"
  PATH_SUFFIXES "include" "include/mysql" "mysql/include"
  DOC "Specify the directory containing mysql.h"
)

set( MYSQL_LIBRARY_NAMES mysql libmysql mysqlclient mysqlclient_r )

find_library( MYSQL_LIBRARIES
  NAMES ${MYSQL_LIBRARY_NAMES}
  PATHS
  "$ENV{ProgramFiles}/MySQL/*/"
  "$ENV{SystemDrive}/MySQL/*/"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\*;Location]/lib/opt"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\*;Location]/lib/opt"
  "C:/Program Files/MySQL/MySQL Server 5.*/lib/opt"
  PATH_SUFFIXES "lib" "mysql" "mysql/lib" "mysql/lib/mysql" "lib/opt"
  DOC "Specify the mysql library here."
)

# On Windows you typically don't need to include any extra libraries
# to build MYSQL stuff.
if( NOT WIN32 )
  find_package( ZLIB )
  set( MYSQL_EXTRA_LIBRARIES ${ZLIB_LIBRARY} )

  include( FindPackageHandleStandardArgs )
  FIND_PACKAGE_HANDLE_STANDARD_ARGS( MYSQL DEFAULT_MSG
    MYSQL_INCLUDE_DIR
    MYSQL_LIBRARIES
    MYSQL_EXTRA_LIBRARIES
  )

  mark_as_advanced( MYSQL_FOUND
    MYSQL_LIBRARIES
    MYSQL_EXTRA_LIBRARIES
    MYSQL_INCLUDE_DIR
  )

else( NOT WIN32 )
  set( MYSQL_EXTRA_LIBRARIES "" )

  include( FindPackageHandleStandardArgs )
  FIND_PACKAGE_HANDLE_STANDARD_ARGS( MYSQL DEFAULT_MSG
    MYSQL_INCLUDE_DIR
    MYSQL_LIBRARIES
  )

  mark_as_advanced( MYSQL_FOUND
    MYSQL_LIBRARIES
    MYSQL_INCLUDE_DIR
  )
endif( NOT WIN32 )

