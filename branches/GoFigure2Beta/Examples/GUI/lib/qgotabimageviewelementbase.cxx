#include <QApplication>

#include "QGoTabImageViewElementBase.h"

class QGoTabImageViewElementBaseTestHelper :
  public QGoTabImageViewElementBase
{
public:
  QGoTabImageViewElementBaseTestHelper( QWidget* iParent = 0 )
    : QGoTabImageViewElementBase( iParent ) {}
  virtual ~QGoTabImageViewElementBaseTestHelper() {}

  GoFigure::TabDimensionType GetTabDimensionType( ) const
    { return GoFigure::TWO_D; }

  virtual void Update() {}

  virtual void WriteSettings() {}
  virtual void ReadSettings() {}
protected:
  virtual void PopulateMenus( QObject* iPlugin )
    {
    (void) iPlugin;
    }
  virtual void GetBackgroundColorFromImageViewer( ) {}
  virtual void SetBackgroundColorToImageViewer( ) {}
  virtual void SetImageToImageViewer( vtkImageData* image )
    {
    (void) image;
    }
};

int main( int argc, char** argv )
{
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName( "MegasonLab" );
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );
  QCoreApplication::setApplicationName( "qgotabimageviewelementbase" );

  QGoTabImageViewElementBaseTestHelper* test =
    new QGoTabImageViewElementBaseTestHelper;
  delete test;

  return EXIT_SUCCESS;
}
