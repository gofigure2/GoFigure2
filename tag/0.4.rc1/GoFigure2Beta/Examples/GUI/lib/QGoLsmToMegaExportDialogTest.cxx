#include <QApplication>
#include <QTimer>

#include "QGoLsmToMegaExportDialog.h"

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cout <<"Usage : ./QGoLsmToMegaExportDialogTest " <<std::endl;
    std::cout << "1-test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }

  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  QTimer* timer = new QTimer;
  timer->setSingleShot( true );

  QGoLsmToMegaExportDialog* dlg =
    new QGoLsmToMegaExportDialog;

  QObject::connect( timer, SIGNAL( timeout() ), dlg, SLOT( accept() ) );

  if( atoi( argv[1] ) == 1 )
    {
    timer->start( 1000 );
    }

  dlg->show();

  app.processEvents();

  int output = app.exec();

  app.closeAllWindows();

  delete dlg;
  delete timer;

  return output;
}

