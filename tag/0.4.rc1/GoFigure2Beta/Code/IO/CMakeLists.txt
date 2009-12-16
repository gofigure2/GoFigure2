SET( QGOIO_SRC
  CreateDataBaseHelper.cxx
  GoDBChannelRow.cxx
  GoDBCollectionOfTraces.cxx
  GoDBColorRow.cxx
  GoDBContourRow.cxx
  GoDBCoordinateRow.cxx
  GoDBImageRow.cxx
  GoDBImgSessionRow.cxx
  GoDBMeshRow.cxx
  GoDBProjectRow.cxx
  GoDBTraceRow.cxx
  GoDBRow.cxx
  GoDBTrackRow.cxx
  GoDBLineageRow.cxx
  GoDBTableWidgetContainer.cxx
  MegaCaptureHeaderReader.cxx
  QueryDataBaseHelper.cxx
  SelectQueryDatabaseHelper.cxx
  itkLsm3DSerieImport.cxx
  itkMegaCaptureImport.cxx
  itkMegaCaptureReader.cxx
  GoFigureFileInfoMultiIndexContainerHelper.cxx
#   itkMultiFileReader.cxx
  vtkPolyDataMySQLTextReader.cxx
  vtkPolyDataMySQLTextWriter.cxx
  LSMToMegaCapture.cxx
  MeshTextFileImport.cxx
  TrackTextFileImport.cxx
)

ADD_LIBRARY( QGoIO
  ${LIBS_STYLE}
  ${QGOIO_SRC}
)

TARGET_LINK_LIBRARIES( QGoIO
  QGoIO
  ${QT_LIBRARIES}
  ${MYSQL_LIBRARY}  
  ${MYSQL_EXTRA_LIBRARIES}
  vtkIO
  vtkImaging
  vtkLSMReader
  ITKCommon
  ITKIO
)

IF( NOT APPLE )
  INSTALL( TARGETS QGoIO
    ARCHIVE
    DESTINATION ${GOFIGURE2_LIB_INSTALL_DIR}
    COMPONENT libraries
    )
ENDIF( NOT APPLE )
#INSTALL_FILES(/include "(\\.h|\\.txx)$")
