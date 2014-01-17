if( WIN32 )
  if( NOT CYGWIN )
    if( NOT MINGW )
      if( BUILD_SHARED_LIBS )
        add_definitions( -DQGoGUI_EXPORT )
      endif( BUILD_SHARED_LIBS )
    endif( NOT MINGW )
  endif( NOT CYGWIN )
endif( WIN32 )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/QGoGUILibConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/QGoGUILibConfigure.h @ONLY IMMEDIATE )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/QGoAboutWidget.cxx.in
  ${CMAKE_CURRENT_BINARY_DIR}/QGoAboutWidget.cxx @ONLY IMMEDIATE )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/QGoNetworkUtilities.cxx.in
  ${CMAKE_CURRENT_BINARY_DIR}/QGoNetworkUtilities.cxx @ONLY IMMEDIATE )

set( QGoGUILIB_SRC
  QGoImageView.cxx
  QGoImageView2D.cxx
  QGoImageView3D.cxx
  QGoPrintDatabase.cxx
  QGoImageInfo.cxx
  QGoLUTDialog.cxx
  TraceEditing/QGoManualSegmentationSettingsDialog.cxx
  QGoTabElementBase.cxx
  QGoTabImageView2D.cxx
  QGoTabImageView3D.cxx
  QGoTabImageView3DwT.cxx
  QGoTabImageViewElementBase.cxx
  QGoTabImageViewNDBase.cxx
  QGoNavigationDockWidget.cxx
  QSplitterChild.cxx
  QGoTableWidget.cxx
  SnapshotHelper.cxx
  QGoCreateMeshDialog.cxx
  QGoLsmToMegaExportDialog.cxx
  ConversionLsmToMegaThread.cxx
  QTextEditChild.cxx
  QGoNameDescriptionInputDialog.cxx
  QGoDockWidgetStatus.cxx
  QGoDeleteFromListDialog.cxx
  QGoTraceSettingsWidget.cxx
  QGoColorComboBox.cxx
  QGoSelectedColorComboBox.cxx
  QGoCollectionColorComboBox.cxx
  QGoComboBox.cxx
  QGoColorCodingDialog.cxx
  QGoDockWidget.cxx
  QGoToolBarStatus.cxx

  # trace containers
  VisualizationTraceContainers/ContourContainer.cxx
  VisualizationTraceContainers/MeshContainer.cxx
  VisualizationTraceContainers/ContourMeshContainer.cxx
  VisualizationTraceContainers/TrackContainer.cxx
  VisualizationTraceContainers/LineageContainer.cxx
  
  # Wizard
  Wizard/QGoDBInitializationWizard.cxx
  Wizard/QGoDBInitCreateUserPage.cxx
  Wizard/QGoDBInitCreateAuthorsPage.cxx
  Wizard/QGoDBInitCreateMicroscopePage.cxx
  Wizard/QGoOpenCreateImgSessionPage.cxx
  Wizard/QGoOpenCreateProjectPage.cxx
  Wizard/QGoConnectServerPage.cxx
  Wizard/QGoCreateImgSessionPage.cxx
  Wizard/QGoWizardDB.cxx
 
  # DB Managers
  DBManager/QGoDBTraceManager.cxx
  DBManager/QGoDBContourManager.cxx
  DBManager/QGoDBMeshManager.cxx
  DBManager/QGoDBTrackManager.cxx
  DBManager/QGoDBLineageManager.cxx
  DBManager/QGoDBNameDescEntityManager.cxx
  DBManager/QGoDBCellTypeManager.cxx
  DBManager/QGoDBSubCellTypeManager.cxx
  DBManager/QGoDBColorManager.cxx
  DBManager/QGoDBBookmarkManager.cxx

  # for the tracks
  QGoTrackViewDockWidget.cxx
  QGoTrackEditingWidget.cxx

  #for the lineages
  QGoLineageViewDockWidget.cxx

  #--manual segmentation--
  TraceEditing/QGoContourManualSegmentation.cxx
  TraceEditing/QGoContourManualSegmentationWidget.cxx
  TraceEditing/QGoFilterChanAndVese.cxx
  TraceEditing/QGoFilterShape.cxx
  TraceEditing/QGoFilterWatershed.cxx
  TraceEditing/QGoTraceEditingWidget.cxx
  TraceEditing/QGoModesManagerWidget.cxx
  TraceEditing/QGoAlgorithmsManagerWidget.cxx
  TraceEditing/QGoAlgorithmWidget.cxx
  TraceEditing/QGoMeshEditingWidgetManager.cxx
  TraceEditing/QGoContourEditingWidgetManager.cxx
  TraceEditing/QGoTraceEditingWidgetManager.cxx
  TraceEditing/QGoSegmentationAlgo.cxx
  TraceEditing/QGoContourLevelSetAlgo.cxx
  TraceEditing/QGoMeshLevelSetAlgo.cxx
  TraceEditing/QGoMeshShapeAlgo.cxx
  TraceEditing/QGoMeshWaterShedAlgo.cxx
  TraceEditing/QGoMeshSplitDanielssonDistanceAlgo.cxx
  TraceEditing/QGoMeshMergeConvexHullAlgo.cxx
  TraceEditing/QGoWaterShedAlgo.cxx
  TraceEditing/QGoSetOfContoursWaterShedAlgo.cxx
  TraceEditing/QGoSetOfContoursLevelSetAlgo.cxx
  TraceEditing/QGoLevelSetAlgo.cxx
  TraceEditing/QGoShapeAlgo.cxx
  TraceEditing/QGoSplitDanielssonDistanceAlgo.cxx
  TraceEditing/QGoMergeConvexHullAlgo.cxx
  TraceEditing/QGoSetOfContoursShapeAlgo.cxx
  TraceEditing/QGoSemiAutoSegmentationAlgo.cxx
  TraceEditing/QGoSplitSegmentationAlgo.cxx
  TraceEditing/QGoFilterSemiAutoBase.cxx

  # transfer function widget
  TransferFunctionEditor/QGoTransferFunctionDockWidget.cxx
  TransferFunctionEditor/GoTransferFunctionEditorWidget.cxx
  TransferFunctionEditor/GoTransferFunctionWidget.cxx
  TransferFunctionEditor/hoverpoints.cpp

  # lineage viewer
  LineageViewer/QGoLineageViewerWidget.cxx

  ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/QGoAboutWidget.cxx
  ${GOFIGURE2_BINARY_DIR}/Code/GUI/lib/QGoNetworkUtilities.cxx
)

set_source_files_properties(
  QGoTabElementBase.cxx
  QGoTabImageViewNDBase.cxx
  QGoTabImageViewElementBase.cxx
  QGoImageView.cxx
  QGoFilterSemiAutoBase.cxx
  DBManager/QGoDBTraceManager.cxx
  QGoDBNameDescEntityManager.cxx
  QGoColorComboBox.cxx
  ABSTRACT
  )

if( BUILD_COMPARETOOL )
  set( QGoGUILIB_SRC
    ${QGoGUILIB_SRC}
    SynchronizedViews/QGoSynchronizedView.cxx
    SynchronizedViews/QGoSynchronizedView2D.cxx
    SynchronizedViews/QGoSynchronizedView3D.cxx
    SynchronizedViews/QGoSynchronizedView2DCallbacks.cxx
    SynchronizedViews/QGoSynchronizedView3DCallbacks.cxx
    SynchronizedViews/QGoSynchronizedViewManager.cxx
  )
endif( BUILD_COMPARETOOL )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

#ADD THIS SRCS IF ENABLE_VIDEO_RECORD is ON
if( ENABLE_VIDEO_RECORD_FFMPEG )
  set(QGoGUILIB_SRC
    ${QGoGUILIB_SRC}
    Video/vtkRenderWindowMovieRecorder.cxx
    Video/vtkFFMPEGRenderWindowRecorder.cxx
    Video/QGoVideoRecorder.cxx
  )
endif( ENABLE_VIDEO_RECORD_FFMPEG )

#ADD THIS SRCS IF ENABLE_VIDEO_RECORD is ON
if( ENABLE_VIDEO_RECORD_AVI )
  set(QGoGUILIB_SRC
    ${QGoGUILIB_SRC}
    Video/vtkRenderWindowMovieRecorder.cxx
    Video/vtkAVIRenderWindowRecorder.cxx
    Video/QGoVideoRecorder.cxx
  )
endif( ENABLE_VIDEO_RECORD_AVI )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

set( QGoGUILIB_HDRS
  QGoImageView.h
  QGoImageView2D.h
  QGoImageView3D.h
  QGoImageInfo.h
  QGoLUTDialog.h
  TraceEditing/QGoManualSegmentationSettingsDialog.h
  QGoPrintDatabase.h
  QGoTabElementBase.h
  QGoTabImageView2D.h
  QGoTabImageView3D.h
  QGoTabImageView3DwT.h
  QGoTabImageViewElementBase.h
  QGoTabImageViewNDBase.h
  QGoNavigationDockWidget.h
  QSplitterChild.h
  QGoTableWidget.h
  QGoCreateMeshDialog.h
  QGoLsmToMegaExportDialog.h
  ConversionLsmToMegaThread.h
  QTextEditChild.h
  QGoNameDescriptionInputDialog.h
  QGoDockWidgetStatus.h
  QGoDeleteFromListDialog.h
  QGoTraceSettingsWidget.h
  QGoAboutWidget.h
  QGoNetworkUtilities.h
  QGoColorComboBox.h
  QGoSelectedColorComboBox.h
  QGoCollectionColorComboBox.h
  QGoComboBox.h
  QGoColorCodingDialog.h
  QGoDockWidget.h
  QGoToolBarStatus.h

  # Trace containers
  VisualizationTraceContainers/ContourContainer.h
  VisualizationTraceContainers/MeshContainer.h
  VisualizationTraceContainers/ContourMeshContainer.h
  VisualizationTraceContainers/TrackContainer.h
  VisualizationTraceContainers/LineageContainer.h

  # Wizard
  Wizard/QGoConnectServerPage.h
  Wizard/QGoCreateImgSessionPage.h
  Wizard/QGoOpenCreateImgSessionPage.h
  Wizard/QGoOpenCreateProjectPage.h
  Wizard/QGoWizardDB.h
  Wizard/QGoDBInitializationWizard.h
  Wizard/QGoDBInitCreateUserPage.h
  Wizard/QGoDBInitCreateAuthorsPage.h
  Wizard/QGoDBInitCreateMicroscopePage.h

  # DB Manager
  DBManager/QGoDBBookmarkManager.h
  DBManager/QGoDBNameDescEntityManager.h
  DBManager/QGoDBCellTypeManager.h
  DBManager/QGoDBSubCellTypeManager.h
  DBManager/QGoDBColorManager.h
  DBManager/QGoDBTraceManager.h
  DBManager/QGoDBContourManager.h
  DBManager/QGoDBMeshManager.h
  DBManager/QGoDBTrackManager.h
  DBManager/QGoDBLineageManager.h

  # for the tracks
  QGoTrackViewDockWidget.h
  QGoTrackEditingWidget.h

  # for the lineages:
  QGoLineageViewDockWidget.h
  
  #--manual segmentation--
  TraceEditing/QGoContourManualSegmentation.h
  TraceEditing/QGoContourManualSegmentationWidget.h
  TraceEditing/QGoFilterChanAndVese.h
  TraceEditing/QGoFilterShape.h
  TraceEditing/QGoFilterWatershed.h
  TraceEditing/QGoFilterWatershed.h
  TraceEditing/QGoTraceEditingWidget.h
  TraceEditing/QGoModesManagerWidget.h
  TraceEditing/QGoAlgorithmsManagerWidget.h
  TraceEditing/QGoAlgorithmWidget.h
  TraceEditing/QGoMeshEditingWidgetManager.h
  TraceEditing/QGoContourEditingWidgetManager.h
  TraceEditing/QGoSegmentationAlgo.h
  TraceEditing/QGoSemiAutoSegmentationAlgo.h
  TraceEditing/QGoSplitSegmentationAlgo.h
  TraceEditing/QGoTraceEditingWidgetManager.h
  TraceEditing/QGoFilterSemiAutoBase.h

  # transfer function widget
  TransferFunctionEditor/QGoTransferFunctionDockWidget.h
  TransferFunctionEditor/GoTransferFunctionEditorWidget.h
  TransferFunctionEditor/GoTransferFunctionWidget.h
  TransferFunctionEditor/hoverpoints.h

  # lineage viewer
  LineageViewer/QGoLineageViewerWidget.h
)

if( BUILD_COMPARETOOL )
  set( QGoGUILIB_HDRS
    ${QGoGUILIB_HDRS}
    SynchronizedViews/QGoSynchronizedView.h
    SynchronizedViews/QGoSynchronizedView2D.h
    SynchronizedViews/QGoSynchronizedView3D.h
    SynchronizedViews/QGoSynchronizedView2DCallbacks.h
    SynchronizedViews/QGoSynchronizedView3DCallbacks.h
    SynchronizedViews/QGoSynchronizedViewManager.h
  )
endif( BUILD_COMPARETOOL )
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

# ADD THIS HEADERS IF ENABLE_VIDEO_RECORD_FFMPEG IS ON
if( ENABLE_VIDEO_RECORD_FFMPEG )
  set(QGoGUILIB_HDRS
    ${QGoGUILIB_HDRS}
    Video/QGoVideoRecorder.h
  )
endif( ENABLE_VIDEO_RECORD_FFMPEG )

# ADD THIS HEADERS IF ENABLE_VIDEO_RECORD_AVI IS ON
if( ENABLE_VIDEO_RECORD_AVI )
  set(QGoGUILIB_HDRS
    ${QGoGUILIB_HDRS}
    Video/QGoVideoRecorder.h
  )
endif( ENABLE_VIDEO_RECORD_AVI )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

set( QGoGUILIB_UI

#base dockwidget
  Resources/SegmentationDockWidgetBase.ui

#segmentation widget
  #Resources/SegmentationSeedBaseWidget.ui

# manual contour segmentation dock widget
  Resources/ManualSegmentationSettingsDlg.ui
  Resources/ContourManualSegmentationWidget.ui

  # other
  Resources/NavigationDockWidget.ui
  Resources/TransferFunctionDockWidget.ui
  Resources/LsmToMegaExportDialog.ui
  Resources/QNameDescriptionInputDialog.ui

  # for tracks
  Resources/TrackEditingWidget.ui

  # lineage viewer
  LineageViewer/QGoLineageViewerWidget.ui
)

if( BUILD_COMPARETOOL )
  set( QGoGUILIB_UI
    ${QGoGUILIB_UI}
    Resources/QGoSynchronizedView.ui
  )
endif( BUILD_COMPARETOOL )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

# ADD THIS UI IF ENABLE_VIDEO_RECORD_FFMPEG IS ON
if( ENABLE_VIDEO_RECORD_FFMPEG )
  set(QGoGUILIB_UI
    ${QGoGUILIB_UI}
    Resources/NewVideoRecorderDockWidget.ui
  )
endif( ENABLE_VIDEO_RECORD_FFMPEG )

# ADD THIS UI IF ENABLE_VIDEO_RECORD_AVI IS ON
if( ENABLE_VIDEO_RECORD_AVI )
  set(QGoGUILIB_UI
    ${QGoGUILIB_UI}
    Resources/NewVideoRecorderDockWidget.ui
  )
endif( ENABLE_VIDEO_RECORD_AVI )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

QT4_WRAP_UI( QGoGUILIB_UI_H
  ${QGoGUILIB_UI}
)

QT4_WRAP_CPP( QGoGUILIB_MOC ${QGoGUILIB_HDRS}
  OPTIONS
    -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED -DBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION # see https://bugreports.qt-project.org/browse/QTBUG-22829
)

QT4_ADD_RESOURCES( QGoGUI_QRC
  ${QGoResourceFile}
)
set_source_files_properties( ${QGoGUILIB_SRC} PROPERTIES
  OBJECT_DEPENDS "${QGoGUILIB_UI_H}"
  )

set_source_files_properties( ${QGoGUILIB_MOC} PROPERTIES
  OBJECT_DEPENDS "${QGoGUILIB_UI_H}"
  )

add_library( QGoGui
  ${QGoGUILIB_SRC}
  ${QGoGUILIB_MOC}
  ${QGoGUI_QRC}
)

set( QGoGUI_LIBS
  ${ITK_LIBRARIES}
  QVTK
  QGoIO
  vtkRenderingAddOn2
  PoissonReconstruction
  ctk
  itkQt
  )

target_link_libraries( QGoGui
  ${QGoGUI_LIBS}
  )


set_target_properties( QGoGui PROPERTIES VERSION ${GOFIGURE2_LIB_VERSION} SOVERSION ${GOFIGURE2_LIB_VERSION} )

# Runtime
install( TARGETS QGoGui
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} NAMELINK_SKIP COMPONENT Libraries
  )

# Development
install( TARGETS QGoGui
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries NAMELINK_ONLY
  )

file( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
file( GLOB __source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/*.txx" )
file( GLOB __binary_file_h "${CMAKE_CURRENT_BINARY_DIR}/*.h" )
file( GLOB __binary_file_txx "${CMAKE_CURRENT_BINARY_DIR}/*.txx" )

file( GLOB __source_video_h "${CMAKE_CURRENT_SOURCE_DIR}/Video/*.h" )
file( GLOB __binary_video_h "${CMAKE_CURRENT_BINARY_DIR}/Video/*.h" )

file( GLOB __source_sync_h
  "${CMAKE_CURRENT_SOURCE_DIR}/SynchronizedViews/*.h"
)
file( GLOB __binary_sync_h
  "${CMAKE_CURRENT_BINARY_DIR}/SynchronizedViews/*.h"
)
file( GLOB __source_tracevisu_h
  "${CMAKE_CURRENT_SOURCE_DIR}/VisualizationTraceContainers/*.h" )
file( GLOB __source_trace_visu_txx
  "${CMAKE_CURRENT_SOURCE_DIR}/VisualizationTraceContainers/*.txx" )
file( GLOB __binary_tracevisu_h
  "${CMAKE_CURRENT_BINARY_DIR}/VisualizationTraceContainers/*.h" )
file( GLOB __binary_tracevisu_txx
  "${CMAKE_CURRENT_BINARY_DIR}/VisualizationTraceContainers/*.txx" )

file( GLOB __source_wizard_h
  "${CMAKE_CURRENT_SOURCE_DIR}/Wizard/*.h" )
file( GLOB __binary_tracevisu_h
  "${CMAKE_CURRENT_BINARY_DIR}/Wizard/*.h" )

install( FILES ${__source_file_h} ${__source_file_txx}
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

