#include <QApplication>

#include "QGoTabElementBase.h"

class QGoTabElementTestHelper : public QGoTabElementBase
{
  public:
    QGoTabElementTestHelper( QWidget* parent = 0 ) :
      QGoTabElementBase( parent ) {}
    virtual ~QGoTabElementTestHelper() {}

    virtual void WriteSettings() {}
    virtual void ReadSettings() {}

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
  delete test;

  return EXIT_SUCCESS;
}
