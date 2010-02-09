SET( QGoGUILIB_SRC
  ContourMeshStructureHelper.cxx
  QGoConnectServerPage.cxx
  QGoCreateDataBasePage.cxx
  QGoCreateImgSessionPage.cxx
  QGoImageView.cxx
  QGoImageView2D.cxx
  QGoImageView3D.cxx
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
)

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------

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

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------

SET( QGoGUILIB_HDRS
  QGoConnectServerPage.h
  QGoCreateDataBasePage.h
  QGoCreateImgSessionPage.h
  QGoImageView.h
  QGoImageView2D.h
  QGoImageView3D.h
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
)

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------

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

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------

SET( QGoGUILIB_UI
  ManualSegmentationSettingsDlg.ui
  ManualSegmentationDockWidget.ui
  VisualizationDockWidget.ui
  QGoPrintDatabase.ui
  LsmToMegaExportDialog.ui
  QNameDescriptionInputDialog.ui
)

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------

# ADD THIS UI IF ENABLE_VIDEO_RECORD_FFMPEG IS ON
IF( ENABLE_VIDEO_RECORD_FFMPEG )
  SET(QGoGUILIB_UI
    ${QGoGUILIB_UI}
    NewVideoRecorderDockWidget.ui
  )
ENDIF( ENABLE_VIDEO_RECORD_FFMPEG )

# ADD THIS UI IF ENABLE_VIDEO_RECORD_AVI IS ON
IF( ENABLE_VIDEO_RECORD_AVI )
  SET(QGoGUILIB_UI
    ${QGoGUILIB_UI}
    NewVideoRecorderDockWidget.ui
  )
ENDIF( ENABLE_VIDEO_RECORD_AVI )

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------

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

TARGET_LINK_LIBRARIES( QGoGui
  QGoGui
  QGoIO
  ${QT_LIBRARIES}
  QVTK
  vtkRenderingAddOn2
)

INSTALL( TARGETS QGoGui
    ARCHIVE
    DESTINATION lib
    COMPONENT libraries
	  )

