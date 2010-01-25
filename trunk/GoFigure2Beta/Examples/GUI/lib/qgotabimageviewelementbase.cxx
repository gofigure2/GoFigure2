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

public slots:
  virtual void ShowAllChannels( bool iChecked )
    {
    (void) iChecked;
    }
  virtual void ShowOneChannel( int iChannel )
    {
    (void) iChannel;
    }
protected:
  virtual void PopulateMenus( QObject* iPlugin )
    {
    (void) iPlugin;
    }
  virtual void GetBackgroundColorFromImageViewer( ) {}
  virtual void SetBackgroundColorToImageViewer( ) {}
  virtual void SetImageToImageViewer( vtkImageData* ) {}
  virtual int* GetImageCoordinatesFromWorldCoordinates( double iPos[3] )
    {
    (void) iPos;
    return 0;
    }

  virtual void RemoveActorFromViewer( const int& , vtkActor* )
    {}
  virtual void DisplayActorInViewer( const int& , vtkActor* )
    {}

  virtual std::vector< vtkActor* > AddContour( const int&,
      vtkPolyData*,
      vtkProperty* iProperty = NULL )
    {
    (void) iProperty;
    return std::vector< vtkActor* >();
    }

  virtual void SetSlice( int, int* ) {}
};

int main( int argc, char** argv )
{
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName( "MegasonLab" );
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );
  QCoreApplication::setApplicationName( "qgotabimageviewelementbase" );

  QGoTabImageViewElementBaseTestHelper* test =
    new QGoTabImageViewElementBaseTestHelper;
  test->SetColor( true );

  delete test;

  return EXIT_SUCCESS;
}
