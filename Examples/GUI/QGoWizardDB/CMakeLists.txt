
    ADD_EXECUTABLE(  QGoWizardDBTest
    QGoWizardDBTest.cxx
    )
    
  #link .exe with the Qt libraries:
  TARGET_LINK_LIBRARIES(  QGoWizardDBTest 
    QMegaVTKAddOn2
    QGoIO 
    ITKCommon
    ITKIO
    ITKBasicFilters
    ITKNumerics
    )

