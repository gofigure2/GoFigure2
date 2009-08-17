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
    QueryDataBaseHelper.cxx
    GoDBCollectionOfTraces.cxx
    vtkPolyDataMySQLTextReader.cxx
    vtkPolyDataMySQLTextWriter.cxx
    )
# ENDIF( MYSQL_DB_SUPPORT )

ADD_LIBRARY( QGoIO
  ${QGOIO_SRC}
)
IF( NOT APPLE )
  INSTALL( TARGETS QGoIO
    ARCHIVE
    DESTINATION lib
    COMPONENT libraries
    )
ENDIF( NOT APPLE )
#INSTALL_FILES(/include "(\\.h|\\.txx)$")
