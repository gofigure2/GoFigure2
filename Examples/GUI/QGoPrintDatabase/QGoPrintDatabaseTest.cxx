#include "QGoPrintDatabase.h"


int main(int argc, char * argv[])
{
  //if( argc != 3 )
   // return false;

  QApplication app(argc, argv);

  QGoPrintDatabase win( QString("localhost"),QString("gofigure"),QString("gofigure"),QString(argv[1]));
  win.show();


  return app.exec();
}
