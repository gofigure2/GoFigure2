if( WIN32 )
  if( NOT CYGWIN )
    if( NOT MINGW )
      if( BUILD_SHARED_LIBS )
        add_definitions( -Dctk_EXPORTS )
      endif( BUILD_SHARED_LIBS )
    endif( NOT MINGW )
  endif( NOT CYGWIN )
endif( WIN32 )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ctkConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/ctkConfigure.h
  @ONLY IMMEDIATE
)

#Define sources
set( ctk_SRCS
  ctkRangeSlider.cpp
  ctkDoubleSlider.cpp
  ctkDoubleRangeSlider.cpp
  ctkCollapsibleGroupBox.cpp
  ctkRangeSlider.h
  ctkDoubleSlider.h
  ctkDoubleRangeSlider.h
  ctkCollapsibleGroupBox.h
  )

#Headers that should run through moc
set(ctk_MOC_SRCS
  ctkRangeSlider.h
  ctkDoubleSlider.h
  ctkDoubleRangeSlider.h
  ctkCollapsibleGroupBox.h
  )

 # Wrap
QT4_WRAP_CPP(MY_MOC_CPP ${ctk_MOC_SRCS})

add_library(ctk
  ${ctk_SRCS}
  ${MY_MOC_CPP}
  )

target_link_libraries( ctk
  ${QT_LIBRARIES}
)

set_target_properties( ctk PROPERTIES VERSION ${GOFIGURE2_LIB_VERSION} SOVERSION ${GOFIGURE2_LIB_VERSION} )

# Runtime
install( TARGETS ctk
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} NAMELINK_SKIP COMPONENT Libraries
  )

# Development
install( TARGETS ctk
  EXPORT GoFigure2Targets
  RUNTIME DESTINATION ${GOFIGURE2_INSTALL_BIN_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries
  LIBRARY DESTINATION ${GOFIGURE2_INSTALL_LIB_DIR} COMPONENT Libraries NAMELINK_ONLY
  )

file( GLOB __source_file_h "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
file( GLOB __binary_file_h "${CMAKE_CURRENT_BINARY_DIR}/*.h" )

install( FILES ${__source_file_h} ${__binary_file_h}
  DESTINATION ${GOFIGURE2_INSTALL_HEADER_DIR}
  COMPONENT Development
)
