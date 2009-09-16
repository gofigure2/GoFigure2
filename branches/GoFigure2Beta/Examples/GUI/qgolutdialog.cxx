#include <QApplication>
#include <QTimer>

#include "vtkLookupTable.h"

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
    timer->start( 1000 );
    }

  for( int i = 0; i < 10; i++ )
    {
    lut->ChangeLookupTable( i );
    vtkLookupTable* temp = lut->GetLookupTable();
    }

  int output = app.exec();
  app.closeAllWindows();

  delete lut;
  delete timer;

  return output;
}