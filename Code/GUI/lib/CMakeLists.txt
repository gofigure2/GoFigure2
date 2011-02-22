IF( WIN32 )
  IF( NOT CYGWIN )
    IF( NOT MINGW )
      IF( BUILD_SHARED_LIBS )
        ADD_DEFINITIONS( -DQGoGUI_EXPORT )
      ENDIF( BUILD_SHARED_LIBS )
    ENDIF( NOT MINGW )
  ENDIF( NOT CYGWIN )
ENDIF( WIN32 )

CONFIGURE_FILE( ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/QGoGUILibConfigure.h.in
  ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/QGoGUILibConfigure.h @ONLY IMMEDIATE )

CONFIGURE_FILE( ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/QGoAboutWidget.cxx.in
  ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/QGoAboutWidget.cxx @ONLY IMMEDIATE )

CONFIGURE_FILE( ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/QGoNetworkUtilities.cxx.in
  ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/QGoNetworkUtilities.cxx @ONLY IMMEDIATE )

SET( QGoGUILIB_SRC
  ContourContainer.cxx
  MeshContainer.cxx
  ContourMeshContainer.cxx
  TrackContainer.cxx
  QGoConnectServerPage.cxx
  QGoCreateImgSessionPage.cxx
  QGoImageView.cxx
  QGoImageView2D.cxx
  QGoImageView3D.cxx
  QGoImageInfo.cxx
  QGoLUTDialog.cxx
  QGoManualSegmentationSettingsDialog.cxx
  QGoOpenCreateImgSessionPage.cxx
  QGoOpenCreateProjectPage.cxx
  QGoPrintDatabase.cxx
  QGoTabElementBase.cxx
  QGoTabImageView2D.cxx
  QGoTabImageView3D.cxx
  QGoTabImageView3DwT.cxx
  QGoTabImageView4D.cxx
  QGoTabImageViewElementBase.cxx
  QGoTabImageViewNDBase.cxx
  QGoNavigationDockWidget.cxx
  QGoWizardDB.cxx
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
  QGoTraceSettingsDockWidget.cxx
  QGoTraceSettingsWidget.cxx
  QGoDBInitializationWizard.cxx
  QGoDBInitCreateUserPage.cxx
  QGoDBInitCreateAuthorsPage.cxx
  QGoDBInitCreateMicroscopePage.cxx
  QGoColorComboBox.cxx
  QGoSelectedColorComboBox.cxx
  QGoCollectionColorComboBox.cxx
  QGoComboBox.cxx
  QGoSynchronizedView.cxx
  QGoSynchronizedView2D.cxx
  QGoSynchronizedView3D.cxx
  QGoSynchronizedView2DCallbacks.cxx
  QGoSynchronizedView3DCallbacks.cxx
  QGoSynchronizedViewManager.cxx
  QGoDBTraceManager.cxx
  QGoDBContourManager.cxx
  QGoDBMeshManager.cxx
  QGoDBTrackManager.cxx
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
  QGoTrackDockWidget.cxx
  QGoTrackEditingWidget.cxx
  QGoTraceEditingWidget.cxx
  QGoModeEditingWidget.cxx
  QGoAdvancedParametersWidget.cxx
  QGoAlgorithmsManagerWidget.cxx

  ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/QGoAboutWidget.cxx
  ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/QGoNetworkUtilities.cxx
)

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

#ADD THIS SRCS IF ENABLE_VIDEO_RECORD is ON
IF( ENABLE_VIDEO_RECORD_FFMPEG )
  SET(QGoGUILIB_SRC
    ${QGoGUILIB_SRC}
    vtkRenderWindowMovieRecorder.cxx
    vtkFFMPEGRenderWindowRecorder.cxx
    QGoVideoRecorder.cxx
  )
ENDIF( ENABLE_VIDEO_RECORD_FFMPEG )

#ADD THIS SRCS IF ENABLE_VIDEO_RECORD is ON
IF( ENABLE_VIDEO_RECORD_AVI )
  SET(QGoGUILIB_SRC
    ${QGoGUILIB_SRC}
    vtkRenderWindowMovieRecorder.cxx
    vtkAVIRenderWindowRecorder.cxx
    QGoVideoRecorder.cxx
  )
ENDIF( ENABLE_VIDEO_RECORD_AVI )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

SET( QGoGUILIB_HDRS
  ContourContainer.h
  MeshContainer.h
  ContourMeshContainer.h
  TrackContainer.h
  QGoConnectServerPage.h
  QGoCreateImgSessionPage.h
  QGoImageView.h
  QGoImageView2D.h
  QGoImageView3D.h
  QGoImageInfo.h
  QGoLUTDialog.h
  QGoManualSegmentationSettingsDialog.h
  QGoOpenCreateImgSessionPage.h
  QGoOpenCreateProjectPage.h
  QGoPrintDatabase.h
  QGoTabElementBase.h
  QGoTabImageView2D.h
  QGoTabImageView3D.h
  QGoTabImageView3DwT.h
  QGoTabImageView4D.h
  QGoTabImageViewElementBase.h
  QGoTabImageViewNDBase.h
  QGoNavigationDockWidget.h
  QGoWizardDB.h
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
  QGoTraceSettingsDockWidget.h
  QGoTraceSettingsWidget.h
  QGoAboutWidget.h
  QGoDBInitializationWizard.h
  QGoDBInitCreateUserPage.h
  QGoDBInitCreateAuthorsPage.h
  QGoDBInitCreateMicroscopePage.h
  QGoNetworkUtilities.h
  QGoSynchronizedView.h
  QGoSynchronizedView2D.h
  QGoSynchronizedView3D.h
  QGoSynchronizedView2DCallbacks.h
  QGoSynchronizedView3DCallbacks.h
  QGoSynchronizedViewManager.h
  QGoColorComboBox.h
  QGoSelectedColorComboBox.h
  QGoCollectionColorComboBox.h
  QGoComboBox.h
  QGoDBTraceManager.h
  QGoDBContourManager.h
  QGoDBMeshManager.h
  QGoDBTrackManager.h
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
  QGoTrackDockWidget.h
  QGoTrackEditingWidget.h
  QGoTraceEditingWidget.h
  QGoModeEditingWidget.h
  QGoAdvancedParametersWidget.h
  QGoAlgorithmsManagerWidget.h
)

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

# ADD THIS HEADERS IF ENABLE_VIDEO_RECORD_FFMPEG IS ON
IF( ENABLE_VIDEO_RECORD_FFMPEG )
  SET(QGoGUILIB_HDRS
    ${QGoGUILIB_HDRS}
    QGoVideoRecorder.h
  )
ENDIF( ENABLE_VIDEO_RECORD_FFMPEG )

# ADD THIS HEADERS IF ENABLE_VIDEO_RECORD_AVI IS ON
IF( ENABLE_VIDEO_RECORD_AVI )
  SET(QGoGUILIB_HDRS
    ${QGoGUILIB_HDRS}
    QGoVideoRecorder.h
  )
ENDIF( ENABLE_VIDEO_RECORD_AVI )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

SET( QGoGUILIB_UI
#segmentation widget
  Resources/SegmentationBaseDockWidget.ui
  Resources/SegmentationSeedBaseWidget.ui

# manual contour segmentation dock widget
  Resources/ManualSegmentationSettingsDlg.ui
  Resources/ContourManualSegmentationWidget.ui
  Resources/ContourSemiAutoShapeWidget.ui

# other
  Resources/NavigationDockWidget.ui
  Resources/QGoPrintDatabase.ui
  Resources/LsmToMegaExportDialog.ui
  Resources/QNameDescriptionInputDialog.ui
  Resources/QGoTraceSettingsWidget.ui

  Resources/QGoSynchronizedView.ui

  # seed segmentation dock widgets
  Resources/BaseAlgoSegmentationDockWidget.ui
  Resources/LevelsetWidget.ui
  Resources/WatershedWidget.ui

  # for tracks
  Resources/TrackDockWidget.ui
  Resources/TrackEditingWidget.ui
)

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

  SET( QGoGUI_LIBS
    ${QGoGUI_LIBS}
    ITKReview
    )

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


