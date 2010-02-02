SET( QGOGUILIB_EXAMPLE_SRC
  qgoimageview2d
  qgoimageview3d
  qgolutdialog
  qgotabelementbase
  qgotabimageviewelementbase
  qgotabimageview2d
  qgotabimageview3d
  qgotabimageview3dwt
  qgotabimageview4d
  qgomanualsegmentationsettingsdialog
  qgovisualizationdockwidget
  qgomanualsegmentationdockwidget
  contourmeshstructurehelpertest
  contourmeshstructurehelpertest2
  QGoWizardDBTest
  QGoPrintDatabaseTest
  qgocreatemeshdialogtest
  QGoLsmToMegaExportDialogTest
  vtkFFMPEGRenderWindowRecorderTest
)

#ADD QGoVideoRecorderTest if ENABLE_VIDEO_RECORD IS ON
IF( ENABLE_VIDEO_RECORD )
  SET(QGOGUILIB_EXAMPLE_SRC
    ${QGOGUILIB_EXAMPLE_SRC}
    QGoVideoRecorderTest
    vtkFFMPEGRenderWindowRecorderTest
    )
ENDIF( ENABLE_VIDEO_RECORD )

FOREACH( var ${QGOGUILIB_EXAMPLE_SRC} )

  ADD_EXECUTABLE( ${var} ${var} )
  TARGET_LINK_LIBRARIES( ${var}
    QGoGui
  )

ENDFOREACH( var ${QGOGUILIB_EXAMPLE_SRC} )

ADD_TEST( qgoimageview2dTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgoimageview2d
  ${TESTING_DATA_PATH}/Circle.png
  1
)

ADD_TEST( qgoimageview3dTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgoimageview3d
  ${TESTING_DATA_PATH}/Circle3D.mhd
  1
)

ADD_TEST( qgolutdialogTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgolutdialog
  1
)

ADD_TEST( qgotabelementbaseTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgotabelementbase
)

ADD_TEST( qgotabimageviewelementbaseTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgotabimageviewelementbase
)

ADD_TEST( qgotabimageview2dTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgotabimageview2d
  ${TESTING_DATA_PATH}/Circle.png
  1
)

ADD_TEST( qgotabimageview3dTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgotabimageview3d
  ${TESTING_DATA_PATH}/Circle3D.mhd
  1
)

ADD_TEST( qgomanualsegmentationsettingsdialogTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgomanualsegmentationsettingsdialog
  1
)

ADD_TEST( qgovisualizationdockwidgetTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgovisualizationdockwidget
  1
)

ADD_TEST( qgomanualsegmentationdockwidgetTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qgomanualsegmentationdockwidget
  1
)

ADD_TEST( contourmeshstructurehelperTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/contourmeshstructurehelpertest
)

ADD_TEST( contourmeshstructurehelperTest2
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/contourmeshstructurehelpertest2
)

ADD_TEST( QGoLsmToMegaExportDialogTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoLsmToMegaExportDialogTest
  1
)

# ADD THIS TEST IF ENABLE_VIDEO_RECORD IS ON
IF( ENABLE_VIDEO_RECORD )
ADD_TEST( QGoVideoRecorderTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/QGoVideoRecorderTest
  1
)

ADD_TEST( vtkFFMPEGRenderWindowRecorderTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vtkFFMPEGRenderWindowRecorderTest
  test.avi
)
ENDIF( ENABLE_VIDEO_RECORD )
