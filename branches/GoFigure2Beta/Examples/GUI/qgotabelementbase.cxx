#include "QGoTabElementBase.h"

class QGoTabElementTestHelper : public QGoTabElementBase
{
  public:
    QGoTabElementTestHelper( QWidget* parent = 0 ) :
      QGoTabElementBase( parent ) {}
    virtual ~QGoTabElementTestHelper() {}

    virtual void WriteSettings() {}
    virtual void ReadSettings() {}

  private:
    QGoTabElementTestHelper( const QGoTabElementTestHelper& );
    void operator = ( const QGoTabElementTestHelper& );
};

int main( int argc, char** argv )
{
  QGoTabElementTestHelper* test = new QGoTabElementTestHelper;
  delete test;

  return EXIT_SUCCESS;
}
