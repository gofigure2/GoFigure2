#-----------------------------------------------------------------------------
# Get and build FFMPEG
# ONLY MAC AND LINUX
# TODO BUILD GOOD ARCH ON MAC i386 or x86_64????
#-----------------------------------------------------------------------------
set(proj FFMPEG)

IF( SUPER_SHARED_LIBS )
  set( SHARED_FFMPEG --enable-shared )
ENDIF( SUPER_SHARED_LIBS )

SET(GF2_SRC_DIR ${CMAKE_CURRENT_SRC_DIR})

ExternalProject_Add(${proj}
  # Set up dirs
  SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}

  # get the project
  GIT_REPOSITORY "${git_protocol}://git.ffmpeg.org/ffmpeg.git"

  # Build the project
  BUILD_IN_SOURCE 1
  
  # Configure step
  # DO STH FOR THE ARCHITECTURE...
  CONFIGURE_COMMAND <SOURCE_DIR>/configure ${SHARED_FFMPEG} --enable-gpl --enable-avfilter --disable-yasm --disable-decoders --disable-zlib --disable-demuxer=matroska

  # Install the project
  INSTALL_COMMAND ""
)

# define the library suffix
IF( SUPER_SHARED_LIBS )
  IF( APPLE )
    SET(LIBRARY_SUFFIX .dylib)
  ELSE( APPLE )
    SET(LIBRARY_SUFFIX .so)
  ENDIF( APPLE )
ELSE( SUPER_SHARED_LIBS )
  SET(LIBRARY_SUFFIX .a)
ENDIF( SUPER_SHARED_LIBS )

set( VIDEO_SUPPORT # turn ON ffmpeg encoder
                   -DVTK_USE_FFMPEG_ENCODER:BOOL=ON
                   # force VTK to use NEW headers
                   -DVTK_FFMPEG_CACHED_INCLUDE:PATH=${CMAKE_BINARY_DIR}/${proj}
                   -DVTK_FFMPEG_HAS_OLD_HEADER:BOOL=FALSE
                   # set useful variables
                   -DFFMPEG_INCLUDE_DIR:PATH=${CMAKE_BINARY_DIR}/${proj}
                   -DFFMPEG_avcodec_LIBRARY:FILEPATH=${CMAKE_BINARY_DIR}/${proj}/libavcodec/libavcodec${LIBRARY_SUFFIX}         
                   -DFFMPEG_avformat_LIBRARY:FILEPATH=${CMAKE_BINARY_DIR}/${proj}/libavformat/libavformat${LIBRARY_SUFFIX}
                   -DFFMPEG_avutil_LIBRARY:FILEPATH=${CMAKE_BINARY_DIR}/${proj}/libavutil/libavutil${LIBRARY_SUFFIX}
                   -DFFMPEG_swscale_LIBRARY:FILEPATH=${CMAKE_BINARY_DIR}/${proj}/libswscale/libswscale${LIBRARY_SUFFIX}
)
