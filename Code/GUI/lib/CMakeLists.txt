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
  QGoConnectServerPage.cxx
  QGoCreateDataBasePage.cxx
  QGoCreateImgSessionPage.cxx
  QGoImageView.cxx
  QGoImageView2D.cxx
  QGoImageView3D.cxx
  QGoImageInfo.cxx
  QGoLUTDialog.cxx
  QGoManualSegmentationDockWidget.cxx
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
  QGoVisualizationDockWidget.cxx
  QGoWizardDB.cxx
  QSplitterChild.cxx
  QTableWidgetChild.cxx
  SnapshotHelper.cxx
  QGoCreateMeshDialog.cxx
  qtcolorcombobox.cxx
  QGoLsmToMegaExportDialog.cxx
  ConversionLsmToMegaThread.cxx
  QGoDBBookmarkManager.cxx
  QTextEditChild.cxx
  QNameDescriptionInputDialog.cxx
  QGoDockWidgetStatus.cxx
  QGoDeleteDBEntityDialog.cxx
  QGoDBEntityManager.cxx
  QGoDBCellTypeManager.cxx
  QGoDBSubCellTypeManager.cxx
  QGoTraceManualEditingWidget.cxx
  QGoTraceManualEditingDockWidget.cxx
  QGoDBInitializationWizard.cxx
  QGoDBInitCreateUserPage.cxx
  QGoDBInitCreateAuthorsPage.cxx
  QGoDBInitCreateMicroscopePage.cxx
  QGoOneClickSegmentationDockWidget.cxx

  QGoComparer.cxx
  QGoComparer2D.cxx
  QGoComparer3D.cxx
  QGoComparer2DSync.cxx
  QGoComparer3DSync.cxx
  QGoCompareOrchestra.cxx
  qgocomparegui.cxx

  QGoSeedsSegmentation.cxx

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
  QGoConnectServerPage.h
  QGoCreateDataBasePage.h
  QGoCreateImgSessionPage.h
  QGoImageView.h
  QGoImageView2D.h
  QGoImageView3D.h
  QGoImageInfo.h
  QGoLUTDialog.h
  QGoManualSegmentationDockWidget.h
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
  QGoVisualizationDockWidget.h
  QGoWizardDB.h
  QSplitterChild.h
  QTableWidgetChild.h
  QGoCreateMeshDialog.h
  qtcolorcombobox.h
  QGoLsmToMegaExportDialog.h
  ConversionLsmToMegaThread.h
  QGoDBBookmarkManager.h
  QTextEditChild.h
  QNameDescriptionInputDialog.h
  QGoDockWidgetStatus.h
  QGoDeleteDBEntityDialog.h
  QGoDBEntityManager.h
  QGoDBCellTypeManager.h
  QGoDBSubCellTypeManager.h
  QGoTraceManualEditingWidget.h
  QGoTraceManualEditingDockWidget.h
  QGoOneClickSegmentationDockWidget.h
  QGoAboutWidget.h
  QGoDBInitializationWizard.h
  QGoDBInitCreateUserPage.h
  QGoDBInitCreateAuthorsPage.h
  QGoDBInitCreateMicroscopePage.h
  QGoNetworkUtilities.h

  QGoComparer.h
  QGoComparer2D.h
  QGoComparer3D.h
  QGoComparer3DSync.h
  QGoCompareOrchestra.h
  qgocomparegui.h
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
  Resources/ManualSegmentationSettingsDlg.ui
  Resources/ManualSegmentationDockWidget.ui
  Resources/VisualizationDockWidget.ui
  Resources/QGoPrintDatabase.ui
  Resources/LsmToMegaExportDialog.ui
  Resources/QNameDescriptionInputDialog.ui
  Resources/QGoTraceManualEditingWidget.ui
  Resources/OneClickSegmentationDockWidget.ui

  Resources/QGoComparer.ui
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
  )

TARGET_LINK_LIBRARIES( QGoGui
  ${QGoGUI_LIBS}
  ITKIO
  ITKQuadEdgeMesh
  ITKCommon
  ITKBasicFilters
  ITKAlgorithms
  ITKStatistics
  ITKNumerics
  )

INSTALL( TARGETS QGoGui
  ARCHIVE DESTINATION lib COMPONENT libraries
  LIBRARY DESTINATION lib COMPONENT libraries
  RUNTIME DESTINATION bin COMPONENT Runtime
  )

