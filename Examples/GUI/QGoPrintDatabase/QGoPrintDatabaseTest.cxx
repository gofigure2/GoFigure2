#include "QGoPrintDatabase.h"


int main(int argc, char * argv[])
{
  if( argc != 3 )
    {
    return false;
    }

  QApplication app(argc, argv);

  QGoPrintDatabase win;
  win.Fill_Database("localhost","gofigure",
      "gofigure", argv[1], argv[2]);
  win.show();

  return app.exec();
}
