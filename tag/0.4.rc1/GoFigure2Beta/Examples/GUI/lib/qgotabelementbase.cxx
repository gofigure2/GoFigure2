#include <QApplication>

#include "QGoTabElementBase.h"

class QGoTabElementTestHelper : public QGoTabElementBase
{
  public:
    QGoTabElementTestHelper( QWidget* iParent = 0 ) :
      QGoTabElementBase( iParent ) {}
    virtual ~QGoTabElementTestHelper() {}

    virtual void WriteSettings() {}
    virtual void ReadSettings() {}

    virtual GoFigure::TabDimensionType GetTabDimensionType( ) const
    { return GoFigure::TWO_D; }

  protected:
    virtual void PopulateMenus( QObject* iPlugin )
      { (void) iPlugin; }
  private:
    QGoTabElementTestHelper( const QGoTabElementTestHelper& );
    void operator = ( const QGoTabElementTestHelper& );
};

int main( int argc, char** argv )
{
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  QGoTabElementTestHelper* test = new QGoTabElementTestHelper;
  test->ViewActions();
  test->DockWidget();
  test->AdditionalWidget();
  test->GetPluginActions();
  test->SetPluginActions( std::list< QAction* >() );

  delete test;

  return EXIT_SUCCESS;
}
