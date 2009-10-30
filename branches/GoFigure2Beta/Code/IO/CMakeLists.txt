#QT4_WRAP_CPP( QGoThreadedMultiFileReader_MOC
#  QGoThreadedMultiFileReader.h
#)

FILE( GLOB QGOIO_SRC *.cxx )

#SET( QGOIO_SRC
#  itkMegaCaptureImport.cxx
#  itkLsm3DSerieImport.cxx
#  itkMultiFileReader.cxx
## )
##
## IF( MYSQL_DB_SUPPORT )
##   LIST( APPEND
##     ${QGOIO_SRC}
#    CreateDataBaseHelper.cxx
#    QueryDataBaseHelper.cxx
#    GoDBCollectionOfTraces.cxx
#    vtkPolyDataMySQLTextReader.cxx
#    vtkPolyDataMySQLTextWriter.cxx
#    )
## ENDIF( MYSQL_DB_SUPPORT )

ADD_LIBRARY( QGoIO
  ${QGOIO_SRC}
#  ${QGoThreadedMultiFileReader_MOC}
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
