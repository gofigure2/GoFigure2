#----------------------------------------------------------
FIND_PACKAGE( ITK REQUIRED )
  IF( ITK_FOUND )
  #----------------------------------------------------------
  INCLUDE( ${ITK_USE_FILE} )

  #----------------------------------------------------------
  # Check the version of ITK
  # ( by checking the existence of a variable
  # can't check version: 4.0.0 is modularized and old style...)
  #----------------------------------------------------------
  IF( NOT DEFINED ITK_MODULES_ENABLED )
    MESSAGE( SEND_ERROR "WRONG VERSION OF ITK, REQUIERES MODULARIZED ITK, CMAKE WILL STOP NOW")
  ENDIF ( NOT DEFINED ITK_MODULES_ENABLED )

  # check if requiered modules are enabled
  # note that ITK-Review requires all modules to be turned ON...
  list(FIND ITK_MODULES_ENABLED ITK-Review ITK-Review_FOUND)
  IF( ITK-Review_FOUND LESS 0 )
    MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-Review turned ON")
  ENDIF( ITK-Review_FOUND LESS 0 )

ELSE( ITK_FOUND )
    MESSAGE( SEND_ERROR "ITK NOT FOUND, CMAKE WILL STOP NOW")
ENDIF( ITK_FOUND )
