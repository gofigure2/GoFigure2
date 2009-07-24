#include "QGoPrintDatabase.h"


int main(int argc, char * argv[])
{
 /* if( argc != 2 )
    {
    return false;
    }*/

  QApplication app(argc, argv);

  QGoPrintDatabase win;
 // win.FillTableFromDatabase(argv[1],"localhost","gofigure",
  //    "gofigure",atoi(argv[2]), argv[3]);

  win.FillTableFromDatabase("db23july","localhost","gofigure",
   "gofigure",3, "megacapture+contour");

  win.show();

  return app.exec();
}
