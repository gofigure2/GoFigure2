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
  IF( ITK_VERSION_MAJOR EQUAL "4" )

    ADD_DEFINITIONS( -DITKv4 )

    IF( NOT DEFINED ITK_MODULES_ENABLED )
      MESSAGE( SEND_ERROR
        "WRONG VERSION OF ITK, REQUIERES MODULARIZED ITK, CMAKE WILL STOP NOW")
    ENDIF ( NOT DEFINED ITK_MODULES_ENABLED )

    # check if requiered modules are enabled
    # note that ITK-Review requires all modules to be turned ON...
    list(FIND ITK_MODULES_ENABLED ITKReview ITKReview_FOUND)
    IF( ITKReview_FOUND LESS 0 )
      MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-Review turned ON")
    ENDIF( ITKReview_FOUND LESS 0 )

  ELSE( ITK_VERSION_MAJOR EQUAL "4" )

    REMOVE_DEFINITIONS( -DITKv4 )
    # Make sure it is at least ITK 3
    IF( ITK_VERSION_MAJOR EQUAL "3" )

      # Make sure it is ITK 3.20
      IF( ITK_VERSION_MINOR EQUAL "20" )

        # Make sure it is ITK 3.20.1
        IF( ITK_VERSION_PATCH EQUAL "1" )

          IF( ( ${ITK_USE_REVIEW} MATCHES "OFF" ) OR
              ( ${ITK_USE_REVIEW_STATISTICS} MATCHES "OFF" ) )
            MESSAGE( SEND_ERROR
              "GoFigure2 requires ITK to be compiled with USE_REVIEW"
              " and USE_REVIEW_STATISTICS turned ON" )
          ENDIF()

        ELSE( ITK_VERSION_PATCH EQUAL "1" )
          MESSAGE( SEND_ERROR "GoFigure2 requires at ITK 3.20.1" )
        ENDIF( ITK_VERSION_PATCH EQUAL "1" )

      ELSE( ITK_VERSION_MINOR EQUAL "20" )

        MESSAGE( SEND_ERROR "GoFigure2 requires at ITK 3.20.1" )

      ENDIF( ITK_VERSION_MINOR EQUAL "20" )

    ELSE( ITK_VERSION_MAJOR EQUAL "3" )

      MESSAGE( SEND_ERROR "GoFigure2 requires at ITK 3.20.1" )

    ENDIF( ITK_VERSION_MAJOR EQUAL "3" )

  ENDIF( ITK_VERSION_MAJOR EQUAL "4" )

ELSE( ITK_FOUND )
    MESSAGE( SEND_ERROR "ITK NOT FOUND, CMAKE WILL STOP NOW")
ENDIF( ITK_FOUND )
