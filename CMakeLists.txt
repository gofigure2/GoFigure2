PROJECT( GOFIGURE2 )

# CMake >= 2.6 MUST be installed
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

IF( COMMAND CMAKE_POLICY )
  CMAKE_POLICY( VERSION 2.4 )
  CMAKE_POLICY( SET CMP0005 OLD )
  CMAKE_POLICY( SET CMP0003 NEW )
ENDIF(COMMAND CMAKE_POLICY)

SET( GOFIGURE2_MAJOR_VERSION "0" )
SET( GOFIGURE2_MINOR_VERSION "5" )

# Subversion
FIND_PACKAGE( Subversion )

IF( Subversion_FOUND )
  Subversion_WC_INFO( ${GOFIGURE2_SOURCE_DIR} GOFIGURE2 )
ELSE( Subversion_FOUND )
  SET( GOFIGURE2_WC_REVISION "0" )
ENDIF( Subversion_FOUND )

SET( GOFIGURE2_VERSION
"${GOFIGURE2_MAJOR_VERSION}.${GOFIGURE2_MINOR_VERSION}.${GOFIGURE2_WC_REVISION}" )

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
# Define EXECUTABLE_OUTPUT_PATH and LIBRARY_OUTPUT_PATH
#---------------------------------------------------------------------------
SET(EXECUTABLE_OUTPUT_PATH
 ${GOFIGURE2_BINARY_DIR}/bin
 CACHE PATH "Single output directory for building all executables."
)

SET(LIBRARY_OUTPUT_PATH
 ${GOFIGURE2_BINARY_DIR}/lib
 CACHE PATH "Single output directory for building all libraries."
)

SET( TESTING_DATA_PATH
  ${GOFIGURE2_SOURCE_DIR}/Testing/Data
)

#---------------------------------------------------------------------------
# Handle Documentation
#---------------------------------------------------------------------------
OPTION( BUILD_DOCUMENTATION "Build Doxygen Documentation" OFF )
OPTION( BUILD_PACKAGE "Software packaging" OFF )

#---------------------------------------------------------------------------
# Temporary option to be able to chosse between experimental spline nodes or
# contour points
#---------------------------------------------------------------------------
OPTION( BUILD_TEMPARNAUD "Use spline nodes instead of contour points" OFF )
MARK_AS_ADVANCED( FORCE BUILD_TEMPARNAUD )

IF( BUILD_TEMPARNAUD )
  ADD_DEFINITIONS( -DTEMPARNAUD )
ELSE( BUILD_TEMPARNAUD )
  REMOVE_DEFINITIONS( -DTEMPARNAUD )
ENDIF( BUILD_TEMPARNAUD )

#---------------------------------------------------------------------------
IF( BUILD_DOCUMENTATION )
  FIND_PACKAGE( Doxygen )
  IF( DOXYGEN_FOUND )
    IF( DOXYGEN_DOT_FOUND )
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

# Everything depends on VTK first
# that way we can get Qt info from VTK
FIND_PACKAGE( VTK )
IF( VTK_FOUND )
  INCLUDE( ${VTK_USE_FILE} )

  IF( ${VTK_USE_MYSQL} )
    MESSAGE( SEND_ERROR "VTK must be compiled with MySQL support" )
    # TODO (arnaudgelas)
    # Here add a definition to be able to compile and use GoFigure
    # without any database support
    # ADD_DEFINITION( -DNODBSUPPORT )
  ENDIF( ${VTK_USE_MYSQL} )

  IF( VTK_BUILD_SHARED_LIBS )
    SET( LIBS_STYLE "SHARED" )
  ELSE( VTK_BUILD_SHARED_LIBS )
    SET( LIBS_STYLE "STATIC" )
  ENDIF( VTK_BUILD_SHARED_LIBS )

  IF( APPLE )
    SET( LIBS_STYLE "SHARED" )
  ENDIF( APPLE )

  # Then find ITK
  FIND_PACKAGE( ITK )
  IF( ITK_FOUND )
    INCLUDE( ${ITK_USE_FILE} )

    IF( ITK_MINOR_VERSION GREATER 14 )
      ADD_DEFINITIONS( -DITKLEVELSETCODE )
    ELSE( ITK_MINOR_VERSION GREATER 14 )
      REMOVE_DEFINITIONS( -DITKLEVELSETCODE )
    ENDIF( ITK_MINOR_VERSION GREATER 14 )
  ENDIF( ITK_FOUND )

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
      INCLUDE( ${QT_USE_FILE} )
    ELSE( QT_USE_FILE )
      SET( QT_LIBRARIES ${QT_QT_LIBRARY} )
    ENDIF( QT_USE_FILE )

    SET( GOFIGURE2_EXTERNAL_SOURCE_DIR
      ${GOFIGURE2_SOURCE_DIR}/Code/ExternalCode )
    SET( GOFIGURE2_EXTERNAL_BINARY_DIR
      ${GOFIGURE2_BINARY_DIR}/Code/ExternalCode )
    SET( MEGAVTK2_SOURCE_DIR ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/MegaVTK )
    SET( MEGAVTK2_BINARY_DIR ${GOFIGURE2_EXTERNAL_BINARY_DIR}/MegaVTK )

    # Set specific flag for shared Qt libraries on windows
    IF( WIN32 )
      IF( NOT CYGWIN )
        IF( NOT MINGW )
          IF( VTK_BUILD_SHARED_LIBS )
            ADD_DEFINITIONS( -DQMEGAVTKADDON2_EXPORTS )
            ADD_DEFINITIONS( -DvtkRenderingAddOn2_EXPORTS )
            SET( LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH} )
          ELSE( VTK_BUILD_SHARED_LIBS )
            REMOVE_DEFINITIONS( -DQMEGAVTKADDON2_EXPORTS )
            REMOVE_DEFINITIONS( -DvtkRenderingAddOn2_EXPORTS )
          ENDIF( VTK_BUILD_SHARED_LIBS )
        ENDIF( NOT MINGW )
      ENDIF( NOT CYGWIN )
    ENDIF( WIN32 )

    LINK_DIRECTORIES( ${LIBRARY_OUTPUT_PATH} )

    INCLUDE_DIRECTORIES( BEFORE
      ${QT_INCLUDES}
      ${GOFIGURE2_SOURCE_DIR}/Code/Filters/
      ${GOFIGURE2_BINARY_DIR}/Code/Filters/
      ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Preprocessing/
      ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Segmentation/
      ${GOFIGURE2_BINARY_DIR}/Code/Filters/Preprocessing/
      ${GOFIGURE2_BINARY_DIR}/Code/Filters/Segmentation/
      ${GOFIGURE2_SOURCE_DIR}/Code/IO/
      ${GOFIGURE2_BINARY_DIR}/Code/IO/
      ${GOFIGURE2_SOURCE_DIR}/Code/GUI/
      ${GOFIGURE2_BINARY_DIR}/Code/GUI/
      ${GOFIGURE2_SOURCE_DIR}/Code/GUI/qt
      ${GOFIGURE2_BINARY_DIR}/Code/GUI/qt
      ${GOFIGURE2_SOURCE_DIR}/Code/GUI/Vizualisation
      ${GOFIGURE2_BINARY_DIR}/Code/GUI/Vizualisation
      ${MEGAVTK2_SOURCE_DIR}
      ${MEGAVTK2_BINARY_DIR}
      ${MEGAVTK2_SOURCE_DIR}/vtkItk
      ${MEGAVTK2_BINARY_DIR}/vtkItk
      ${MEGAVTK2_SOURCE_DIR}/vtkRenderingAddOn
      ${MEGAVTK2_BINARY_DIR}/vtkRenderingAddOn
      ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/vtkLSM
      ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/itkQt )

    SUBDIRS( Code )

    INCLUDE( CTest )

    SUBDIRS( Examples
           KWStyle
    )

#---------------------------------------------------------------------------
# PACKAGE With CPack
#---------------------------------------------------------------------------
    IF( BUILD_PACKAGE )
      INCLUDE( GOFIGURE2CPack.cmake )
    ENDIF( BUILD_PACKAGE )

  ENDIF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )

ENDIF( VTK_FOUND )

