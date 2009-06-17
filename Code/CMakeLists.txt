SUBDIRS( ExternalCode )
# following directories do not contain any CMakeLists.txt
# SUBDIRS( Vizualisation )
SUBDIRS( IO )
SUBDIRS( GUI )

IF( ITK_MINOR_VERSION GREATER 14 )
  SUBDIRS( Filters )
ENDIF( ITK_MINOR_VERSION GREATER 14 )
