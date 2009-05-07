#include <qapplication.h>
#include "QGoImageInfo.h"

int main( int argc, char** argv )
{
  QApplication app(argc, argv);
  QGoImageInfo info;

  app.setMainWidget( &info );
  info.setDimension( 3 );
  info.setNumberOfChannels( 3 );
  info.show( );

  return app.exec();
}
