#include <QApplication>
#include <QTimer>
#include <QMenuBar>

#include "vtkPNGReader.h"
#include "vtkImageData.h"

#include "QGoTabImageView2D.h"

int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cerr <<"qgotabimageview2d requires 2 arguments:" <<std::endl;
    std::cerr <<"1-png filename" <<std::endl;
    std::cerr <<"2-test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  vtkPNGReader* reader = vtkPNGReader::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  QGoTabImageView2D* tab = new QGoTabImageView2D;
  tab->SetImage( reader->GetOutput() );
  tab->Update();
  tab->ActivateManualSegmentationEditor( true );
  tab->show();

  QMenuBar* menubar = new QMenuBar;
  std::vector< QAction* > action_vector = tab->ViewActions();

  for( std::vector< QAction* >::iterator q_it = action_vector.begin();
    q_it != action_vector.end();
    q_it++ )
    {
    menubar->addAction( *q_it );
    }
  menubar->show();

  QTimer* timer = new QTimer;
  timer->setSingleShot( true );
  QObject::connect( timer, SIGNAL( timeout() ), tab, SLOT( close() ) );
  QObject::connect( timer, SIGNAL( timeout() ), menubar, SLOT( close() ) );

  if( atoi( argv[2] ) == 1 )
    {
    timer->start( 1000 );
    }

  app.processEvents();

  int output = app.exec();

  app.closeAllWindows();

  delete menubar;
  delete tab;
  reader->Delete();

  return output;
}
