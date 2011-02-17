#-----------------------------------------------------------------------------
# Get and build FFMPEG
# ONLY MAC AND LINUX
# TODO BUILD GOOD ARCH ON MAC i386 or x86_64????
#-----------------------------------------------------------------------------
set(proj FFMPEG)

ExternalProject_Add(${proj}
  # Set up dirs
  SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}

  # get the project
  GIT_REPOSITORY "${git_protocol}://git.ffmpeg.org/ffmpeg.git"

  # Build the project
  BUILD_IN_SOURCE 1
  
  # Configure step
  # DO STH FOR THE ARCHITECTURE...
  CONFIGURE_COMMAND ${CMAKE_BINARY_DIR}/${proj}/configure
   --enable-shared --enable-gpl --enable-avfilter --disable-yasm

  # Install the project
  INSTALL_COMMAND ""
)

set( VIDEO_SUPPORT -DVTK_USE_FFMPEG_ENCODER:BOOL=ON
                   -DFFMPEG_INCLUDE_DIR=STRING:${CMAKE_BINARY_DIR}/${proj}
                   -DFFMPEG_avcodec_LIBRARY=STRING:${CMAKE_BINARY_DIR}/${proj}/libavcodec/libavcodec.dylib            
                   -DFFMPEG_avformat_LIBRARY=STRING:${CMAKE_BINARY_DIR}/${proj}/libavformat/libavformat.dylib           
                   -DFFMPEG_avutil_LIBRARY=STRING:${CMAKE_BINARY_DIR}/${proj}/libavutil/lib/libavutil.dylib              
                   -DFFMPEG_swscale_LIBRARY=STRING:${CMAKE_BINARY_DIR}/${proj}/libswscale/libswscale.dylib )
