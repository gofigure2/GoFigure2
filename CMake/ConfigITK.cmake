#----------------------------------------------------------
FIND_PACKAGE( ITK REQUIRED )
IF( ITK_FOUND )

  #----------------------------------------------------------
  INCLUDE( ${ITK_USE_FILE} )

  #----------------------------------------------------------
  # Check the version of ITK
  IF( ( ${ITK_VERSION_MAJOR} LESS 3 ) OR
    ( ( ${ITK_VERSION_MAJOR} EQUAL 3 ) AND
      ( ${ITK_VERSION_MINOR} LESS 18 ) ) ) 
    MESSAGE( SEND_ERROR
      "GoFigure2 requires ITK 3.18 or newer version (your version of ITK is ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_PATCH})" )
  ENDIF( ( ${ITK_VERSION_MAJOR} LESS 3 ) OR
       ( ( ${ITK_VERSION_MAJOR} EQUAL 3 ) AND
         ( ${ITK_VERSION_MINOR} LESS 18 ) ) ) 

  #----------------------------------------------------------
  # Check if "REVIEW" is "ON"
  IF( ${ITK_VERSION_MAJOR} EQUAL 3 ) 
    IF( ( ${ITK_USE_REVIEW} MATCHES "OFF" ) OR 
        ( ${ITK_USE_REVIEW_STATISTICS} MATCHES "OFF" ) )
      MESSAGE( SEND_ERROR
        "GoFigure2 requires ITK to be compiled with USE_REVIEW and USE_REVIEW_STATISTICS turned ON" )
     ENDIF( ( ${ITK_USE_REVIEW} MATCHES "OFF" ) OR 
      ( ${ITK_USE_REVIEW_STATISTICS} MATCHES "OFF" ) )
  ELSE( ${ITK_VERSION_MAJOR} EQUAL 3 )
    IF( ${ITK_USE_REVIEW} MATCHES "OFF" )
      MESSAGE( SEND_ERROR
        "GoFigure2 requires ITK to be compiled with USE_REVIEW turned ON" )
    ENDIF( ${ITK_USE_REVIEW} MATCHES "OFF" )
  ENDIF( ${ITK_VERSION_MAJOR} EQUAL 3 ) 

ELSE( ITK_FOUND )
    MESSAGE( SEND_ERROR "ITK NOT FOUND, CMAKE WILL STOP NOW")
ENDIF( ITK_FOUND )