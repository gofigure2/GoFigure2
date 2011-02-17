#-----------------------------------------------------------------------------
# Get and build FFMPEG
# ONLY MAC AND LINUX
# TODO BUILD GOOD ARCH ON MAC i386 or x86_64????
#-----------------------------------------------------------------------------
set(proj FFMPEG)

IF( SUPER_SHARED_LIBS )
  set( SHARED_FFMPEG --enable-shared )
ENDIF( SUPER_SHARED_LIBS )

ExternalProject_Add(${proj}
  # Set up dirs
  SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}

  # get the project
  GIT_REPOSITORY "${git_protocol}://git.ffmpeg.org/ffmpeg.git"

  # Build the project
  BUILD_IN_SOURCE 1
  
  # Configure step
  # DO STH FOR THE ARCHITECTURE...
  CONFIGURE_COMMAND <SOURCE_DIR>/configure ${SHARED_FFMPEG} --enable-gpl --enable-avfilter --disable-yasm

  # Install the project
  INSTALL_COMMAND ""
)

# REQUIRE FIND_LIBRARY TO GET THE PLATFORM SPECIFIC EXTENSIONS (so, a, dylib...)
FIND_LIBRARY(FFMPEG_avformat_LIBRARY avformat
  ${CMAKE_BINARY_DIR}/${proj}/libavformat
)

FIND_LIBRARY(FFMPEG_avcodec_LIBRARY avcodec
  ${CMAKE_BINARY_DIR}/${proj}/libavcodec
)

FIND_LIBRARY(FFMPEG_avutil_LIBRARY avutil
  ${CMAKE_BINARY_DIR}/${proj}/libavutil
)

FIND_LIBRARY(FFMPEG_swscale_LIBRARY swscale
  ${CMAKE_BINARY_DIR}/${proj}/libswscale
)

set( VIDEO_SUPPORT -DVTK_USE_FFMPEG_ENCODER:BOOL=ON
                   -DVTK_FFMPEG_HAS_OLD_HEADER:BOOL=FALSE
                   -DFFMPEG_INCLUDE_DIR:PATH=${CMAKE_BINARY_DIR}/${proj}
                   -DFFMPEG_avcodec_LIBRARY:FILEPATH=${FFMPEG_avcodec_LIBRARY}          
                   -DFFMPEG_avformat_LIBRARY:FILEPATH=${FFMPEG_avformat_LIBRARY}          
                   -DFFMPEG_avutil_LIBRARY:FILEPATH=${FFMPEG_avutil_LIBRARY}            
                   -DFFMPEG_swscale_LIBRARY:FILEPATH=${FFMPEG_swscale_LIBRARY}
)
