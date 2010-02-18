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
  ${LIBS_STYLE}
  ${QGOIO_SRC}
)

TARGET_LINK_LIBRARIES( QGoIO
  QGoIO
  ${QT_LIBRARIES}
  ITKIO
  ITKCommon
  vtkLSMReader
  vtkIO
  vtkCommon
  vtkFiltering
  vtkImaging
)

IF( NOT APPLE )
  INSTALL( TARGETS QGoIO
    ARCHIVE
    DESTINATION ${GOFIGURE2_LIB_INSTALL_DIR}
    COMPONENT libraries
    )
ENDIF( NOT APPLE )
#INSTALL_FILES(/include "(\\.h|\\.txx)$")
