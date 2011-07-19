IF( WIN32 )
  IF( NOT CYGWIN )
    IF( NOT MINGW )
      IF( BUILD_SHARED_LIBS )
        ADD_DEFINITIONS( -DQGoIO_EXPORT )
      ENDIF( BUILD_SHARED_LIBS )
    ENDIF( NOT MINGW )
  ENDIF( NOT CYGWIN )
ENDIF( WIN32 )

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/QGoIOConfigure.h.in
 ${CMAKE_CURRENT_BINARY_DIR}/QGoIOConfigure.h
 @ONLY IMMEDIATE
)

SET( QGOIO_SRC
  TraceStructure.cxx
  ContourMeshStructure.cxx
  TrackStructure.cxx
  LineageStructure.cxx
  CreateDataBaseHelper.cxx
  GoDBTableWidgetContainer.cxx
  GoDBTWContainerForContourMesh.cxx
  GoDBTWContainerForTrackLineage.cxx
  GoDBTWContainerForMesh.cxx
  GoDBTWContainerForTrack.cxx
  GoDBTWContainerForLineage.cxx
  GoDBExport.cxx
  GoDBImport.cxx
  GoDBCollectionOfTraces.cxx
  
  # GoDB*Row
  GoDBRow/GoDBAuthorRow.cxx
  GoDBRow/GoDBChannelRow.cxx
  GoDBRow/GoDBColorRow.cxx
  GoDBRow/GoDBContourRow.cxx
  GoDBRow/GoDBCoordinateRow.cxx
  GoDBRow/GoDBImageRow.cxx
  GoDBRow/GoDBImgSessionRow.cxx
  GoDBRow/GoDBMeshRow.cxx
  GoDBRow/GoDBProjectRow.cxx
  GoDBRow/GoDBTraceRow.cxx
  GoDBRow/GoDBRow.cxx
  GoDBRow/GoDBTrackRow.cxx
  GoDBRow/GoDBTrackFamilyRow.cxx
  GoDBRow/GoDBLineageRow.cxx
  GoDBRow/GoDBNameDescRow.cxx
  GoDBRow/GoDBBookmarkRow.cxx
  GoDBRow/GoDBCellTypeRow.cxx
  GoDBRow/GoDBSubCellTypeRow.cxx
  GoDBRow/GoDBIntensityRow.cxx

  # Mega related files
  GoImage/GoImageProcessor.cxx
  GoImage/GoMegaImageProcessor.cxx
  GoImage/GoLSMImageProcessor.cxx
  itkMegaCaptureImport.cxx
  itkMegaCaptureReader.cxx
  MegaCaptureHeaderReader.cxx

  QueryDataBaseHelper.cxx
  SelectQueryDatabaseHelper.cxx
  QueryBuilderHelper.cxx
  # itkLsm3DSerieImport.cxx

  GoFigureFileInfoMultiIndexContainerHelper.cxx
  vtkPolyDataMySQLContourReader.cxx
  vtkPolyDataMySQLMeshReader.cxx
  vtkPolyDataMySQLTrackReader.cxx
  vtkPolyDataMySQLContourWriter.cxx
  vtkPolyDataMySQLMeshWriter.cxx
  vtkPolyDataMySQLTrackWriter.cxx
  LSMToMegaCapture.cxx
#  MeshTextFileImport.cxx
#  TrackTextFileImport.cxx
)

SET_SOURCE_FILES_PROPERTIES(
  GoDBRow/GoDBRow.cxx
  GoDBRow/GoDBTraceRow.cxx
  GoDBTableWidgetContainer.cxx
  GoDBRow/GoDBNameDescRow.cxx
  ABSTRACT
  )

ADD_LIBRARY( QGoIO
  ${QGOIO_SRC}
)

ADD_DEPENDENCIES( QGoIO vtkLSMReader )

TARGET_LINK_LIBRARIES( QGoIO
  ${QT_LIBRARIES}
  QGoIO
  vtkHybrid
  vtkIO
  vtkImaging
  vtkLSMReader
  ${ITK_LIBRARIES}
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

FILE( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
FILE( GLOB __source_file_txx "${CMAKE_CURRENT_SOURCE_DIR}/*.txx" )

FILE( GLOB __binary_file_h "${CMAKE_CURRENT_BINARY_DIR}/*.h" )
FILE( GLOB __binary_file_txx "${CMAKE_CURRENT_BINARY_DIR}/*.txx" )

INSTALL( FILES ${__source_file_h} ${__source_file_txx}
    ${__binary_file_h} ${__binary_file_txx}
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)


