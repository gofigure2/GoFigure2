PROJECT( GOFIGURE2 )

# CMake >= 2.6 MUST be installed
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

IF( COMMAND CMAKE_POLICY )
  CMAKE_POLICY( VERSION 2.4 )
  CMAKE_POLICY( SET CMP0005 OLD )
  CMAKE_POLICY( SET CMP0003 NEW )
ENDIF(COMMAND CMAKE_POLICY)

#-------------------------------------------------------------------------
# Subversion
IF( EXISTS ${CMAKE_SOURCE_DIR}/.svn/ )
  FIND_PACKAGE( Subversion )

  IF( Subversion_FOUND )
    Subversion_WC_INFO( ${GOFIGURE2_SOURCE_DIR} GOFIGURE2 )
  ELSE( Subversion_FOUND )
    SET( GOFIGURE2_WC_REVISION 0 )
  ENDIF( Subversion_FOUND )
ELSE( EXISTS ${CMAKE_SOURCE_DIR}/.svn/ )
  SET( GOFIGURE2_WC_REVISION 0 )
ENDIF( EXISTS ${CMAKE_SOURCE_DIR}/.svn/ )
#-------------------------------------------------------------------------

# When making one release: update GOFIGURE2_*_VERSION, 
# GOFIGURE2_WC_REVISION and the date of the release
SET( GOFIGURE2_MAJOR_VERSION "0" )
SET( GOFIGURE2_MINOR_VERSION "4" )
# SET( GOFIGURE2_WC_REVISION 1 )

SET( DATE "04/13/2010" )

SET( GOFIGURE2_VERSION
  "${GOFIGURE2_MAJOR_VERSION}.${GOFIGURE2_MINOR_VERSION}.${GOFIGURE2_WC_REVISION}" )

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

      OPTION( LINK_EXTERNAL_DOC "Should the documentation be linked with external sources such as VTK?" OFF )

      ADD_SUBDIRECTORY( Documentation )

    ELSE( DOXYGEN_DOT_FOUND )
      MESSAGE( STATUS "Warning: Dot (from GraphViz) not found - Documentation
will not ce generated" )
    ENDIF( DOXYGEN_DOT_FOUND )
  ELSE( DOXYGEN_FOUND AND )
    MESSAGE( STATUS "Warning: Doxygen not found - Documentation will not be created")
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
FIND_PACKAGE( VTK )
IF( VTK_FOUND )
  INCLUDE( ${VTK_USE_FILE} )

  IF( ${VTK_USE_MYSQL} MATCHES "OFF" )
    MESSAGE( SEND_ERROR "VTK must be compiled with MySQL support" )
    # TODO (arnaudgelas)
    # Here add a definition to be able to compile and use GoFigure
    # without any database support
    # ADD_DEFINITION( -DNODBSUPPORT )
  ELSE( ${VTK_USE_MYSQL} MATCHES "OFF" )
    FIND_PACKAGE( MySQL REQUIRED )
    SET( MYSQL_LIBRARY ${MYSQL_LIBRARY} CACHE FILEPATH "" )
    SET( MYSQL_EXTRA_LIBRARIES ${MYSQL_EXTRA_LIBRARIES} CACHE FILEPATH "" )
  ENDIF( ${VTK_USE_MYSQL} MATCHES "OFF" )

  IF( VTK_BUILD_SHARED_LIBS )
    # ADD_DEFINITIONS( -DQT_SHARED )
    SET( LIBS_STYLE "SHARED" )    
    SET( BUILD_SHARED_LIBS "TRUE" )
  ELSE( VTK_BUILD_SHARED_LIBS )
    SET( LIBS_STYLE "STATIC" )
    SET( BUILD_SHARED_LIBS "FALSE" )
  ENDIF( VTK_BUILD_SHARED_LIBS )

#  IF( APPLE )
#    SET( LIBS_STYLE "SHARED" )
#  ENDIF( APPLE )

  # Then find ITK
  FIND_PACKAGE( ITK )
  IF( ITK_FOUND )
    INCLUDE( ${ITK_USE_FILE} )
  ENDIF( ITK_FOUND )

  # Then find oost
  FIND_PACKAGE( Boost )
  IF( Boost_FOUND )
    INCLUDE_DIRECTORIES( BEFORE ${Boost_INCLUDE_DIRS} )
  ENDIF( Boost_FOUND )

  # Try to extract Qt variable from VTK
  SET(QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "")
  SET(QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "")
  SET(QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "")

  # Try to find Qt.
  # Note that some command line tools will get compiled even without Qt
  FIND_PACKAGE( Qt4 )
  IF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )
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
    # ADD_DEFINITIONS( -DQT_GUI_LIBS )
    # ADD_DEFINITIONS( -DQT_CORE_LIB )
    
    SET( GOFIGURE2_EXTERNAL_SOURCE_DIR
      ${GOFIGURE2_SOURCE_DIR}/Code/ExternalCode )
    SET( GOFIGURE2_EXTERNAL_BINARY_DIR
      ${GOFIGURE2_BINARY_DIR}/Code/ExternalCode )
    SET( GOFIGURE2_BOOST_MULTI_INDEX_DIR
      ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/Boost/multi_index )

    SET( MEGAVTK2_SOURCE_DIR ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/MegaVTK )
    SET( MEGAVTK2_BINARY_DIR ${GOFIGURE2_EXTERNAL_BINARY_DIR}/MegaVTK )

    # Set specific flag for shared Qt libraries on windows
    #IF( WIN32 )
    #  IF( NOT CYGWIN )
    #    IF( NOT MINGW )
    #      IF( VTK_BUILD_SHARED_LIBS )
    #        ADD_DEFINITIONS( -DvtkRenderingAddOn2_EXPORTS )
    #        ADD_DEFINITIONS( -DGoFigureLibs_EXPORTS )
    #        ADD_DEFINITIONS( -DQGOLIBS_EXPORTS )
    #        
    #        # SET( EXECUTABLE_OUTPUT_PATH ${GOFIGURE2_BINARY_DIR}/bin )
    #        # SET( LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH} )
    #      ELSE( VTK_BUILD_SHARED_LIBS )
    #        REMOVE_DEFINITIONS( -DvtkRenderingAddOn2_EXPORTS )
    #        REMOVE_DEFINITIONS( -DGoFigureLibs_EXPORTS )
    #        REMOVE_DEFINITIONS( -DQGOLIBS_EXPORTS )
    #        # SET( EXECUTABLE_OUTPUT_PATH ${GOFIGURE2_BINARY_DIR}/bin )
    #        # SET( LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH} )
    #      ENDIF( VTK_BUILD_SHARED_LIBS )
    #    ENDIF( NOT MINGW )
    #  ENDIF( NOT CYGWIN )
    #ENDIF( WIN32 )

    LINK_DIRECTORIES( 
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
      ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
      )#${LIBRARY_OUTPUT_PATH} )


    INCLUDE_DIRECTORIES( BEFORE
      ${QT_INCLUDES}
      ${GOFIGURE2_SOURCE_DIR}/Code/
      ${GOFIGURE2_BINARY_DIR}/Code/
      ${GOFIGURE2_SOURCE_DIR}/Code/IO/
      ${GOFIGURE2_BINARY_DIR}/Code/IO/
      ${GOFIGURE2_SOURCE_DIR}/Code/GUI/
      ${GOFIGURE2_BINARY_DIR}/Code/GUI/
      ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/
      ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/
      ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/Resources/
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
    )

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
      ADD_DEFINITIONS(-DENABLEFFMPEG)
    ELSE( ENABLE_VIDEO_RECORD_FFMPEG )
      REMOVE_DEFINITIONS(-DENABLEFFMPEG)
    ENDIF( ENABLE_VIDEO_RECORD_FFMPEG )

    # AVI: for Windows only
    IF( VTK_USE_VIDEO_FOR_WINDOWS )
      OPTION (ENABLE_VIDEO_RECORD_AVI "VTK must be built with VTK_USE_AVI_ENCODER" ON)
    ENDIF( VTK_USE_VIDEO_FOR_WINDOWS )

    IF( ENABLE_VIDEO_RECORD_AVI )
      ADD_DEFINITIONS(-DENABLEAVI)
    ELSE( ENABLE_VIDEO_RECORD_AVI )
      REMOVE_DEFINITIONS(-DENABLEAVI)
    ENDIF( ENABLE_VIDEO_RECORD_AVI )

    #---------------------------------------------------------------------
    #---------------------------------------------------------------------

    IF( BUILD_EXAMPLES )
      INCLUDE( CTest )
    ENDIF( BUILD_EXAMPLES )

    IF( BUILD_TESTING )
      SET( BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard" )
      MARK_AS_ADVANCED( BUILDNAME )
    ENDIF( BUILD_TESTING )

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

  OPTION( BUILD_EXAMPLES "Build Examples" OFF )

  IF( VTK_BUILD_SHARED_LIBS )
    OPTION( BUILD_PLUGINS  "Build GoFigure Plugins" OFF )
  ELSE( VTK_BUILD_SHARED_LIBS )
    SET( BUILD_PLUGINS OFF )
  ENDIF( VTK_BUILD_SHARED_LIBS )

#---------------------------------------------------------------------------
# Examples and tests
#---------------------------------------------------------------------------

  IF( BUILD_EXAMPLES )

    ADD_SUBDIRECTORY( Examples )
    ADD_SUBDIRECTORY( KWStyle )

  ENDIF( BUILD_EXAMPLES )

  IF( BUILD_PLUGINS )
    ADD_SUBDIRECTORY( Plugins )
  ENDIF( BUILD_PLUGINS )

#---------------------------------------------------------------------------
# PACKAGE With CPack
#---------------------------------------------------------------------------
  # add the uninstall support
  CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/GOFIGURE2Uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/GOFIGURE2Uninstall.cmake"
	@ONLY
    )

  ADD_CUSTOM_TARGET(uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/GOFIGURE2Uninstall.cmake"
    )

  IF( ( ${CMAKE_MAJOR_VERSION} EQUAL 2 )  AND ( ${CMAKE_MINOR_VERSION} GREATER 7 ) )
    INCLUDE( GOFIGURE2CPack.cmake )
  ENDIF( ( ${CMAKE_MAJOR_VERSION} EQUAL 2 )  AND ( ${CMAKE_MINOR_VERSION} GREATER 7 ) )

  ENDIF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )

ENDIF( VTK_FOUND )
