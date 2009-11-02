SET( QGOIO_SRC
  CreateDataBaseHelper.cxx
  GoDBChannelRow.cxx
  GoDBCollectionOfTraces.cxx
  GoDBColorRow.cxx
  GoDBCoordinateRow.cxx
  GoDBImageRow.cxx
  GoDBImgSessionRow.cxx
  GoDBProjectRow.cxx
  GoDBRow.cxx
  itkLsm3DSerieImport.cxx
  itkMegaCaptureImport.cxx
  itkMultiFileReader.cxx
  MegaCaptureHeaderReader.cxx
  QueryDataBaseHelper.cxx
  SelectQueryDatabaseHelper.cxx
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

