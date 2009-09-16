#include <QApplication>

#include "QGoTabImageViewElementBase.h"

class QGoTabImageViewElementBaseTestHelper :
  public QGoTabImageViewElementBase
{
public:
  QGoTabImageViewElementBaseTestHelper( QWidget* parent = 0 )
    : QGoTabImageViewElementBase( parent ) {}
  virtual ~QGoTabImageViewElementBaseTestHelper() {}

  virtual void SetImage( vtkImageData* ) {}
  virtual void Update() {}

  virtual void WriteSettings() {}
  virtual void ReadSettings() {}
};

int main( int argc, char** argv )
{
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  QGoTabImageViewElementBaseTestHelper* test =
    new QGoTabImageViewElementBaseTestHelper;
  delete test;

  return EXIT_SUCCESS;
}