#include "QGoCreateMeshDialog.h"


int main(int argc, char * argv[])
{
 /* if( argc != 2 )
    {
    return false;
    }*/

  QApplication app(argc, argv);

  QGoCreateMeshDialog win;
 
  win.show();
 
  return app.exec();
}
