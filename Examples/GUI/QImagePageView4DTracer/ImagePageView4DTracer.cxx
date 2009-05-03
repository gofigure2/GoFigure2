#include <qapplication.h>

#include "QImagePageView4DTracer.h"

int main( int argc, char** argv )
{
  if( argc < 3 ) return EXIT_FAILURE;

  QApplication app(argc, argv);
  QImagePageView4DTracer form;
  form.setMinimumSize( 800, 600 );
  form.SetColorVizu( 0 );
  form.SetFileTypeToSerie( );
  form.SetSerieTypeToMegaCapture();
  // filename must come last
  form.SetFileName( argv[1] );
  app.setMainWidget( &form );
  form.show( );

  return app.exec();
}
