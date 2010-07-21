IF( WIN32 )
  IF( NOT CYGWIN )
    IF( NOT MINGW )
      IF( BUILD_SHARED_LIBS )
        ADD_DEFINITIONS( -DQGoIO_EXPORT )
      ENDIF( BUILD_SHARED_LIBS )
    ENDIF( NOT MINGW )
  ENDIF( NOT CYGWIN )
ENDIF( WIN32 )

CONFIGURE_FILE(${GOFIGURE2_SOURCE_DIR}/Code/IO/QGoIOConfigure.h.in
               ${GOFIGURE2_BINARY_DIR}/Code/IO/QGoIOConfigure.h @ONLY IMMEDIATE)

SET( QGOIO_SRC
  ContourMeshStructure.cxx
  ContourMeshStructureHelper.cxx
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
  GoDBNameDescRow.cxx
  GoDBBookmarkRow.cxx
  GoDBCellTypeRow.cxx
  GoDBSubCellTypeRow.cxx
  GoDBIntensityRow.cxx
  GoDBTableWidgetContainer.cxx
  GoDBExport.cxx
  GoDBImport.cxx
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
  ${QGOIO_SRC}
)

ADD_DEPENDENCIES( QGoIO vtkLSMReader )

TARGET_LINK_LIBRARIES( QGoIO
  ${QT_LIBRARIES}
  QGoIO
  vtkIO
  vtkImaging
  vtkLSMReader
  ITKCommon
  ITKIO
)

SET_TARGET_PROPERTIES( QGoIO PROPERTIES VERSION ${GOFIGURE2_LIB_VERSION} SOVERSION ${GOFIGURE2_LIB_VERSION} )

# Runtime
INSTALL( TARGETS QGoIO
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} NAMELINK_SKIP COMPONENT Libraries
  )
  
# Development
INSTALL( TARGETS QGoIO
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries NAMELINK_ONLY
  )

