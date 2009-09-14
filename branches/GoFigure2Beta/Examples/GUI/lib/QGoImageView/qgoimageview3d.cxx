#include <QApplication>
#include <QTimer>

#include "vtkMetaImageReader.h"
#include "vtkImageData.h"

#include "QGoImageView3D.h"

int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cout <<"Usage : qgoimageview2d(.exe) " <<std::endl;
    std::cout << "1-file.png" <<std::endl;
    std::cout << "2-test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  QGoImageView3D* viewer = new QGoImageView3D;

  vtkMetaImageReader* reader = vtkMetaImageReader::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  vtkImageData* image = reader->GetOutput();

  QTimer* timer = new QTimer;
  timer->setSingleShot( true );
  QObject::connect( timer, SIGNAL( timeout() ), viewer, SLOT( close() ) );

  viewer->SetImage( reader->GetOutput() );

  if( atoi( argv[2] ) == 1 )
    {
    timer->start( 2000 );
    }
  viewer->Update();
  viewer->show();

  app.processEvents();

  int output = app.exec();

  app.closeAllWindows();

  delete timer;
  delete viewer;

  return output;
}
