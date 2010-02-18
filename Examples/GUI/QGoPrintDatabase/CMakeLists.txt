
    ADD_EXECUTABLE(  QGoPrintDatabaseTest
    QGoPrintDatabaseTest.cxx
    )
    
  #link .exe with the Qt libraries:
  TARGET_LINK_LIBRARIES(  QGoPrintDatabaseTest 
    QMegaVTKAddOn2 
    ITKCommon
    ITKIO
    ITKBasicFilters
    ITKNumerics
    )

