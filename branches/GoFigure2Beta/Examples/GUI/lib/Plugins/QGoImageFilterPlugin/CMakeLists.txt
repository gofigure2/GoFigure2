SET( IMAGEIDENTITY_PLUGIN_SRC identityplugin.cxx )
SET( IMAGEIDENTITY_PLUGIN_HDRS identityplugin.h )

QT4_WRAP_CPP( IMAGEIDENTITY_PLUGIN_MOC
  ${IMAGEIDENTITY_PLUGIN_HDRS} )

ADD_LIBRARY( ImageIdentityPlugin
  SHARED
  ${IMAGEIDENTITY_PLUGIN_MOC}
  ${IMAGEIDENTITY_PLUGIN_SRC}
  ${GOFIGURE2_SOURCE_DIR}/Code/GUI/lib/QGoImageFilterPluginBase.cxx
)

TARGET_LINK_LIBRARIES( ImageIdentityPlugin
  QGoLib
)

ADD_EXECUTABLE( identityimagepluginExample
  identitypluginMain.cxx
)


TARGET_LINK_LIBRARIES( identityimagepluginExample
  ImageIdentityPlugin
)

ADD_TEST( identityimagepluginTest
  ${EXECUTABLE_OUTPUT_PATH}/identityimagepluginExample
)