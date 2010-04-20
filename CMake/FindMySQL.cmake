#
# Find the MySQL client includes and library
# 

# This module defines
# MYSQL_INCLUDE_DIRECTORIES, where to find mysql.h
# MYSQL_LIBRARIES, the libraries to link against to connect to MySQL
# MYSQL_FOUND, If false, you cannot build anything that requires MySQL.

# also defined, but not for general use are
# MYSQL_LIBRARY, where to find the MySQL library.

# 
# XXX NOTE: This is not yet for general use.  I'm pretty sure there
# are other libraries I have to link against at the same time.
#

SET( MYSQL_FOUND 0 )

FIND_PATH(MYSQL_INCLUDE_DIRECTORIES mysql.h
  /usr/include
  /usr/include/mysql
  /usr/local/include
  /usr/local/include/mysql
  /usr/local/mysql/include
  /usr/local/mysql/include/mysql
  "C:/Program Files/MySQL/include"
  "C:/Program Files/MySQL/MySQL Server 5.1/include"
  "C:/Program Files (x86)/MySQL/MySQL Server 5.1/include"
  "C:/MySQL/include"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.1;Location]/include"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.1;Location]/include"
  DOC "Specify the directory containing mysql.h."
)

FIND_LIBRARY( MYSQL_LIBRARY 
  NAMES mysql libmysql mysqlclient
  PATHS
  /usr/lib
  /usr/lib64
  /usr/lib/mysql
  /usr/lib64/mysql
  /usr/local/lib
  /usr/local/lib/mysql
  /usr/local/mysql/lib
  /usr/local/mysql/lib/mysql
  "C:/Program Files/MySQL/lib"
  "C:/Program Files/MySQL/MySQL Server 5.1/lib/opt"
  "C:/MySQL/lib/opt"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.1;Location]/lib/opt"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.1;Location]/lib/opt"
  DOC "Specify the mysql library here."
)

# On Windows you typically don't need to include any extra libraries
# to build MYSQL stuff.

IF (NOT WIN32)
  FIND_LIBRARY( MYSQL_EXTRA_LIBRARIES
                NAMES z zlib
                PATHS /usr/lib /usr/lib64 /usr/local/lib 
                DOC "If more libraries are necessary to link in a MySQL client (typically zlib), specify them here.")
ELSE (NOT WIN32)
  SET( MYSQL_EXTRA_LIBRARIES "" )
ENDIF (NOT WIN32)


IF (MYSQL_LIBRARY)
  IF (MYSQL_INCLUDE_DIRECTORIES)
    SET( MYSQL_FOUND 1 )
  ENDIF (MYSQL_INCLUDE_DIRECTORIES)
ENDIF (MYSQL_LIBRARY)

MARK_AS_ADVANCED( MYSQL_FOUND MYSQL_LIBRARY MYSQL_EXTRA_LIBRARIES MYSQL_INCLUDE_DIRECTORIES )
