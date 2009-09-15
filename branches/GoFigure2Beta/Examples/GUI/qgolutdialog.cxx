#include <QApplication>
#include <QTimer>

#include "QGoLUTDialog.h"

int main( int argc, char** argv )
{
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  QGoLUTDialog* lut = new QGoLUTDialog;
  lut->show();

  QTimer* timer = new QTimer;
  timer->setSingleShot( true );
  QObject::connect( timer, SIGNAL( timeout() ), lut, SLOT( accept() ) );

  if( atoi( argv[1] ) == 1 )
    {
    timer->start( 2000 );
    }
  lut->ChangeLookupTable( 0 );
  lut->ChangeLookupTable( 1 );
  lut->ChangeLookupTable( 2 );
  lut->ChangeLookupTable( 3 );
  lut->ChangeLookupTable( 4 );
  lut->ChangeLookupTable( 5 );
  lut->ChangeLookupTable( 6 );
  lut->ChangeLookupTable( 7 );
  lut->ChangeLookupTable( 8 );
  lut->ChangeLookupTable( 9 );
  lut->ChangeLookupTable( 10 );

  int output = app.exec();
  app.closeAllWindows();

  return output;
}