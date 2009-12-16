#include <QApplication>
#include <QTimer>
#include "QGoVisualizationDockWidget.h"
#include <iostream>

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cerr <<"qgovisualizationdockwidget requires 1 argument1:" <<std::endl;
    std::cerr <<"1-test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  QGoVisualizationDockWidget*
    dock2d = new QGoVisualizationDockWidget( 0, 2 );

  dock2d->show();

  QGoVisualizationDockWidget*
    dock3d = new QGoVisualizationDockWidget( 0, 3 );

  dock3d->show();

  QGoVisualizationDockWidget*
    dock4d = new QGoVisualizationDockWidget( 0, 4 );

  dock4d->show();

  QTimer* timer = new QTimer;
  timer->setSingleShot( true );
  QObject::connect( timer, SIGNAL( timeout() ), dock2d, SLOT( close() ) );
  QObject::connect( timer, SIGNAL( timeout() ), dock3d, SLOT( close() ) );
  QObject::connect( timer, SIGNAL( timeout() ), dock4d, SLOT( close() ) );

  if( atoi( argv[1] ) == 1 )
    {
    timer->start( 100 );
    }

  app.processEvents();

  int output = app.exec();

  app.closeAllWindows();

  delete dock2d;
  delete dock3d;
  delete dock4d;
  delete timer;

  return output;
}
