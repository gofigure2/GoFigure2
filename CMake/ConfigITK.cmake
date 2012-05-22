#----------------------------------------------------------
find_package( ITK REQUIRED )

if( ITK_FOUND )
  #----------------------------------------------------------
  include( ${ITK_USE_FILE} )

  message(STATUS "ITK libs: ${ITK_LIBRARIES}")

  #----------------------------------------------------------
  # Check the version of ITK
  # ( by checking the existence of a variable
  # can't check version: 4.0.0 is modularized and old style...)
  #----------------------------------------------------------
  if( ITK_VERSION_MAJOR EQUAL "4" )
    ADD_DEFINITIONS( -DITKv4 )

    if( NOT DEFINED ITK_MODULES_ENABLED )
      message( SEND_ERROR
        "WRONG VERSION OF ITK, REQUIERES MODULARIZED ITK, CMAKE WILL STOP NOW")
    ENDIF ( NOT DEFINED ITK_MODULES_ENABLED )

    # check if requiered modules are enabled
    # note that ITK-Review requires all modules to be turned ON...
    list(FIND ITK_MODULES_ENABLED ITKReview ITKReview_FOUND)
    if( ITKReview_FOUND LESS 0 )
      message( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-Review turned ON")
    endif( ITKReview_FOUND LESS 0 )

  else( ITK_VERSION_MAJOR EQUAL "4" )

    remove_definitions( -DITKv4 )
    # Make sure it is at least ITK 3
    if( ITK_VERSION_MAJOR EQUAL "3" )

      # Make sure it is ITK 3.20
      if( ITK_VERSION_MINOR EQUAL "20" )

        # Make sure it is ITK 3.20.1
        if( ITK_VERSION_PATCH EQUAL "1" )

          if( ( ${ITK_USE_REVIEW} MATCHES "OFF" ) OR
              ( ${ITK_USE_REVIEW_STATISTICS} MATCHES "OFF" ) )
            message( SEND_ERROR
              "GoFigure2 requires ITK to be compiled with USE_REVIEW"
              " and USE_REVIEW_STATISTICS turned ON" )
          endif()

        else( ITK_VERSION_PATCH EQUAL "1" )
          message( SEND_ERROR "GoFigure2 requires at ITK 3.20.1" )
        endif( ITK_VERSION_PATCH EQUAL "1" )

      else( ITK_VERSION_MINOR EQUAL "20" )

        message( SEND_ERROR "GoFigure2 requires at ITK 3.20.1" )

      endif( ITK_VERSION_MINOR EQUAL "20" )

    else( ITK_VERSION_MAJOR EQUAL "3" )

      message( SEND_ERROR "GoFigure2 requires at ITK 3.20.1" )

    endif( ITK_VERSION_MAJOR EQUAL "3" )

  endif( ITK_VERSION_MAJOR EQUAL "4" )

else( ITK_FOUND )
    message( SEND_ERROR "ITK NOT FOUND, CMAKE WILL STOP NOW")
endif( ITK_FOUND )
