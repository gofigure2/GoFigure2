set( QGOGUILIB_EXAMPLE_SRC
  qgoimageview2d
  qgoimageview3d
  QGoImageInfoTest
  qgolutdialog
  qgotabelementbase
  qgotabimageviewelementbase
  qgotabimageview2d
  qgotabimageview3d
  qgotabimageview3dwt
  qgomanualsegmentationsettingsdialog
  qgonavigationdockwidget
  QGoContourManualSegmentationDockWidgetTest
  ContourMeshContainerTest
  QGoWizardDBTest
  QGoPrintDatabaseTest
  QGoColorCodingDialogTest
  qgocreatemeshdialogtest
  QGoLsmToMegaExportDialogTest
  QGoLsmToMegaExportThreadTest
  QGoDBCreateBookmarkDialogTest
  QGoTraceSettingsWidgetTest
  QGoDBInitializationWizardTest
  qgotabimageview3dwt2
  QGoLevelset2DTest
  QGoDBInitCreateAuthorPageTest
  QGoDBInitCreateMicroscopePageTest
  QGoColorComboBoxTest
  QGoSegmentationAlgoTest
  #QGoTrackEditingWidgetTest
#  QGoTraceEditingWidgetTest
#  QGoAlgorithmsManagerWidgetTest
#  QGoAlgorithmWidgetTest
#  QGoModesManagerWidgetTest
#  QGoMeshEditingWidgetManagerTest
)

if( BUILD_COMPARETOOL )
  set( QGOGUILIB_EXAMPLE_SRC
    ${QGOGUILIB_EXAMPLE_SRC}
    qgosynchronizedview2dtest
    qgosynchronizedview3dtest
    qgosynchronizedviewmanagertest
    compareexample
    comparepipelineexample
  )
endif( BUILD_COMPARETOOL )
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

#ADD QGoVideoRecorderTest if ENABLE_VIDEO_RECORD IS ON
if( ENABLE_VIDEO_RECORD_FFMPEG )
  set(QGOGUILIB_EXAMPLE_SRC
    ${QGOGUILIB_EXAMPLE_SRC}
    QGoVideoRecorderTest
    vtkFFMPEGRenderWindowRecorderTest
    )
endif( ENABLE_VIDEO_RECORD_FFMPEG )

#ADD QGoVideoRecorderTest if ENABLE_VIDEO_RECORD IS ON
if( ENABLE_VIDEO_RECORD_AVI )
  set(QGOGUILIB_EXAMPLE_SRC
    ${QGOGUILIB_EXAMPLE_SRC}
    QGoVideoRecorderTest
    vtkAVIRenderWindowRecorderTest
    )
endif( ENABLE_VIDEO_RECORD_AVI )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

foreach( var ${QGOGUILIB_EXAMPLE_SRC} )

  add_executable( ${var} ${var} )
  target_link_libraries( ${var}
    QGoGui
    ${ITK_LIBRARIES}
  )

endforeach( var ${QGOGUILIB_EXAMPLE_SRC} )

add_test( qgoimageview2dTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgoimageview2d
  ${TESTING_DATA_PATH}/Circle.png
  1
)

add_test( qgoimageview3dTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgoimageview3d
  ${TESTING_DATA_PATH}/Circle3D.mhd
  1
)

add_test( QGoImageInfoTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoImageInfoTest
  1
)

add_test( qgolutdialogTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgolutdialog
  1
)

add_test( qgotabelementbaseTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgotabelementbase
)

add_test( qgotabimageviewelementbaseTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgotabimageviewelementbase
)

add_test( qgotabimageview2dTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgotabimageview2d
  ${TESTING_DATA_PATH}/Circle.png
  1
)

add_test( qgotabimageview3dTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgotabimageview3d
  ${TESTING_DATA_PATH}/Circle3D.mhd
  1
)

add_test( qgotabimageview3dwtTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgotabimageview3dwt
  ${TTestESTING_DATA_PATH}/earpax2isl11.lsm
  1
)

add_test( qgomanualsegmentationsettingsdialogTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgomanualsegmentationsettingsdialog
  1
)

add_test( qgonavigationdockwidgetTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgonavigationdockwidget
  1
)

add_test( QGoContourManualSegmentationDockWidgetTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoContourManualSegmentationDockWidgetTest
  1
)

add_test( ContourMeshContainerTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ContourMeshContainerTest
)

add_test( QGoLsmToMegaExportDialogTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoLsmToMegaExportDialogTest
  1
)

add_test( QGoLsmToMegaExportThreadTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoLsmToMegaExportThreadTest
  earpax2isl11.lsm
  ${TESTING_DATA_PATH}/earpax2isl11.lsm
  ${GOFIGURE2_BINARY_DIR}/Testing/earpax2isl11/
  1
)

add_test( QGoWizardDBTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoWizardDBTest
  1
)

add_test( QGoPrintDatabaseTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoPrintDatabaseTest
  1
)

add_test( QGoDBCreateBookmarkDialogTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoDBCreateBookmarkDialogTest
  1
)

add_test( qgotabimageview3dwtTest2
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgotabimageview3dwt2
  ${GOFIGURE2_BINARY_DIR}/Testing/earpax2isl11/earpax2isl11.lsm-PL00-CO00-RO00-ZT00-YT00-XT00-TM0000-ch00-zs0000.png
)

set_tests_properties( qgotabimageview3dwtTest2
  PROPERTIES DEPENDS
  lsmtomegacaptureTest
)

if( BUILD_COMPARETOOL )
  add_test( qgosynchronizedview2dtest
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgosynchronizedview2dtest
    ${TESTING_DATA_PATH}/Circle.png
    1
  )

  add_test( qgosynchronizedview3dtest
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgosynchronizedview3dtest
    ${TESTING_DATA_PATH}/Circle3D.mhd
    1
  )

  add_test( qgosynchronizedviewmanagertest
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgosynchronizedviewmanagertest
    ${TESTING_DATA_PATH}/Circle.png
    ${TESTING_DATA_PATH}/Circle3D.mhd
    1
  )
endif( BUILD_COMPARETOOL )

add_test( QGoLevelset2DTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoLevelset2DTest
  ${TESTING_DATA_PATH}/Circle3D.mhd
  1
)

add_test( QGoDBInitCreateAuthorPageTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoDBInitCreateAuthorPageTest
  1
)

add_test( QGoDBInitCreateMicroscopePageTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoDBInitCreateMicroscopePageTest
  1
)

add_test( QGoSegmentationAlgoTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoSegmentationAlgoTest
  ${TESTING_DATA_PATH}/Circle3D.mhd
  1
)

#add_test( QGoTrackEditingWidgetTest
#  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoTrackEditingWidgetTest
#  1
#)

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

# ADD THIS TEST IF ENABLE_VIDEO_RECORD_FFMPEG IS ON
if( ENABLE_VIDEO_RECORD_FFMPEG )
  add_test( QGoVideoRecorderTest
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoVideoRecorderTest
    1
  )

  add_test( vtkFFMPEGRenderWindowRecorderTest
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkFFMPEGRenderWindowRecorderTest
    test.avi
  )
endif( ENABLE_VIDEO_RECORD_FFMPEG )

# ADD THIS TEST IF ENABLE_VIDEO_RECORD_AVI IS ON
if( ENABLE_VIDEO_RECORD_AVI )
  add_test( QGoVideoRecorderTest
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoVideoRecorderTest
    1
  )

  add_test( vtkAVIRenderWindowRecorderTest
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkAVIRenderWindowRecorderTest
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test.avi
  )
endif( ENABLE_VIDEO_RECORD_AVI )

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------
