SET( QGOIO_SRC
  itkMegaCaptureImport.cxx
  itkLsm3DSerieImport.cxx
  itkMultiFileReader.cxx
# )
#
# IF( MYSQL_DB_SUPPORT )
#   LIST( APPEND
#     ${QGOIO_SRC}
    CreateDataBaseHelper.cxx
    GoDBCollectionOfTraces.cxx
    vtkPolyDataMySQLTextReader.cxx
    vtkPolyDataMySQLTextWriter.cxx
    )
# ENDIF( MYSQL_DB_SUPPORT )

ADD_LIBRARY( QGoIO
  ${QGOIO_SRC}
)
