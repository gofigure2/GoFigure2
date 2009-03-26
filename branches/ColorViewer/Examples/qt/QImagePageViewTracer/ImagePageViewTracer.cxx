#include <qapplication.h>

#include "QImagePageView4DTracer.h"

int main( int argc, char** argv )
{
  QApplication app(argc, argv);
  QImagePageView4DTracer form;
  form.setMinimumSize( 800, 600 );
  form.SetFileName( argv[1] );
  app.setMainWidget( &form );
  form.show( );

  return app.exec();
}
