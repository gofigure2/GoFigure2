IF( WIN32 )
  IF( NOT CYGWIN )
    IF( NOT MINGW )
      IF( BUILD_SHARED_LIBS )
        ADD_DEFINITIONS( -Dctk_EXPORTS )
      ENDIF( BUILD_SHARED_LIBS )
    ENDIF( NOT MINGW )
  ENDIF( NOT CYGWIN )
ENDIF( WIN32 )

CONFIGURE_FILE( ${GOFIGURE2_SOURCE_DIR}/Code/ExternalCode/ctk/ctkConfigure.h.in
  ${GOFIGURE2_BINARY_DIR}/Code/ExternalCode/ctk/ctkConfigure.h @ONLY IMMEDIATE )

#Define sources
SET( ctk_SRCS
  ctkRangeSlider.cpp
  ctkDoubleSlider.cpp
  ctkDoubleRangeSlider.cpp
  ctkRangeSlider.h
  ctkDoubleSlider.h
  ctkDoubleRangeSlider.h
  )

#Headers that should run through moc
SET(ctk_MOC_SRCS
  ctkRangeSlider.h
  ctkDoubleSlider.h
  ctkDoubleRangeSlider.h
  )

 # Wrap
QT4_WRAP_CPP(MY_MOC_CPP ${ctk_MOC_SRCS})

ADD_LIBRARY(ctk
  ${ctk_SRCS}
  ${MY_MOC_CPP}
  )
  
  INCLUDE(${QT_USE_FILE})
  ADD_DEFINITIONS(${QT_DEFINITIONS})

TARGET_LINK_LIBRARIES( ctk
  ${QT_LIBRARIES}
)

# Runtime
INSTALL( TARGETS ctk
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} NAMELINK_SKIP COMPONENT Libraries
  )
  
# Development
INSTALL( TARGETS ctk
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries NAMELINK_ONLY
  )