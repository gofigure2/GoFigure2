SET( QGoGUILIB_SRC
  ContourStructureHelper.cxx
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
  QGoVisualizationDockWidget.cxx
  QGoWizardDB.cxx
  QSplitterChild.cxx
  QTableWidgetChild.cxx
  SnapshotHelper.cxx
  QTableWidgetNumericalItem.cxx
  QGoCreateMeshDialog.cxx
)

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
  QGoVisualizationDockWidget.h
  QGoWizardDB.h
  QSplitterChild.h
  QTableWidgetChild.h
  QGoCreateMeshDialog.h
)

SET( QGoGUILIB_UI
  ManualSegmentationSettingsDlg.ui
  ManualSegmentationDockWidget.ui
  VisualizationDockWidget.ui
  QGoPrintDatabase.ui
)

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

