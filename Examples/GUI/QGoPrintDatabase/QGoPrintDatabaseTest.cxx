#include "QGoPrintDatabase.h"


int main(int argc, char * argv[])
{
  if( argc != 4 )
    {
    return false;
    }

  QApplication app(argc, argv);

  QGoPrintDatabase win;
  win.Fill_Database("localhost","gofigure",
      "gofigure", argv[1], atoi(argv[2]), argv[3]);
  win.show();

  return app.exec();
}
