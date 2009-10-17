#include <QApplication>
#include <QTimer>
#include <QMenuBar>

#include "vtkMetaImageReader.h"
#include "vtkImageData.h"

#include "QGoTabImageView3DwT.h"

int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cerr <<"qgotabimageview3d requires 2 arguments:" <<std::endl;
    std::cerr <<"1-filename" <<std::endl;
    std::cerr <<"2-test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  vtkLSMReader* reader = vtkLSMReader::New();
  reader->SetFileName( argv[1] );
  reader->SetUpdateTimePoint( 0 );
  reader->Update();

  QGoTabImageView3DwT* tab = new QGoTabImageView3DwT;
  tab->SetLSMReader( reader, 0 );
  tab->Update();
  tab->show();

//   tab->SetTimePoint( 1 );
//   tab->SetTimePoint( 2 );

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

  std::list< QDockWidget* > dockwidget_list = tab->DockWidget();

  for( std::list< QDockWidget* >::iterator it = dockwidget_list.begin();
    it != dockwidget_list.end();
    it++ )
    {
    (*it)->show();
    QObject::connect( timer, SIGNAL( timeout() ), (*it), SLOT( close() ) );
    }

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
