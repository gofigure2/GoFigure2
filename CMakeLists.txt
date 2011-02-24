PROJECT( GOFIGURE2 )

# CMake >= 2.8.2 MUST be installed (which is requiered by ITK)
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.2 )

IF( COMMAND CMAKE_POLICY )
  CMAKE_POLICY( VERSION 2.6 )
  CMAKE_POLICY( SET CMP0005 OLD )
  CMAKE_POLICY( SET CMP0003 NEW )
ENDIF(COMMAND CMAKE_POLICY)

# When making one release: update GOFIGURE2_*_VERSION,
# GOFIGURE2_WC_REVISION and the date of the release
SET( GOFIGURE2_MAJOR_VERSION "0" )
SET( GOFIGURE2_MINOR_VERSION "8" )
SET( GOFIGURE2_WC_REVISION "1" )

SET( DATE "02/14/2011" )

SET( GOFIGURE2_VERSION
  "${GOFIGURE2_MAJOR_VERSION}.${GOFIGURE2_MINOR_VERSION}.${GOFIGURE2_WC_REVISION}" )

SET( GOFIGURE2_LIB_VERSION
  "${GOFIGURE2_MAJOR_VERSION}.${GOFIGURE2_MINOR_VERSION}" )

MESSAGE( STATUS "GoFigure2 version ${GOFIGURE2_VERSION}" )

# On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security
# warnings. Copied from ITK CMakeLists.
IF( WIN32 )
  IF( NOT BORLAND )
    IF( NOT CYGWIN )
      IF( NOT MINGW )
          ADD_DEFINITIONS(
            -D_CRT_FAR_MAPPINGS_NO_DEPRECATE
            -D_CRT_IS_WCTYPE_NO_DEPRECATE
            -D_CRT_MANAGED_FP_NO_DEPRECATE
            -D_CRT_NONSTDC_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE_GLOBALS
            -D_CRT_SETERRORMODE_BEEP_SLEEP_NO_DEPRECATE
            -D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
            -D_CRT_VCCLRIT_NO_DEPRECATE
            -D_SCL_SECURE_NO_DEPRECATE
            )
      ENDIF( NOT MINGW )
    ENDIF( NOT CYGWIN )
  ENDIF( NOT BORLAND )
ENDIF( WIN32 )

#-----------------------------------------------------------------------
# Export and Install configuration
#
# GOFIGURE2_INSTALL_BUNDLE_DIR       - bundle dir
# GOFIGURE2_INSTALL_BIN_DIR          - binary dir (executables)
# GOFIGURE2_INSTALL_LIB_DIR          - library dir (libs)
# GOFIGURE2_INSTALL_DOC_DIR          - documentation dir
# GOFIGURE2_INSTALL_LICENSE_DIR      - licenses dir
#-----------------------------------------------------------------------
IF( NOT GOFIGURE2_INSTALL_BUNDLE_DIR )
  SET( GOFIGURE2_INSTALL_BUNDLE_DIR "." )
ENDIF( NOT GOFIGURE2_INSTALL_BUNDLE_DIR )

IF( NOT GOFIGURE2_INSTALL_BIN_DIR )
  SET( GOFIGURE2_INSTALL_BIN_DIR "bin" )
ENDIF( NOT GOFIGURE2_INSTALL_BIN_DIR )

IF( NOT GOFIGURE2_INSTALL_LIB_DIR )
  SET( GOFIGURE2_INSTALL_LIB_DIR "lib/gofigure2" )
ENDIF( NOT GOFIGURE2_INSTALL_LIB_DIR )

IF( NOT GOFIGURE2_INSTALL_DOC_DIR )
  SET( GOFIGURE2_INSTALL_DOC_DIR "share/doc/gofigure2" )
ENDIF( NOT GOFIGURE2_INSTALL_DOC_DIR )

IF( NOT GOFIGURE2_INSTALL_LICENSE_DIR )
  SET( GOFIGURE2_INSTALL_LICENSE_DIR "share/doc/gofigure2/Licenses" )
ENDIF( NOT GOFIGURE2_INSTALL_LICENSE_DIR )

#---------------------------------------------------------------------------
# Define CMAKE_whatever_OUTPUT_DIRECTORY
#---------------------------------------------------------------------------
# all target executables will be
# in ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${GOFIGURE2_BINARY_DIR}/bin
)

# all target static libraries will be in ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${GOFIGURE2_BINARY_DIR}/lib
)

# all shared libraries will be in ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
# Note that modules (plugins) are considered as shared libraries
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY
#  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins
   ${GOFIGURE2_BINARY_DIR}/lib
)
SET( GOFIGURE2_LIBRARY_DIRS ${GOFIGURE2_BINARY_DIR}/lib )

#---------------------------------------------------------------------------
# Define TESTING_DATA_PATH
#---------------------------------------------------------------------------
SET( TESTING_DATA_PATH
  ${GOFIGURE2_SOURCE_DIR}/Testing/Data
  CACHE PATH "Directory which contains data for testing."
)

#---------------------------------------------------------------------------
# Handle Documentation
#---------------------------------------------------------------------------
OPTION( BUILD_DOCUMENTATION "Build Doxygen Documentation" OFF )

#---------------------------------------------------------------------------
IF( BUILD_DOCUMENTATION )
  FIND_PACKAGE( Doxygen )
  IF( DOXYGEN_FOUND )
    IF( DOXYGEN_DOT_FOUND )

      OPTION( LINK_EXTERNAL_DOC
        "Should the doc be linked with external sources (VTK, ITK...)?"
        OFF
      )

      ADD_SUBDIRECTORY( Documentation )

    ELSE( DOXYGEN_DOT_FOUND )
      MESSAGE( SEND_ERROR
        "Warning: Dot (from GraphViz) not found - Documentation will not ce generated"
      )
    ENDIF( DOXYGEN_DOT_FOUND )
  ELSE( DOXYGEN_FOUND )
    MESSAGE( SEND_ERROR
      "Warning: Doxygen not found - Documentation will not be created"
    )
  ENDIF( DOXYGEN_FOUND )
ENDIF( BUILD_DOCUMENTATION )

#---------------------------------------------------------------------------
# Main GoFigure Settings
#---------------------------------------------------------------------------

SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${GOFIGURE2_SOURCE_DIR}/CMake/"
)

IF( UNIX )
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF( UNIX )

# Everything depends on VTK first
# that way we can get Qt info from VTK
#### VTK ####
FIND_PACKAGE( VTK REQUIRED )
IF( VTK_FOUND )

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

  #----------------------------------------------------------
  # Then find ITK
  #### ITK ####
  FIND_PACKAGE( ITK REQUIRED )
  IF( ITK_FOUND )
    INCLUDE( ${ITK_USE_FILE} )

    IF( ( ${ITK_VERSION_MAJOR} LESS 3 ) OR
        ( ( ${ITK_VERSION_MAJOR} EQUAL 3 ) AND
          ( ${ITK_VERSION_MINOR} LESS 18 ) ) )
      MESSAGE( SEND_ERROR
        "GoFigure2 requires ITK 3.18 or newer version (your version of ITK is ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_PATCH})" )
    ENDIF( ( ${ITK_VERSION_MAJOR} LESS 3 ) OR
        ( ( ${ITK_VERSION_MAJOR} EQUAL 3 ) AND
          ( ${ITK_VERSION_MINOR} LESS 18 ) ) )


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

  ENDIF( ITK_FOUND )
  #----------------------------------------------------------

  #----------------------------------------------------------
  # Then find Boost. Note that we only need headers
  #### BOOST ####
  FIND_PACKAGE( Boost REQUIRED )
  IF( Boost_FOUND )
    INCLUDE_DIRECTORIES( BEFORE ${Boost_INCLUDE_DIRS} )
  ENDIF( Boost_FOUND )

  # Try to find Qt.
  # Note that some command line tools will get compiled even without Qt
  #### Qt ####
  FIND_PACKAGE( Qt4 4.5 REQUIRED QtCore QtGui )
  IF( QT4_FOUND )
    IF( QT_USE_FILE )
      SET( QT_USE_QT3SUPPORT FALSE )
      SET( QT_USE_QTCORE TRUE )
      SET( QT_USE_QTGUI TRUE )
      INCLUDE( ${QT_USE_FILE} )
    ELSE( QT_USE_FILE )
      SET( QT_LIBRARIES ${QT_QT_LIBRARY} )
    ENDIF( QT_USE_FILE )

    SET( QT_PLUGIN_LIBS ${QT_LIBRARIES} )

    ADD_DEFINITIONS( ${QT_DEFINITIONS} )
    #
    # ADD_DEFINITIONS( -DQT_GUI_LIBS )
    # ADD_DEFINITIONS( -DQT_CORE_LIB )

    SET( GOFIGURE2_EXTERNAL_SOURCE_DIR
      ${GOFIGURE2_SOURCE_DIR}/Code/ExternalCode )
    SET( GOFIGURE2_EXTERNAL_BINARY_DIR
      ${GOFIGURE2_BINARY_DIR}/Code/ExternalCode )

    SET( MEGAVTK2_SOURCE_DIR ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/MegaVTK )
    SET( MEGAVTK2_BINARY_DIR ${GOFIGURE2_EXTERNAL_BINARY_DIR}/MegaVTK )

    LINK_DIRECTORIES(
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
      ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
      )


    INCLUDE_DIRECTORIES( BEFORE
      ${QT_INCLUDES}
      ${GOFIGURE2_SOURCE_DIR}/Code/
      ${GOFIGURE2_BINARY_DIR}/Code/
      ${GOFIGURE2_SOURCE_DIR}/Code/IO/
      ${GOFIGURE2_BINARY_DIR}/Code/IO/
      ${GOFIGURE2_SOURCE_DIR}/Code/Filters/
      ${GOFIGURE2_BINARY_DIR}/Code/Filters/
      ${GOFIGURE2_SOURCE_DIR}/Code/GUI/
      ${GOFIGURE2_BINARY_DIR}/Code/GUI/
      ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/
      ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/
      ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/Resources/
      ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/TraceContainers/
      ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/TraceContainers/
      ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/Video/
      ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/Video/
      ${GOFIGURE2_SOURCE_DIR}/Interfaces/
      ${GOFIGURE2_BINARY_DIR}/Interfaces/
      ${GOFIGURE2_SOURCE_DIR}/Code/GUI/src/
      ${GOFIGURE2_BINARY_DIR}/Code/GUI/src/
      ${MEGAVTK2_SOURCE_DIR}
      ${MEGAVTK2_BINARY_DIR}
      ${MEGAVTK2_SOURCE_DIR}/vtkItk
      ${MEGAVTK2_BINARY_DIR}/vtkItk
      ${MEGAVTK2_SOURCE_DIR}/vtkRenderingAddOn
      ${MEGAVTK2_BINARY_DIR}/vtkRenderingAddOn
      ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/vtkLSM
      ${GOFIGURE2_EXTERNAL_BINARY_DIR}/vtkLSM
      ${GOFIGURE2_EXTERNAL_BINARY_DIR}/itkQt
      ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/itkQt
      ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/PoissonReconstruction
      ${GOFIGURE2_EXTERNAL_BINARY_DIR}/PoissonReconstruction
      ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/ctk
      ${GOFIGURE2_EXTERNAL_BINARY_DIR}/ctk
      ${GOFIGURE2_SOURCE_DIR}/Main
      ${GOFIGURE2_BINARY_DIR}/Main
    )

    # Try to extract Qt variable from VTK
    SET( QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "" )
    SET( QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "" )
    SET( QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "" )

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

    #---------------------------------------------------------------------
    #---------------------------------------------------------------------

    SET( QGoResourceFile ${GOFIGURE2_SOURCE_DIR}/Resources/axes.qrc )

    SET( QGoGUIINTERFACES_HDRS
      ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoPlugin.h
      ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoImageFilterPluginBase.h
    )
    SET( QGoGUIINTERFACES_SRC
      ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoPluginHelper.cxx
      ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoPlugin.cxx
      ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoImageFilterPluginBase.cxx
    )

    ADD_SUBDIRECTORY( Code )
    ADD_SUBDIRECTORY( Main )

    #-----------------------------------------------------------------------
    # Plugins
    #-----------------------------------------------------------------------
    IF( VTK_BUILD_SHARED_LIBS )
      OPTION( BUILD_PLUGINS  "Build GoFigure Plugins" OFF )
    ELSE( VTK_BUILD_SHARED_LIBS )
      SET( BUILD_PLUGINS OFF )
    ENDIF( VTK_BUILD_SHARED_LIBS )

    IF( BUILD_PLUGINS )
      ADD_SUBDIRECTORY( Plugins )
    ENDIF( BUILD_PLUGINS )

    #-----------------------------------------------------------------------
    # Examples and tests
    #-----------------------------------------------------------------------
    OPTION( BUILD_EXAMPLES "Build Examples" OFF )

    IF( BUILD_TESTING )
      SET( BUILDNAME "${BUILDNAME}"
             CACHE STRING "Name of build on the dashboard"
            )

      MARK_AS_ADVANCED( BUILDNAME )

      OPTION( TESTING_USING_SIKULI "Use Sikuli for testing" OFF )
      MARK_AS_ADVANCED( TESTING_USING_SIKULI )

      CONFIGURE_FILE( CMake/CTestCustom.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake
        @ONLY
        )
      INCLUDE ( CTest )

      IF( TESTING_USING_SIKULI )
        FIND_PACKAGE( Sikuli )
        ADD_SUBDIRECTORY( Testing )
      ENDIF( TESTING_USING_SIKULI )

    ENDIF( BUILD_TESTING )

    IF( BUILD_EXAMPLES )
      ADD_SUBDIRECTORY( Examples )
      ADD_SUBDIRECTORY( KWStyle )
      OPTION( BUILD_TESTING "Test the project" ON )
    ELSE( BUILD_EXAMPLES )
      SET( BUILD_TESTING OFF )
    ENDIF( BUILD_EXAMPLES )

    INCLUDE( GoFigure2IncludeDirectories.cmake )

    CONFIGURE_FILE(
      "${CMAKE_CURRENT_SOURCE_DIR}/UseGoFigure2.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/UseGoFigure2.cmake"
      @ONLY
      )

    #-----------------------------------------------------------------------
    # PACKAGE With CPack
    #-----------------------------------------------------------------------
    # add the uninstall support
    CONFIGURE_FILE(
      "${CMAKE_CURRENT_SOURCE_DIR}/GOFIGURE2Uninstall.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/GOFIGURE2Uninstall.cmake"
      @ONLY
      )

    ADD_CUSTOM_TARGET( uninstall
      "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/GOFIGURE2Uninstall.cmake"
      )

    INCLUDE( GOFIGURE2CPack.cmake )
    
  ENDIF( QT4_FOUND )

ENDIF( VTK_FOUND )
