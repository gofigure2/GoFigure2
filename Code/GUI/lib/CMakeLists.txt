IF( WIN32 )
  IF( NOT CYGWIN )
    IF( NOT MINGW )
      IF( BUILD_SHARED_LIBS )
        ADD_DEFINITIONS( -DQGoGUI_EXPORT )
      ENDIF( BUILD_SHARED_LIBS )
    ENDIF( NOT MINGW )
  ENDIF( NOT CYGWIN )
ENDIF( WIN32 )

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/QGoGUILibConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/QGoGUILibConfigure.h @ONLY IMMEDIATE )

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/QGoAboutWidget.cxx.in
  ${CMAKE_CURRENT_BINARY_DIR}/QGoAboutWidget.cxx @ONLY IMMEDIATE )

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/QGoNetworkUtilities.cxx.in
  ${CMAKE_CURRENT_BINARY_DIR}/QGoNetworkUtilities.cxx @ONLY IMMEDIATE )

SET( QGoGUILIB_SRC
  VisualizationTraceContainers/ContourContainer.cxx
  VisualizationTraceContainers/MeshContainer.cxx
  VisualizationTraceContainers/ContourMeshContainer.cxx
  VisualizationTraceContainers/TrackContainer.cxx
  VisualizationTraceContainers/LineageContainer.cxx
  Wizard/QGoConnectServerPage.cxx
  Wizard/QGoCreateImgSessionPage.cxx
  QGoImageView.cxx
  QGoImageView2D.cxx
  QGoImageView3D.cxx
  QGoImageInfo.cxx
  QGoLUTDialog.cxx
  QGoManualSegmentationSettingsDialog.cxx
  Wizard/QGoOpenCreateImgSessionPage.cxx
  Wizard/QGoOpenCreateProjectPage.cxx
  QGoPrintDatabase.cxx
  QGoTabElementBase.cxx
  QGoTabImageView2D.cxx
  QGoTabImageView3D.cxx
  QGoTabImageView3DwT.cxx
  QGoTabImageView4D.cxx
  QGoTabImageViewElementBase.cxx
  QGoTabImageViewNDBase.cxx
  QGoNavigationDockWidget.cxx
  Wizard/QGoWizardDB.cxx
  QSplitterChild.cxx
  QGoTableWidget.cxx
  SnapshotHelper.cxx
  QGoCreateMeshDialog.cxx
  QGoLsmToMegaExportDialog.cxx
  ConversionLsmToMegaThread.cxx
  QGoDBBookmarkManager.cxx
  QTextEditChild.cxx
  QGoNameDescriptionInputDialog.cxx
  QGoDockWidgetStatus.cxx
  QGoDeleteFromListDialog.cxx
  QGoDBNameDescEntityManager.cxx
  QGoDBCellTypeManager.cxx
  QGoDBSubCellTypeManager.cxx
  QGoDBColorManager.cxx
  QGoTraceSettingsWidget.cxx
  Wizard/QGoDBInitializationWizard.cxx
  Wizard/QGoDBInitCreateUserPage.cxx
  Wizard/QGoDBInitCreateAuthorsPage.cxx
  Wizard/QGoDBInitCreateMicroscopePage.cxx
  QGoColorComboBox.cxx
  QGoSelectedColorComboBox.cxx
  QGoCollectionColorComboBox.cxx
  QGoComboBox.cxx
  QGoDBTraceManager.cxx
  QGoDBContourManager.cxx
  QGoDBMeshManager.cxx
  QGoDBTrackManager.cxx
  QGoDBLineageManager.cxx
  QGoColorCodingDialog.cxx

  # base segmentation widgets
  QGoContourSegmentationBaseDockWidget.cxx
  QGoMeshSegmentationBaseDockWidget.cxx

  #--manual segmentation--
  QGoContourManualSegmentation.cxx
  QGoContourManualSegmentationWidget.cxx

  #--seed segmentation--
  QGoSeedSegmentationBase.cxx
    QGoContourSeedSegmentation.cxx
    QGoMeshSeedSegmentation.cxx
  QGoSeedBaseWidget.cxx
    QGoContourSemiAutoLevelsetWidget.cxx
    QGoContourSemiAutoShapeWidget.cxx
    QGoContourSemiAutoWatershedWidget.cxx
  QGoFilterSemiAutoBase.cxx
    QGoFilterChanAndVese.cxx
    QGoFilterShape.cxx
    QGoFilterWatershed.cxx

  # for the tracks
  QGoTrackViewDockWidget.cxx
  QGoTrackEditingWidget.cxx
  
  #for the lineages
  QGoLineageViewDockWidget.cxx
  
  QGoTraceEditingWidget.cxx
  QGoModeEditingWidget.cxx
  QGoAdvancedParametersWidget.cxx
  QGoAlgorithmsWidget.cxx

  ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/QGoAboutWidget.cxx
  ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/QGoNetworkUtilities.cxx
)

SET_SOURCE_FILES_PROPERTIES(
  QGoTabElementBase.cxx
  QGoTabImageViewNDBase.cxx
  QGoTabImageViewElementBase.cxx
  QGoImageView.cxx
  QGoFilterSemiAutoBase.cxx
  QGoDBTraceManager.cxx
  QGoDBNameDescEntityManager.cxx
  QGoColorComboBox.cxx
  ABSTRACT
  )

IF( BUILD_COMPARETOOL )
  SET( QGoGUILIB_SRC
    ${QGoGUILIB_SRC}
    SynchronizedViews/QGoSynchronizedView.cxx
    SynchronizedViews/QGoSynchronizedView2D.cxx
    SynchronizedViews/QGoSynchronizedView3D.cxx
    SynchronizedViews/QGoSynchronizedView2DCallbacks.cxx
    SynchronizedViews/QGoSynchronizedView3DCallbacks.cxx
    SynchronizedViews/QGoSynchronizedViewManager.cxx
  )
ENDIF( BUILD_COMPARETOOL )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

#ADD THIS SRCS IF ENABLE_VIDEO_RECORD is ON
IF( ENABLE_VIDEO_RECORD_FFMPEG )
  SET(QGoGUILIB_SRC
    ${QGoGUILIB_SRC}
    Video/vtkRenderWindowMovieRecorder.cxx
    Video/vtkFFMPEGRenderWindowRecorder.cxx
    Video/QGoVideoRecorder.cxx
  )
ENDIF( ENABLE_VIDEO_RECORD_FFMPEG )

#ADD THIS SRCS IF ENABLE_VIDEO_RECORD is ON
IF( ENABLE_VIDEO_RECORD_AVI )
  SET(QGoGUILIB_SRC
    ${QGoGUILIB_SRC}
    Video/vtkRenderWindowMovieRecorder.cxx
    Video/vtkAVIRenderWindowRecorder.cxx
    Video/QGoVideoRecorder.cxx
  )
ENDIF( ENABLE_VIDEO_RECORD_AVI )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

SET( QGoGUILIB_HDRS
  VisualizationTraceContainers/ContourContainer.h
  VisualizationTraceContainers/MeshContainer.h
  VisualizationTraceContainers/ContourMeshContainer.h
  VisualizationTraceContainers/TrackContainer.h
  VisualizationTraceContainers/LineageContainer.h
  Wizard/QGoConnectServerPage.h
  Wizard/QGoCreateImgSessionPage.h
  QGoImageView.h
  QGoImageView2D.h
  QGoImageView3D.h
  QGoImageInfo.h
  QGoLUTDialog.h
  QGoManualSegmentationSettingsDialog.h
  Wizard/QGoOpenCreateImgSessionPage.h
  Wizard/QGoOpenCreateProjectPage.h
  QGoPrintDatabase.h
  QGoTabElementBase.h
  QGoTabImageView2D.h
  QGoTabImageView3D.h
  QGoTabImageView3DwT.h
  QGoTabImageView4D.h
  QGoTabImageViewElementBase.h
  QGoTabImageViewNDBase.h
  QGoNavigationDockWidget.h
  Wizard/QGoWizardDB.h
  QSplitterChild.h
  QGoTableWidget.h
  QGoCreateMeshDialog.h
  QGoLsmToMegaExportDialog.h
  ConversionLsmToMegaThread.h
  QGoDBBookmarkManager.h
  QTextEditChild.h
  QGoNameDescriptionInputDialog.h
  QGoDockWidgetStatus.h
  QGoDeleteFromListDialog.h
  QGoDBNameDescEntityManager.h
  QGoDBCellTypeManager.h
  QGoDBSubCellTypeManager.h
  QGoDBColorManager.h
  QGoTraceSettingsWidget.h
  QGoAboutWidget.h
  Wizard/QGoDBInitializationWizard.h
  Wizard/QGoDBInitCreateUserPage.h
  Wizard/QGoDBInitCreateAuthorsPage.h
  Wizard/QGoDBInitCreateMicroscopePage.h
  QGoNetworkUtilities.h
  QGoColorComboBox.h
  QGoSelectedColorComboBox.h
  QGoCollectionColorComboBox.h
  QGoComboBox.h
  QGoDBTraceManager.h
  QGoDBContourManager.h
  QGoDBMeshManager.h
  QGoDBTrackManager.h
  QGoDBLineageManager.h
  QGoColorCodingDialog.h

  # base segmentation widgets
  QGoContourSegmentationBaseDockWidget.h
  QGoMeshSegmentationBaseDockWidget.h

  #--manual segmentation--
  QGoContourManualSegmentation.h
  QGoContourManualSegmentationWidget.h

  #--seed segmentation--
  QGoSeedSegmentationBase.h
    QGoContourSeedSegmentation.h
    QGoMeshSeedSegmentation.h
  QGoSeedBaseWidget.h
    QGoContourSemiAutoLevelsetWidget.h
    QGoContourSemiAutoShapeWidget.h
    QGoContourSemiAutoWatershedWidget.h
  QGoFilterSemiAutoBase.h
    QGoFilterChanAndVese.h
    QGoFilterShape.h
    QGoFilterWatershed.h

  # for the tracks
  QGoTrackViewDockWidget.h
  QGoTrackEditingWidget.h
  
  # for the lineages
  QGoLineageViewDockWidget.h
  
  QGoTraceEditingWidget.h
  QGoModeEditingWidget.h
  QGoAdvancedParametersWidget.h
  QGoAlgorithmsWidget.h
)

IF( BUILD_COMPARETOOL )
  SET( QGoGUILIB_HDRS
    ${QGoGUILIB_HDRS}
    SynchronizedViews/QGoSynchronizedView.h
    SynchronizedViews/QGoSynchronizedView2D.h
    SynchronizedViews/QGoSynchronizedView3D.h
    SynchronizedViews/QGoSynchronizedView2DCallbacks.h
    SynchronizedViews/QGoSynchronizedView3DCallbacks.h
    SynchronizedViews/QGoSynchronizedViewManager.h
  )
ENDIF( BUILD_COMPARETOOL )
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

# ADD THIS HEADERS IF ENABLE_VIDEO_RECORD_FFMPEG IS ON
IF( ENABLE_VIDEO_RECORD_FFMPEG )
  SET(QGoGUILIB_HDRS
    ${QGoGUILIB_HDRS}
    Video/QGoVideoRecorder.h
  )
ENDIF( ENABLE_VIDEO_RECORD_FFMPEG )

# ADD THIS HEADERS IF ENABLE_VIDEO_RECORD_AVI IS ON
IF( ENABLE_VIDEO_RECORD_AVI )
  SET(QGoGUILIB_HDRS
    ${QGoGUILIB_HDRS}
    Video/QGoVideoRecorder.h
  )
ENDIF( ENABLE_VIDEO_RECORD_AVI )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

SET( QGoGUILIB_UI

#base dockwidget
  Resources/SegmentationDockWidgetBase.ui

#segmentation widget
  Resources/SegmentationSeedBaseWidget.ui

# manual contour segmentation dock widget
  Resources/ManualSegmentationSettingsDlg.ui
  Resources/ContourManualSegmentationWidget.ui

# seed segmentation dock widgets
  Resources/LevelsetWidget.ui
  Resources/WatershedWidget.ui
  Resources/ShapeWidget.ui

  # other
  Resources/NavigationDockWidget.ui
  Resources/LsmToMegaExportDialog.ui
  Resources/QNameDescriptionInputDialog.ui

  # seed segmentation dock widgets
  Resources/LevelsetWidget.ui
  Resources/WatershedWidget.ui

  # for tracks
  Resources/TrackEditingWidget.ui
)

IF( BUILD_COMPARETOOL )
  SET( QGoGUILIB_UI
    ${QGoGUILIB_UI}
    Resources/QGoSynchronizedView.ui
  )
ENDIF( BUILD_COMPARETOOL )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

# ADD THIS UI IF ENABLE_VIDEO_RECORD_FFMPEG IS ON
IF( ENABLE_VIDEO_RECORD_FFMPEG )
  SET(QGoGUILIB_UI
    ${QGoGUILIB_UI}
    Resources/NewVideoRecorderDockWidget.ui
  )
ENDIF( ENABLE_VIDEO_RECORD_FFMPEG )

# ADD THIS UI IF ENABLE_VIDEO_RECORD_AVI IS ON
IF( ENABLE_VIDEO_RECORD_AVI )
  SET(QGoGUILIB_UI
    ${QGoGUILIB_UI}
    Resources/NewVideoRecorderDockWidget.ui
  )
ENDIF( ENABLE_VIDEO_RECORD_AVI )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

QT4_WRAP_UI( QGoGUILIB_UI_H
  ${QGoGUILIB_UI}
)

QT4_WRAP_CPP( QGoGUILIB_MOC ${QGoGUILIB_HDRS} )

QT4_ADD_RESOURCES( QGoGUI_QRC
  ${QGoResourceFile}
)
SET_SOURCE_FILES_PROPERTIES( ${QGoGUILIB_SRC} PROPERTIES
  OBJECT_DEPENDS "${QGoGUILIB_UI_H}"
  )

SET_SOURCE_FILES_PROPERTIES( ${QGoGUILIB_MOC} PROPERTIES
  OBJECT_DEPENDS "${QGoGUILIB_UI_H}"
  )

ADD_LIBRARY( QGoGui
  ${QGoGUILIB_SRC}
  ${QGoGUILIB_MOC}
  ${QGoGUI_QRC}
)

SET( QGoGUI_LIBS
  QVTK
  QGoIO
  vtkRenderingAddOn2
  PoissonReconstruction
  ctk
  ITKIO
  ITKCommon
  ITKBasicFilters
  ITKStatistics
  )

IF( ( ITK_VERSION_MAJOR EQUAL 4 ) AND
    ( ITK_VERSION_MINOR EQUAL 0 ) AND
    ( ITK_VERSION_PATCH EQUAL 0 )
    )
IF( SUPERBUILD )
  SET( QGoGUI_LIBS
    ${QGoGUI_LIBS}
    ITK-Review
    )
ELSE( SUPERBUILD )
  SET( QGoGUI_LIBS
    ${QGoGUI_LIBS}
    ITKReview
    )
ENDIF( SUPERBUILD )

ENDIF( ( ITK_VERSION_MAJOR EQUAL 4 ) AND
    ( ITK_VERSION_MINOR EQUAL 0 ) AND
    ( ITK_VERSION_PATCH EQUAL 0 )
  )


TARGET_LINK_LIBRARIES( QGoGui
  ${QGoGUI_LIBS}
  )


SET_TARGET_PROPERTIES( QGoGui PROPERTIES VERSION ${GOFIGURE2_LIB_VERSION} SOVERSION ${GOFIGURE2_LIB_VERSION} )

# Runtime
INSTALL( TARGETS QGoGui
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} NAMELINK_SKIP COMPONENT Libraries
  )

# Development
INSTALL( TARGETS QGoGui
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries NAMELINK_ONLY
  )

FILE( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
FILE( GLOB __source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/*.txx" )
FILE( GLOB __binary_file_h "${CMAKE_CURRENT_BINARY_DIR}/*.h" )
FILE( GLOB __binary_file_txx "${CMAKE_CURRENT_BINARY_DIR}/*.txx" )

FILE( GLOB __source_video_h "${CMAKE_CURRENT_SOURCE_DIR}/Video/*.h" )
FILE( GLOB __binary_video_h "${CMAKE_CURRENT_BINARY_DIR}/Video/*.h" )

FILE( GLOB __source_sync_h
  "${CMAKE_CURRENT_SOURCE_DIR}/SynchronizedViews/*.h"
)
FILE( GLOB __binary_sync_h
  "${CMAKE_CURRENT_BINARY_DIR}/SynchronizedViews/*.h"
)
FILE( GLOB __source_tracevisu_h
  "${CMAKE_CURRENT_SOURCE_DIR}/VisualizationTraceContainers/*.h" )
FILE( GLOB __source_trace_visu_txx
  "${CMAKE_CURRENT_SOURCE_DIR}/VisualizationTraceContainers/*.txx" )
FILE( GLOB __binary_tracevisu_h
  "${CMAKE_CURRENT_BINARY_DIR}/VisualizationTraceContainers/*.h" )
FILE( GLOB __binary_tracevisu_txx
  "${CMAKE_CURRENT_BINARY_DIR}/VisualizationTraceContainers/*.txx" )

FILE( GLOB __source_wizard_h
  "${CMAKE_CURRENT_SOURCE_DIR}/Wizard/*.h" )
FILE( GLOB __binary_tracevisu_h
  "${CMAKE_CURRENT_BINARY_DIR}/Wizard/*.h" )

INSTALL( FILES ${__source_file_h} ${__source_file_txx}
    ${__binary_file_h} ${__binary_file_txx}
    ${__source_video_h} ${__binary_video_h}
    ${__source_sync_h} ${__binary_sync_h}
    ${__source_tracevisu_h} ${__source_trace_visu_txx}
    ${__binary_tracevisu_h} ${__binary_tracevisu_txx}
    ${__source_wizard_h} ${__binary_tracevisu_h}
    ${QGoGUILIB_UI_H}
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)

