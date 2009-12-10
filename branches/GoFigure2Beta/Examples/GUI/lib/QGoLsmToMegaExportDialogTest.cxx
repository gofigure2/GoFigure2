#include <QApplication>
#include <QTimer>

#include "QGoLsmToMegaExportDialog.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cout <<"Usage : qgomanualsegmentationsettingsdialog(.exe) " <<std::endl;
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

  dlg->show();

  app.processEvents();

  int output = app.exec();

  app.closeAllWindows();

  delete dlg;

  return output;
}

