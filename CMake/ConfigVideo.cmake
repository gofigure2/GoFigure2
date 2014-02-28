set( VIDEO_SUPPORT )

#-----------------------------------------------------------------------------
# Is FFMPEG AVAILABLE ( FOR UNIX) MAC??
if( NOT WIN32 )

  find_package( FFMPEG )

  if( FFMPEG_FOUND )
    list( APPEND VIDEO_SUPPORT
      -DVTK_USE_FFMPEG_ENCODER:BOOL=ON
    )
  else( FFMPEG_FOUND )
    list( APPEND VIDEO_SUPPORT
      -DVTK_USE_FFMPEG_ENCODER:BOOL=OFF
    )
  endif( FFMPEG_FOUND )

# DO STH FOR WINDOWS
# turn on video by default
else( NOT WIN32 )
  list( APPEND VIDEO_SUPPORT
    -DVTK_USE_AVI_ENCODER:BOOL=ON
  )
endif( NOT WIN32 )
