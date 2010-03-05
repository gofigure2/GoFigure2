#include <qapplication.h>
#include <iostream>
#include <QTimer>

#include "QGoImageInfo.h"

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    return EXIT_FAILURE;
    }

  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain("http://gofigure2.sourceforge.net");

  QGoImageInfo* info = new QGoImageInfo;

  const unsigned int Dim3 = 3;
  info->setDimension( Dim3 );
  info->setNumberOfChannels( Dim3 );
  std::vector< unsigned int > size3( Dim3, 512 );
  std::vector< float > spacing3( Dim3, static_cast< const float >( 0.1 ) );

  info->setSize( size3 );
  info->setSpacing( spacing3 );
  info->setMemory( 1234567890 );

  std::vector< unsigned int > ppos3( Dim3, 12 );
  std::vector< float > wpos3( Dim3, static_cast< const float >( 1.2 ) );
  info->setPixelPosition( ppos3 );
  info->setWorldPosition( wpos3 );

  std::vector< float > value( Dim3, 128. );
  info->setValue( value );

  info->show( );
  
  QTimer* timer = new QTimer;
  timer->setSingleShot( true );
  QObject::connect( timer, SIGNAL( timeout() ), info, SLOT( close() ) );

  if( atoi( argv[1] ) == 1 )
    {
    timer->start( 500 );
    }

  const unsigned int Dim4 = 4;

  info->setDimension( Dim4 );
  info->setNumberOfChannels( Dim3 );
  std::vector< unsigned int > size4( Dim4, 512 );
  std::vector< float > spacing4( Dim4, static_cast< const float >( 0.1 ) );

  info->setSize( size4 );
  info->setSpacing( spacing4 );
  info->setMemory( 1234567890 );

  info->setPixelPosition( ppos3 );
  info->setWorldPosition( wpos3 );
  info->setTimePoint( 0. );
  info->setValue( value );

  info->show( );

  if( atoi( argv[1] ) == 1 )
    {
    timer->start( 500 );
    }
  int output = app.exec();
  app.closeAllWindows();

  delete info;
  delete timer;

  return output;
}

