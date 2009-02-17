#include <qapplication.h>
#include "QGoMainWindow.h"

int main( int argc, char** argv )
{
  QApplication app(argc, argv);
  QGoMainWindow form;

  if( argc == 2 )
    form.SetFileName( argv[1] );

  app.setMainWidget( &form );
  form.show( );

  return app.exec();
}
