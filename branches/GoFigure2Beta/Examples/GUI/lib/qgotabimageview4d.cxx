#include <QApplication>

#include "QGoTabImageView4D.h"

int main( int argc, char** argv )
{
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  QGoTabImageView4D* tab = new QGoTabImageView4D;
  delete tab;

  return EXIT_SUCCESS;
}