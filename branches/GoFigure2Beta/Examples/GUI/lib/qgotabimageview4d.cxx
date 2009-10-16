#include <QApplication>
#include <QMenuBar>
#include <QTimer>

#include "QGoTabImageView4D.h"
#include "itkMegaCaptureImport.h"

int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cerr <<"qgotabimageview4d requires 2 arguments:" <<std::endl;
    std::cerr <<"1-filename(.jpg)" <<std::endl;
    std::cerr <<"2-test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }

  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName( "MegasonLab" );
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  typedef itk::MegaCaptureImport MegaCaptureImportType;
  MegaCaptureImportType::Pointer
    importFileInfoList = MegaCaptureImportType::New();
  importFileInfoList->SetFileName( argv[1] );
  importFileInfoList->Update();

  QGoTabImageView4D* tab = new QGoTabImageView4D;
  tab->SetMultiFiles( importFileInfoList->GetOutput(), 0 );
  tab->Update();
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

//   tab->show();
  app.processEvents();

  int output = app.exec();

  app.closeAllWindows();

  delete tab;

  return output;
}