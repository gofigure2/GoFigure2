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
)

ADD_LIBRARY( QGoIO
  ${QGOIO_SRC}
)

TARGET_LINK_LIBRARIES( QGoIO
  QGoIO
  ${MYSQL_LIBRARY}  
  ${MYSQL_EXTRA_LIBRARIES}
  vtkIO
  vtkImaging
  vtkLSMReader
  ITKCommon
  ITKIO
)

INSTALL_TARGETS(/lib QGoIO)

INSTALL_FILES(/include "(\\.h|\\.txx)$")

INSTALL( TARGETS QGoIO
  ARCHIVE
  DESTINATION lib
  COMPONENT libraries
  )

