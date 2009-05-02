#include <qapplication.h>
#include "QGoLUTDialog.h"

int main( int argc, char** argv )
{
  QApplication app(argc, argv);
  QGoLUTDialog lut;

  app.setMainWidget( &lut );
  lut.show( );

  return app.exec();
}
