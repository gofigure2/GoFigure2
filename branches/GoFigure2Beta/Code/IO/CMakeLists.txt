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
  GoDBRow.cxx
  MegaCaptureHeaderReader.cxx
  QueryDataBaseHelper.cxx
  SelectQueryDatabaseHelper.cxx
  itkLsm3DSerieImport.cxx
  itkMegaCaptureImport.cxx
  itkMultiFileReader.cxx
  vtkPolyDataMySQLTextReader.cxx
  vtkPolyDataMySQLTextWriter.cxx
)

ADD_LIBRARY( QGoIO
  ${QGOIO_SRC}
)

TARGET_LINK_LIBRARIES( QGoIO
  QGoIO
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

