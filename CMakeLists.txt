IF( COMMAND CMAKE_POLICY )
  CMAKE_POLICY( VERSION 2.6 )
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

#---------------------------------------------------------------------------
# CONFIGURE CMAKE
#---------------------------------------------------------------------------

  SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/SuperBuild/"
  )

  IF( UNIX )
    SET (CACHE CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  ENDIF( UNIX )


#---------------------------------------------------------------------------
# SUPERBUILD - ENABLED BY DEFAULT
#---------------------------------------------------------------------------

OPTION( SUPERBUILD "Use SuperBuild" OFF )

IF( SUPERBUILD )

  CMAKE_MINIMUM_REQUIRED( VERSION 2.8.3 )

  PROJECT( MYSUPERPROJECT )
  INCLUDE( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/SuperBuild/SuperBuild.cmake" )
  RETURN()

ELSE( SUPERBUILD )

  # CMake >= 2.8.2 MUST be installed (which is required by ITK)
  CMAKE_MINIMUM_REQUIRED( VERSION 2.8.2 )

  PROJECT( GOFIGURE2 )

  #-------------------------------------------------------------------------
  FIND_PACKAGE( OpenMP )

  IF( OPENMP_FOUND )
    MESSAGE( STATUS "OpenMP found" )

    ADD_DEFINITIONS( -DHAS_OPENMP )

    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
    SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
  ELSE()
    MESSAGE( STATUS "OpenMP not found" )

    REMOVE_DEFINITIONS( -DHAS_OPENMP )
  ENDIF()

  #-------------------------------------------------------------------------
  # When making one release: update GOFIGURE2_*_VERSION,
  # GOFIGURE2_WC_REVISION and the date of the release
  SET( GOFIGURE2_MAJOR_VERSION "0" )
  SET( GOFIGURE2_MINOR_VERSION "8" )
  SET( GOFIGURE2_WC_REVISION "develop" )

  SET( DATE "10/01/2011" )

  SET( GOFIGURE2_VERSION
    "${GOFIGURE2_MAJOR_VERSION}.${GOFIGURE2_MINOR_VERSION}.${GOFIGURE2_WC_REVISION}" )

  SET( GOFIGURE2_LIB_VERSION
    "${GOFIGURE2_MAJOR_VERSION}.${GOFIGURE2_MINOR_VERSION}" )

  MESSAGE( STATUS "GoFigure2 version ${GOFIGURE2_VERSION}" )

  #-----------------------------------------------------------------------
  # Export and Install configuration
  #
  # GOFIGURE2_INSTALL_BUNDLE_DIR       - bundle dir
  # GOFIGURE2_INSTALL_HEADER_DIR       - include dir (headers)
  # GOFIGURE2_INSTALL_BIN_DIR          - binary dir (executables)
  # GOFIGURE2_INSTALL_LIB_DIR          - library dir (libs)
  # GOFIGURE2_INSTALL_DOC_DIR          - documentation dir
  # GOFIGURE2_INSTALL_LICENSE_DIR      - licenses dir
  # GOFIGURE2_INSTALL_RESOURCE_DIR     - resources dir
  #-----------------------------------------------------------------------

  IF( NOT GOFIGURE2_INSTALL_BUNDLE_DIR )
    SET( GOFIGURE2_INSTALL_BUNDLE_DIR "." )
  ENDIF( NOT GOFIGURE2_INSTALL_BUNDLE_DIR )

  IF( NOT GOFIGURE2_INSTALL_BIN_DIR )
    SET( GOFIGURE2_INSTALL_BIN_DIR "bin" )
  ENDIF( NOT GOFIGURE2_INSTALL_BIN_DIR )

  IF( NOT GOFIGURE2_INSTALL_HEADER_DIR )
    SET( GOFIGURE2_INSTALL_HEADER_DIR "include/gofigure2/" )
  ENDIF( NOT GOFIGURE2_INSTALL_HEADER_DIR )

  IF( NOT GOFIGURE2_INSTALL_LIB_DIR )
    SET( GOFIGURE2_INSTALL_LIB_DIR "lib/gofigure2" )
  ENDIF( NOT GOFIGURE2_INSTALL_LIB_DIR )

  IF( NOT GOFIGURE2_INSTALL_DOC_DIR )
    SET( GOFIGURE2_INSTALL_DOC_DIR "share/doc/gofigure2" )
  ENDIF( NOT GOFIGURE2_INSTALL_DOC_DIR )

  IF( NOT GOFIGURE2_INSTALL_LICENSE_DIR )
    SET( GOFIGURE2_INSTALL_LICENSE_DIR "share/doc/gofigure2/Licenses" )
  ENDIF( NOT GOFIGURE2_INSTALL_LICENSE_DIR )

  IF( NOT GOFIGURE2_INSTALL_RESOURCE_DIR )
    SET( GOFIGURE2_INSTALL_RESOURCE_DIR "share/doc/gofigure2/Resources" )
  ENDIF( NOT GOFIGURE2_INSTALL_RESOURCE_DIR )
  
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
# Export files (body of emails, etc...)
#---------------------------------------------------------------------------
# define useful variables
IF( WIN32 )
  SET( OS "Windows" )
ELSE( WIN32 )
  IF ( APPLE )
    SET( OS "Mac OS X" )
  ELSE( APPLE )
    SET( OS "Linux" )
  ENDIF( APPLE )
ENDIF( WIN32 )

# Licences
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Licenses)

file(COPY        ${GOFIGURE2_SOURCE_DIR}/Licenses/
     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Licenses)

# Resources
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Resources)

CONFIGURE_FILE( ${GOFIGURE2_SOURCE_DIR}/Resources/BugEntry.txt.in
 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Resources/BugEntry.txt
 @ONLY
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

  IF( BUILD_DOCUMENTATION )
    INCLUDE( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigDoxygen.cmake" )
  ENDIF( BUILD_DOCUMENTATION )

  #---------------------------------------------------------------------------
  # Main GoFigure Settings
  #---------------------------------------------------------------------------

  #---------------------------------------------------------------------------
  # Everything depends on VTK first
  # that way we can get Qt info from VTK
  #### VTK ####

  INCLUDE( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigVTK.cmake" )

  #----------------------------------------------------------
  # Then find ITK
  #### ITK ####
  INCLUDE( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigITK.cmake" )

  #----------------------------------------------------------
  # Then find Boost. Note that we only need headers
  #### BOOST ####
  INCLUDE( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigBoost.cmake" )

  #----------------------------------------------------------
  # Then find Qt.
  # Note that some command line tools will get compiled even without Qt
  #### Qt ####
  INCLUDE( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigQT.cmake" )

  #----------------------------------------------------------
  #
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
    ${GOFIGURE2_SOURCE_DIR}/Code/IO/GoDBRow/
    ${GOFIGURE2_BINARY_DIR}/Code/IO/GoDBRow/
    ${GOFIGURE2_SOURCE_DIR}/Code/IO/GoImage/
    ${GOFIGURE2_BINARY_DIR}/Code/IO/GoImage/
    ${GOFIGURE2_SOURCE_DIR}/Code/Filters/
    ${GOFIGURE2_BINARY_DIR}/Code/Filters/
    ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Contour/
    ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/
    ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Attributes/
    ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Split/
    ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Merge/
    ${GOFIGURE2_SOURCE_DIR}/Code/GUI/
    ${GOFIGURE2_BINARY_DIR}/Code/GUI/
    ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/
    ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/
    ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/Resources/
    ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/DBManager/
    ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/DBManager/
    ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/SynchronizedViews/
    ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/SynchronizedViews/
    ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/VisualizationTraceContainers/
    ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/VisualizationTraceContainers/
    ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/Video/
    ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/Video/
    ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/Wizard/
    ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/Wizard/
    ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/TraceEditing/
    ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/TraceEditing/
    ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/TransferFunctionEditor/
    ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/TransferFunctionEditor/
    ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/LineageViewer/
    ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/LineageViewer/
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

  INSTALL(
      FILES ${QGoGUIINTERFACES_HDRS}
        ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoPluginHelper.h
        ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoImageSegmentationPluginBase.h
        ${GOFIGURE2_SOURCE_DIR}/Interfaces/PluginInformationBase.h
      DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
      COMPONENT Development
      )

  OPTION( BUILD_COMPARETOOL "Build image comparison tool" OFF )

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
    INCLUDE( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigTests.cmake" )
  ENDIF( BUILD_TESTING )


  IF( BUILD_EXAMPLES )
    INCLUDE( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigExamples.cmake" )
  ELSE( BUILD_EXAMPLES )
    SET( BUILD_TESTING OFF )
  ENDIF( BUILD_EXAMPLES )

  #-----------------------------------------------------------------------
  #
  #-----------------------------------------------------------------------
  INCLUDE( GoFigure2IncludeDirectories.cmake )

  CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/GoFigure2Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/GoFigure2Config.cmake"
    @ONLY
  )

  INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/GoFigure2Config.cmake
    DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR}
    COMPONENT Development
  )

  IF( UNIX AND NOT APPLE )
    CONFIGURE_FILE(
      "${CMAKE_CURRENT_SOURCE_DIR}/Resources/GoFigure2.desktop.in"
      "${CMAKE_CURRENT_BINARY_DIR}/GoFigure2.desktop"
      @ONLY
      )

    INSTALL(
      FILES ${CMAKE_CURRENT_BINARY_DIR}/GoFigure2.desktop
      DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR}
      )
  ENDIF()

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

ENDIF( SUPERBUILD )
