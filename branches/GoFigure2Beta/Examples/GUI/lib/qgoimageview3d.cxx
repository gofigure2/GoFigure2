#include <QApplication>
#include <QTimer>

#include "vtkMetaImageReader.h"
#include "vtkImageData.h"

#include "QGoImageView3D.h"

int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cout <<"Usage : qgoimageview3d(.exe) " <<std::endl;
    std::cout << "1-file.mhd" <<std::endl;
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

  viewer->SetImage( image );
  viewer->Update();
  viewer->show();

  if( atoi( argv[2] ) == 1 )
    {
    timer->start( 1000 );

    viewer->FullScreenViewXY();
    if( viewer->GetFullScreenView() != 1 )
      {
      std::cerr <<"viewer->GetFullScreenView() = " <<viewer->GetFullScreenView();
      std::cerr <<" != 1" <<std::endl;
      app.closeAllWindows();

      reader->Delete();
      delete timer;
      delete viewer;
      return EXIT_FAILURE;
      }

    viewer->FullScreenViewXZ();
    if( viewer->GetFullScreenView() != 2 )
      {
      std::cerr <<"viewer->GetFullScreenView() = " <<viewer->GetFullScreenView();
      std::cerr <<" != 2" <<std::endl;
      app.closeAllWindows();

      reader->Delete();
      delete timer;
      delete viewer;
      return EXIT_FAILURE;
      }

    viewer->FullScreenViewYZ();
    if( viewer->GetFullScreenView() != 3 )
      {
      std::cerr <<"viewer->GetFullScreenView() = " <<viewer->GetFullScreenView();
      std::cerr <<" != 3" <<std::endl;
      app.closeAllWindows();

      reader->Delete();
      delete timer;
      delete viewer;
      return EXIT_FAILURE;
      }

    viewer->FullScreenViewXYZ();
    if( viewer->GetFullScreenView() != 4 )
      {
      std::cerr <<"viewer->GetFullScreenView() = " <<viewer->GetFullScreenView();
      std::cerr <<" != 4" <<std::endl;
      app.closeAllWindows();

      reader->Delete();
      delete timer;
      delete viewer;
      return EXIT_FAILURE;
      }

    viewer->Quadview();
    if( viewer->GetFullScreenView() != 0 )
      {
      std::cerr <<"viewer->GetFullScreenView() = " <<viewer->GetFullScreenView();
      std::cerr <<" != 0" <<std::endl;
      app.closeAllWindows();

      reader->Delete();
      delete timer;
      delete viewer;
      return EXIT_FAILURE;
      }

    int slice = viewer->GetSliceViewXY();
    viewer->SetSliceViewXY( slice + 1 );

    if( viewer->GetSliceViewXY() != slice + 1 )
      {
      std::cerr <<"viewer->GetSliceViewXY() = " <<viewer->GetSliceViewXY();
      std::cerr <<" != slice + 1" <<std::endl;
      app.closeAllWindows();

      reader->Delete();
      delete timer;
      delete viewer;
      return EXIT_FAILURE;
      }

    slice = viewer->GetSliceViewXZ();
    viewer->SetSliceViewXZ( slice - 1 );

    if( viewer->GetSliceViewXZ() != slice - 1 )
      {
      std::cerr <<"viewer->GetSliceViewXZ() = " <<viewer->GetSliceViewXZ();
      std::cerr <<" != slice - 1" <<std::endl;
      app.closeAllWindows();

      reader->Delete();
      delete timer;
      delete viewer;
      return EXIT_FAILURE;
      }

    slice = viewer->GetSliceViewYZ();
    viewer->SetSliceViewYZ( slice + 1 );

    if( viewer->GetSliceViewYZ() != slice + 1 )
      {
      std::cerr <<"viewer->GetSliceViewYZ() = " <<viewer->GetSliceViewYZ();
      std::cerr <<" != slice + 1" <<std::endl;
      app.closeAllWindows();

      reader->Delete();
      delete timer;
      delete viewer;
      return EXIT_FAILURE;
      }

    double r, g, b;
    viewer->SetBackgroundColor( 0.5, 0.5, 0.5 );
    viewer->GetBackgroundColor( r, g, b );
    if( r != 0.5 || g != 0.5 || b != 0.5 )
      {
      std::cerr <<r <<" " <<g <<" " <<b <<" != {0.5, 0.5, 0.5}" <<std::endl;
      app.closeAllWindows();

      reader->Delete();
      delete timer;
      delete viewer;
      return EXIT_FAILURE;
      }

    viewer->SetBackgroundColor( 0.1, 0.6, 0.7 );
    viewer->GetBackgroundColor( r, g, b );
    if( r != 0.1 || g != 0.6 || b != 0.7 )
      {
      std::cerr <<r <<" " <<g <<" " <<b <<" != {0.1, 0.6, 0.7}" <<std::endl;
      app.closeAllWindows();

      reader->Delete();
      delete timer;
      delete viewer;
      return EXIT_FAILURE;
      }
    viewer->SetBackgroundColor( 0., 0., 0. );
    viewer->GetBackgroundColor( r, g, b );
    if( r != 0. || g != 0. || b != 0. )
      {
      std::cerr <<r <<" " <<g <<" " <<b <<" != {0., 0., 0.}" <<std::endl;
      app.closeAllWindows();

      reader->Delete();
      delete timer;
      delete viewer;
      return EXIT_FAILURE;
      }
  }

  app.processEvents();

  int output = app.exec();

  app.closeAllWindows();

  reader->Delete();
  delete timer;
  delete viewer;

  return output;
}
