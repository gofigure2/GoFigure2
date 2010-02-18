#include <qapplication.h>
#include "QGoLUTDialog.h"

int main( int argc, char** argv )
{
  QApplication app(argc, argv);
  QGoLUTDialog lut;

  lut.show( );

  int output = app.exec();
  app.closeAllWindows();

  return output;
}

