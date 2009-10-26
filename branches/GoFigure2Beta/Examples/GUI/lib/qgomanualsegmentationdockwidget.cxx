#include <QApplication>
#include <QTimer>

#include "QGoManualSegmentationDockWidget.h"

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cerr <<"qgomanualsegmentationdockwidget requires 1 argument:" <<std::endl;
    std::cerr <<"1-test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }

  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  QGoManualSegmentationDockWidget* dock = new QGoManualSegmentationDockWidget;
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

  std::cout <<dock->GetMeshId() <<std::endl;
  QColor color = dock->GetValidatedColor();

  std::cout <<color.red() <<" " <<color.green() <<" " <<color.blue() <<std::endl;
  std::cout <<color.hue() <<" " <<color.saturation() <<" " <<color.value() <<std::endl;

  delete dock;

  return output;
}
