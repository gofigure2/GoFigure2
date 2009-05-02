PROJECT( GOFIGURE2 )

SET( GOFIGURE2_MAJOR_VERSION "0" )
SET( GOFIGURE2_MINOR_VERSION "5" )

SET( GOFIGURE2_VERSION
"${GOFIGURE2_MAJOR_VERSION}.${GOFIGURE2_MINOR_VERSION}" )

CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

IF( COMMAND CMAKE_POLICY )
  CMAKE_POLICY( VERSION 2.4 )
  CMAKE_POLICY( SET CMP0005 OLD )
  CMAKE_POLICY( SET CMP0003 NEW )
ENDIF(COMMAND CMAKE_POLICY)

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

#------------------------------------------------------------------------------
# Define EXECUTABLE_OUTPUT_PATH and LIBRARY_OUTPUT_PATH
#------------------------------------------------------------------------------
SET(EXECUTABLE_OUTPUT_PATH
 ${GOFIGURE2_BINARY_DIR}/bin
 CACHE PATH "Single output directory for building all executables."
)
SET(LIBRARY_OUTPUT_PATH
 ${GOFIGURE2_BINARY_DIR}/lib
 CACHE PATH "Single output directory for building all libraries."
)

#------------------------------------------------------------------------------
# Handle Documentation
#------------------------------------------------------------------------------
OPTION( BUILD_DOCUMENTATION "Build Doxygen Documentation" OFF )
OPTION( BUILD_PACKAGE "Software packaging" OFF )

IF( BUILD_DOCUMENTATION )
  FIND_PACKAGE( Doxygen )
  IF( DOXYGEN_FOUND )
    CONFIGURE_FILE (
      ${GOFIGURE2_SOURCE_DIR}/Doxyfile.txt.in
      ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
    )

    ADD_CUSTOM_TARGET(doc ALL
      ${DOXYGEN_EXECUTABLE} ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
      DEPENDS ${GOFIGURE2_BINARY_DIR}/Doxyfile.txt
    )
  ELSE( DOXYGEN_FOUND )
    MESSAGE( STATUS "Warning: Doxygen not found - Documentation will not be created")
  ENDIF( DOXYGEN_FOUND )
ENDIF( BUILD_DOCUMENTATION )

#------------------------------------------------------------------------------
# Main GoFigure Settings
#------------------------------------------------------------------------------

# Everything depends on VTK first
# that way we can get Qt info from VTK
FIND_PACKAGE( VTK )
IF( VTK_FOUND )
  INCLUDE( ${VTK_USE_FILE} )

  IF( VTK_BUILD_SHARED_LIBS )
    SET(LIBS_STYLE "SHARED")
  ELSE( VTK_BUILD_SHARED_LIBS )
    SET(LIBS_STYLE "STATIC")
  ENDIF( VTK_BUILD_SHARED_LIBS )

  # Then find ITK
  FIND_PACKAGE( ITK )
  IF( ITK_FOUND )
    INCLUDE( ${ITK_USE_FILE} )
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
      SET( QT_USE_QT3SUPPORT TRUE )
      INCLUDE( ${QT_USE_FILE} )
    ELSE( QT_USE_FILE )
      SET( QT_LIBRARIES ${QT_QT_LIBRARY} )
    ENDIF( QT_USE_FILE )

    SET(GOFIGURE2_EXTERNAL_SOURCE_DIR
      ${GOFIGURE2_SOURCE_DIR}/Code/ExternalCode )
    SET(GOFIGURE2_EXTERNAL_BINARY_DIR
      ${GOFIGURE2_BINARY_DIR}/Code/ExternalCode )
    SET( MEGAVTK2_SOURCE_DIR ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/MegaVTK )
    SET( MEGAVTK2_BINARY_DIR ${GOFIGURE2_EXTERNAL_BINARY_DIR}/MegaVTK )

    # Set specific flag for shared Qt libraries on windows
    IF(WIN32)
      IF(NOT CYGWIN)
        IF(NOT MINGW)
          IF(VTK_BUILD_SHARED_LIBS)
            ADD_DEFINITIONS( -DQMEGAVTKADDON2_EXPORTS )
            ADD_DEFINITIONS( -DvtkRenderingAddOn2_EXPORTS )
            SET( LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH} )
          ENDIF(VTK_BUILD_SHARED_LIBS)
        ENDIF(NOT MINGW)
      ENDIF(NOT CYGWIN)
    ENDIF(WIN32)

    LINK_DIRECTORIES( ${LIBRARY_OUTPUT_PATH} )

    INCLUDE_DIRECTORIES( BEFORE
      ${QT_INCLUDES}
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

    IF( BUILD_PACKAGE )
      INCLUDE( InstallRequiredSystemLibraries )
      SET( CPACK_PACKAGE_DESCRIPTION_SUMMARY "GoFigure2" )
      SET( CPACK_PACKAGE_VENDOR "Megason Lab, Systems Biology, HMS" )
#     SET( CPACK_PACKAGE_DESCRIPTION_FILE
#       "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
#     SET( CPACK_RESOURCE_FILE_LICENSE
#       "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
      SET( CPACK_PACKAGE_VERSION_MAJOR ${GOFIGURE2_MAJOR_VERSION} )
      SET( CPACK_PACKAGE_VERSION_MINOR ${GOFIGURE2_MINOR_VERSION} )
      SET( CPACK_PACKAGE_VERSION_PATCH "0" )
      SET( CPACK_PACKAGE_INSTALL_DIRECTORY
        "GoFigure2-${GOFIGURE2_MAJOR_VERSION}.${GOFIGURE2_MINOR_VERSION}" )

      IF( WIN32 AND NOT UNIX )
        # There is a bug in NSI that does not handle full unix paths properly.
        # Make sure there is at least one set of four (4) backslashes.
#       SET( CPACK_PACKAGE_ICON
#         "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp" )
        SET( CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\gofigure.exe")
        SET( CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}
          GoFigure2" )
        SET( CPACK_NSIS_HELP_LINK
          "http:\\\\\\\\sourceforget.net\\\\projects\\\\gofigure2" )
        SET( CPACK_NSIS_URL_INFO_ABOUT "http:\\\\megason.med.harvard.edu" )
#       SET( CPACK_NSIS_CONTACT "me@my-personal-home-page.com" )
        SET( CPACK_NSIS_MODIFY_PATH ON )
      ELSE( WIN32 AND NOT UNIX )
        SET( CPACK_STRIP_FILES "bin/gofigure" )
        SET( CPACK_SOURCE_STRIP_FILES "" )
      ENDIF( WIN32 AND NOT UNIX )

      SET( CPACK_PACKAGE_EXECUTABLES "gofigure" "Gofigure2" )

      IF( UNIX )
        SET( CPACK_BINARY_RPM ON )
        SET( CPACK_RPM_PACKAGE_DEBUG "1" )
        SET( CPACK_RPM_SPEC_INSTALL_POST "/bin/true" )
      ENDIF( UNIX )

      INCLUDE( CPack )

    ENDIF( BUILD_PACKAGE )
  ENDIF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )

  SUBDIRS( Examples )

ENDIF( VTK_FOUND )


