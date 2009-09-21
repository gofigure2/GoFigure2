SET( IMAGEIDENTITY_PLUGIN_SRC identityplugin.cxx )
SET( IMAGEIDENTITY_PLUGIN_HDRS identityplugin.h )

QT4_WRAP_CPP( IMAGEIDENTITY_PLUGIN_MOC
  ${IMAGEIDENTITY_PLUGIN_HDRS} )

ADD_EXECUTABLE( identityimagepluginExample
  identitypluginMain.cxx
  ${IMAGEIDENTITY_PLUGIN_MOC}
  ${IMAGEIDENTITY_PLUGIN_SRC}
  ${QGOPLUGIN_SRC}
)

TARGET_LINK_LIBRARIES( identityimagepluginExample
  QGoLib
)

ADD_TEST( identityimagepluginTest
  ${EXECUTABLE_OUTPUT_PATH}/identityimagepluginExample
)