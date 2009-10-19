#include <QApplication>
#include <QTimer>
#include "QGoChannelVisualizationDockWidget.h"
#include <iostream>

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cerr <<"qgochannelvisualizationdockwidget requires 1 argument1:" <<std::endl;
    std::cerr <<"1-test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  QGoChannelVisualizationDockWidget*
    dock = new QGoChannelVisualizationDockWidget;

  dock->show();

  QTimer* timer = new QTimer;
  timer->setSingleShot( true );
  QObject::connect( timer, SIGNAL( timeout() ), dock, SLOT( close() ) );

  if( atoi( argv[1] ) == 1 )
    {
    timer->start( 100 );
    }

  app.processEvents();

  int output = app.exec();

  app.closeAllWindows();

  return output;
}
