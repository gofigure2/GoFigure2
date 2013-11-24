#----------------------------------------------------------
find_package( VTK REQUIRED )
  if( VTK_FOUND )
  #----------------------------------------------------------
  include( ${VTK_USE_FILE} )

  # check for Qt installer

  #----------------------------------------------------------
  # Check the version of VTK
  # GoFigure2 requires vtk >= 5.6
  if( ( ${VTK_MAJOR_VERSION} LESS 6 ) OR ( ${VTK_MINOR_VERSION} LESS 0 ) )
    message( SEND_ERROR
      "GoFigure2 requires VTK 6.0 (your version of VTK is ${VTK_VERSION})" )
  endif( ( ${VTK_MAJOR_VERSION} LESS 6 ) OR ( ${VTK_MINOR_VERSION} LESS 0 ) )

  #----------------------------------------------------------
  # Check if version of VTK is > 5.6
  # Required to now if we can use setBitRate() and setBitRateTolerance
  # in the vtkFFMPEGWriter
  if( ( ${VTK_MAJOR_VERSION} EQUAL 5 ) AND ( ${VTK_MINOR_VERSION} GREATER 6 ))
    add_definitions( -DVTKTRUNK )
  endif( ( ${VTK_MAJOR_VERSION} EQUAL 5 ) AND ( ${VTK_MINOR_VERSION} GREATER 6 ))

  #----------------------------------------------------------
  if( NOT ${VTK_USE_BOOST} MATCHES "ON" )
    message( SEND_ERROR "VTK must be compiled with Boost support" )
  endif( NOT ${VTK_USE_BOOST} MATCHES "ON" )

  #----------------------------------------------------------
  # Check if mysql is enabled
  if( NOT ${VTK_USE_MYSQL} MATCHES "ON" )
  #  find_package( MySQL REQUIRED )
  #  set( MYSQL_LIBRARIES ${MYSQL_LIBRARIES} CACHE FILEPATH "" )
  #    set( MYSQL_EXTRA_LIBRARIES ${MYSQL_EXTRA_LIBRARIES} CACHE FILEPATH "" )
  #else( ${VTK_USE_MYSQL} MATCHES "ON" )
    message( SEND_ERROR "VTK must be compiled with MySQL support" )
    # TODO (arnaudgelas)
    # Here add a definition to be able to compile and use GoFigure
    # without any database support
    # ADD_DEFINITION( -DNODBSUPPORT )
  endif( NOT ${VTK_USE_MYSQL} MATCHES "ON" )

  #----------------------------------------------------------
  # Determine if GoFigure has to be built in shared or static
  # based on the configuration of VTK
  if( VTK_BUILD_SHARED_LIBS )
    set( LIBS_STYLE "SHARED" )
    set( BUILD_SHARED_LIBS "TRUE" )
    add_definitions( -DGOFIGURE2_BUILD_SHARED_LIBS )
  else( VTK_BUILD_SHARED_LIBS )
    set( LIBS_STYLE "STATIC" )
    set( BUILD_SHARED_LIBS "FALSE" )
    remove_definitions( -DGOFIGURE2_BUILD_SHARED_LIBS )
  endif( VTK_BUILD_SHARED_LIBS )

  #----------------------------------------------------------
  # Check if we can enable the video support
  # FFMPEG: for Linux and Mac (tested and validated)
  if( VTK_USE_FFMPEG_ENCODER )
    option( ENABLE_VIDEO_RECORD_FFMPEG "VTK must be built with VTK_USE_FFMPEG_ENCODER" ON )
  endif( VTK_USE_FFMPEG_ENCODER )

  if( ENABLE_VIDEO_RECORD_FFMPEG )
    add_definitions( -DENABLEFFMPEG )
  else( ENABLE_VIDEO_RECORD_FFMPEG )
    remove_definitions( -DENABLEFFMPEG )
  endif( ENABLE_VIDEO_RECORD_FFMPEG )

  # AVI: for Windows only
  if( VTK_USE_VIDEO_FOR_WINDOWS )
    option( ENABLE_VIDEO_RECORD_AVI "VTK must be built with VTK_USE_AVI_ENCODER" ON )
  endif( VTK_USE_VIDEO_FOR_WINDOWS )

  if( ENABLE_VIDEO_RECORD_AVI )
    add_definitions( -DENABLEAVI )
  else( ENABLE_VIDEO_RECORD_AVI )
    remove_definitions( -DENABLEAVI )
  endif( ENABLE_VIDEO_RECORD_AVI )

else( VTK_FOUND )
    message( SEND_ERROR "VTK NOT FOUND, CMAKE WILL STOP NOW")
endif( VTK_FOUND )
