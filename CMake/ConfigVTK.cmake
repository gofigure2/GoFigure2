
# Check the version of VTK
# GoFigure2 requires vtk >= 5.6
IF( ( ${VTK_MAJOR_VERSION} LESS 5 ) OR ( ${VTK_MINOR_VERSION} LESS 6 ) )
  MESSAGE( SEND_ERROR
    "GoFigure2 requires VTK 5.6 (your version of VTK is ${VTK_VERSION})" )
ENDIF( ( ${VTK_MAJOR_VERSION} LESS 5 ) OR ( ${VTK_MINOR_VERSION} LESS 6 ) )
  
# Check if version of VTK is > 5.6
# Requiered to now if we can use setBitRate() and setBitRateTolerance
# in the vtkFFMPEGWriter
IF( ( ${VTK_MAJOR_VERSION} EQUAL 5 ) AND ( ${VTK_MINOR_VERSION} GREATER 6 ))
  ADD_DEFINITIONS( -DVTKTRUNK )
ENDIF( ( ${VTK_MAJOR_VERSION} EQUAL 5 ) AND ( ${VTK_MINOR_VERSION} GREATER 6 ))

INCLUDE( ${VTK_USE_FILE} )

IF( ${VTK_USE_MYSQL} MATCHES "ON" )
  FIND_PACKAGE( MySQL REQUIRED )
  SET( MYSQL_LIBRARIES ${MYSQL_LIBRARIES} CACHE FILEPATH "" )
#    SET( MYSQL_EXTRA_LIBRARIES ${MYSQL_EXTRA_LIBRARIES} CACHE FILEPATH "" )
ELSE( ${VTK_USE_MYSQL} MATCHES "ON" )
  MESSAGE( SEND_ERROR "VTK must be compiled with MySQL support" )
  # TODO (arnaudgelas)
  # Here add a definition to be able to compile and use GoFigure
  # without any database support
  # ADD_DEFINITION( -DNODBSUPPORT )
ENDIF( ${VTK_USE_MYSQL} MATCHES "ON" )

#----------------------------------------------------------
# Determine if GoFigure has to be built in shared or static
# based on the configuration of VTK
IF( VTK_BUILD_SHARED_LIBS )
  SET( LIBS_STYLE "SHARED" )
  SET( BUILD_SHARED_LIBS "TRUE" )
  ADD_DEFINITIONS( -DGOFIGURE2_BUILD_SHARED_LIBS )
ELSE( VTK_BUILD_SHARED_LIBS )
  SET( LIBS_STYLE "STATIC" )
  SET( BUILD_SHARED_LIBS "FALSE" )
  REMOVE_DEFINITIONS( -DGOFIGURE2_BUILD_SHARED_LIBS )
ENDIF( VTK_BUILD_SHARED_LIBS )
#----------------------------------------------------------

    #---------------------------------------------------------------------
    #---------------------------------------------------------------------
    # VIDEO SUPPORT
    #---------------------------------------------------------------------
    #---------------------------------------------------------------------

    # FFMPEG: for Linux and Mac (tested and validated)
      IF( VTK_USE_FFMPEG_ENCODER )
        OPTION( ENABLE_VIDEO_RECORD_FFMPEG "VTK must be built with VTK_USE_FFMPEG_ENCODER" ON )
      ENDIF( VTK_USE_FFMPEG_ENCODER )

      IF( ENABLE_VIDEO_RECORD_FFMPEG )
        ADD_DEFINITIONS( -DENABLEFFMPEG )
      ELSE( ENABLE_VIDEO_RECORD_FFMPEG )
        REMOVE_DEFINITIONS( -DENABLEFFMPEG )
      ENDIF( ENABLE_VIDEO_RECORD_FFMPEG )

    # AVI: for Windows only
      IF( VTK_USE_VIDEO_FOR_WINDOWS )
        OPTION( ENABLE_VIDEO_RECORD_AVI "VTK must be built with VTK_USE_AVI_ENCODER" ON )
      ENDIF( VTK_USE_VIDEO_FOR_WINDOWS )

      IF( ENABLE_VIDEO_RECORD_AVI )
        ADD_DEFINITIONS( -DENABLEAVI )
      ELSE( ENABLE_VIDEO_RECORD_AVI )
        REMOVE_DEFINITIONS( -DENABLEAVI )
      ENDIF( ENABLE_VIDEO_RECORD_AVI )

