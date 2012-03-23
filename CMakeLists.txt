if( COMMAND CMAKE_POLICY )
  cmake_policy( VERSION 2.6 )
  cmake_policy( SET CMP0005 OLD )
  cmake_policy( SET CMP0003 NEW )
endif(COMMAND CMAKE_POLICY)

# On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security
# warnings. Copied from ITK CMakeLists.
if( WIN32 )
  if( NOT BORLAND )
    if( NOT CYGWIN )
      if( NOT MINGW )
          add_definitions(
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
      endif( NOT MINGW )
    endif( NOT CYGWIN )
  endif( NOT BORLAND )
endif( WIN32 )

#-------------------------------------------------------------------------
# When making one release: update GOFIGURE2_*_VERSION,
# GOFIGURE2_WC_REVISION and the date of the release

set( GOFIGURE2_MAJOR_VERSION "0" )
set( GOFIGURE2_MINOR_VERSION "9" )
set( GOFIGURE2_WC_REVISION "0" )

set( DATE "12/07/2011" )

set( GOFIGURE2_VERSION
  "${GOFIGURE2_MAJOR_VERSION}.${GOFIGURE2_MINOR_VERSION}.${GOFIGURE2_WC_REVISION}" )

set( GOFIGURE2_LIB_VERSION
  "${GOFIGURE2_MAJOR_VERSION}.${GOFIGURE2_MINOR_VERSION}" )

message( STATUS "GoFigure2 version ${GOFIGURE2_VERSION}" )


#---------------------------------------------------------------------------
# CONFIGURE CMAKE
#---------------------------------------------------------------------------

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake/"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake/SuperBuild/"
  )

if( UNIX )
  SET (CACHE CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif( UNIX )


#---------------------------------------------------------------------------
# SUPERBUILD - DISABLED BY DEFAULT
#---------------------------------------------------------------------------

option( SUPERBUILD "Use SuperBuild" OFF )

if( SUPERBUILD )

  cmake_minimum_required( VERSION 2.8.3 )

  project( MYSUPERPROJECT )
  include( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/SuperBuild/SuperBuild.cmake" )
  return()

else( SUPERBUILD )

  # CMake >= 2.8.2 MUST be installed (which is required by ITK)
  cmake_minimum_required( VERSION 2.8.2 )

  project( GOFIGURE2 )

  #-------------------------------------------------------------------------
  option( OPENMP_SUPPORT "Enable OpenMP support if possible" ON )

  if( OPENMP_SUPPORT )

    find_package( OpenMP )

    if( OPENMP_FOUND )
      message( STATUS "OpenMP found" )

      add_definitions( -DHAS_OPENMP )

      set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
      set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
    else()
      message( STATUS "OpenMP not found" )

      remove_definitions( -DHAS_OPENMP )
    endif()

  else()

      remove_definitions( -DHAS_OPENMP )

  endif()

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

  if( NOT GOFIGURE2_INSTALL_BUNDLE_DIR )
    set( GOFIGURE2_INSTALL_BUNDLE_DIR "." )
  endif( NOT GOFIGURE2_INSTALL_BUNDLE_DIR )

  if( NOT GOFIGURE2_INSTALL_BIN_DIR )
    set( GOFIGURE2_INSTALL_BIN_DIR "bin" )
  endif( NOT GOFIGURE2_INSTALL_BIN_DIR )

  if( NOT GOFIGURE2_INSTALL_HEADER_DIR )
    set( GOFIGURE2_INSTALL_HEADER_DIR "include/gofigure2/" )
  endif( NOT GOFIGURE2_INSTALL_HEADER_DIR )

  if( NOT GOFIGURE2_INSTALL_LIB_DIR )
    set( GOFIGURE2_INSTALL_LIB_DIR "lib/gofigure2" )
  endif( NOT GOFIGURE2_INSTALL_LIB_DIR )

  if( NOT GOFIGURE2_INSTALL_DOC_DIR )
    set( GOFIGURE2_INSTALL_DOC_DIR "share/doc/gofigure2" )
  endif( NOT GOFIGURE2_INSTALL_DOC_DIR )

  if( NOT GOFIGURE2_INSTALL_LICENSE_DIR )
    set( GOFIGURE2_INSTALL_LICENSE_DIR "share/doc/gofigure2/Licenses" )
  endif( NOT GOFIGURE2_INSTALL_LICENSE_DIR )

  if( NOT GOFIGURE2_INSTALL_RESOURCE_DIR )
    set( GOFIGURE2_INSTALL_RESOURCE_DIR "share/doc/gofigure2/Resources" )
  endif( NOT GOFIGURE2_INSTALL_RESOURCE_DIR )
  
  #---------------------------------------------------------------------------
  # Define CMAKE_whatever_OUTPUT_DIRECTORY
  #---------------------------------------------------------------------------
  # all target executables will be
  # in ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${GOFIGURE2_BINARY_DIR}/bin
  )

  # all target static libraries will be in ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${GOFIGURE2_BINARY_DIR}/lib
  )

  if( NOT APPLE )
    # all shared libraries will be in ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    # Note that modules (plugins) are considered as shared libraries
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY
    #  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins
       ${GOFIGURE2_BINARY_DIR}/lib
    )
  else()
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY
      ${GOFIGURE2_BINARY_DIR}/bin/gofigure.app/Contents/MacOS/
      )
  endif()

  set( GOFIGURE2_LIBRARY_DIRS ${GOFIGURE2_BINARY_DIR}/lib )

  #---------------------------------------------------------------------------
  # Export files (body of emails, etc...)
  #---------------------------------------------------------------------------
  # define useful variables
  if( WIN32 )
    set( OS "Windows" )
  else( WIN32 )
    IF ( APPLE )
      set( OS "Mac OS X" )
    else( APPLE )
      set( OS "Linux" )
    endif( APPLE )
  endif( WIN32 )

  # Licences
  file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Licenses)

  file(COPY        ${GOFIGURE2_SOURCE_DIR}/Licenses/
     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Licenses)

  # Resources
  file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Resources)

  configure_file( ${GOFIGURE2_SOURCE_DIR}/Resources/BugEntry.txt.in
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Resources/BugEntry.txt
    @ONLY
    )

  #---------------------------------------------------------------------------
  # Define TESTING_DATA_PATH
  #---------------------------------------------------------------------------
  set( TESTING_DATA_PATH
    ${GOFIGURE2_SOURCE_DIR}/Testing/Data
    CACHE PATH "Directory which contains data for testing."
  )

  #---------------------------------------------------------------------------
  # Handle Documentation
  #---------------------------------------------------------------------------
  option( BUILD_DOCUMENTATION "Build Doxygen Documentation" OFF )

  if( BUILD_DOCUMENTATION )
    include( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigDoxygen.cmake" )
  endif( BUILD_DOCUMENTATION )

  #---------------------------------------------------------------------------
  # Main GoFigure Settings
  #---------------------------------------------------------------------------

  #---------------------------------------------------------------------------
  # Everything depends on VTK first
  # that way we can get Qt info from VTK
  #### VTK ####

  include( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigVTK.cmake" )

  #----------------------------------------------------------
  # Then find ITK
  #### ITK ####
  include( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigITK.cmake" )

  #----------------------------------------------------------
  # Then find Boost
  #### BOOST ####
  include( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigBoost.cmake" )

  #----------------------------------------------------------
  # Then find Qt.
  # Note that some command line tools will get compiled even without Qt
  #### Qt ####
  include( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigQT.cmake" )

  #----------------------------------------------------------
  #
  set( GOFIGURE2_EXTERNAL_SOURCE_DIR
    ${GOFIGURE2_SOURCE_DIR}/Code/ExternalCode )
  set( GOFIGURE2_EXTERNAL_BINARY_DIR
    ${GOFIGURE2_BINARY_DIR}/Code/ExternalCode )

  set( MEGAVTK2_SOURCE_DIR ${GOFIGURE2_EXTERNAL_SOURCE_DIR}/MegaVTK )
  set( MEGAVTK2_BINARY_DIR ${GOFIGURE2_EXTERNAL_BINARY_DIR}/MegaVTK )

  link_directories(
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    )

  include_directories( BEFORE
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
    ${GOFIGURE2_BINARY_DIR}/Code/Filters/Contour/
    ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/
    ${GOFIGURE2_BINARY_DIR}/Code/Filters/Mesh/
    ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Attributes/
    ${GOFIGURE2_BINARY_DIR}/Code/Filters/Mesh/Attributes/
    ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Split/
    ${GOFIGURE2_BINARY_DIR}/Code/Filters/Mesh/Split/
    ${GOFIGURE2_SOURCE_DIR}/Code/Filters/Mesh/Merge/
    ${GOFIGURE2_BINARY_DIR}/Code/Filters/Mesh/Merge/
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

  set( QGoResourceFile ${GOFIGURE2_SOURCE_DIR}/Resources/axes.qrc )

  set( QGoGUIINTERFACES_HDRS
    ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoPlugin.h
    ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoImageFilterPluginBase.h
  )
  set( QGoGUIINTERFACES_SRC
    ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoPluginHelper.cxx
    ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoPlugin.cxx
    ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoImageFilterPluginBase.cxx
  )

  install(
      FILES ${QGoGUIINTERFACES_HDRS}
        ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoPluginHelper.h
        ${GOFIGURE2_SOURCE_DIR}/Interfaces/QGoImageSegmentationPluginBase.h
        ${GOFIGURE2_SOURCE_DIR}/Interfaces/PluginInformationBase.h
      DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
      COMPONENT Development
      )

  option( BUILD_COMPARETOOL "Build image comparison tool" OFF )

  add_subdirectory( Code )
  add_subdirectory( Main )

  #-----------------------------------------------------------------------
  # Plugins
  #-----------------------------------------------------------------------
  if( VTK_BUILD_SHARED_LIBS )
    option( BUILD_PLUGINS  "Build GoFigure Plugins" OFF )
  else( VTK_BUILD_SHARED_LIBS )
    set( BUILD_PLUGINS OFF )
  endif( VTK_BUILD_SHARED_LIBS )

  if( BUILD_PLUGINS )
    add_subdirectory( Plugins )
  endif( BUILD_PLUGINS )

  #-----------------------------------------------------------------------
  # Examples and tests
  #-----------------------------------------------------------------------
  option( BUILD_EXAMPLES "Build Examples" OFF )

  if( BUILD_TESTING )
    include( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigTests.cmake" )
  endif( BUILD_TESTING )


  if( BUILD_EXAMPLES )
    include( "${GOFIGURE2_SOURCE_DIR}/CMake/ConfigExamples.cmake" )
  else( BUILD_EXAMPLES )
    set( BUILD_TESTING OFF )
  endif( BUILD_EXAMPLES )

  #-----------------------------------------------------------------------
  #
  #-----------------------------------------------------------------------
  include( GoFigure2IncludeDirectories.cmake )

  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/GoFigure2Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/GoFigure2Config.cmake"
    @ONLY
  )

  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/GoFigure2Config.cmake
    DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR}
    COMPONENT Development
  )

  if( UNIX AND NOT APPLE )
    configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/Resources/GoFigure2.desktop.in"
      "${CMAKE_CURRENT_BINARY_DIR}/GoFigure2.desktop"
      @ONLY
      )

    install(
      FILES ${CMAKE_CURRENT_BINARY_DIR}/GoFigure2.desktop
      DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR}
      )
  endif()

  #-----------------------------------------------------------------------
  # PACKAGE With CPack
  #-----------------------------------------------------------------------
  # add the uninstall support
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/GOFIGURE2Uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/GOFIGURE2Uninstall.cmake"
    @ONLY
    )

  add_custom_target( uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/GOFIGURE2Uninstall.cmake"
    )

  include( GOFIGURE2CPack.cmake )

endif( SUPERBUILD )
