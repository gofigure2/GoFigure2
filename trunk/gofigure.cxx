#include <qapplication.h>
#include "QGoMainWindow.h"
#include <qsplashscreen.h>

int main( int argc, char** argv )
{
  QApplication app(argc, argv);
  
  QPixmap pixmap(":/fig/splash.jpg");
  QSplashScreen *splash = new QSplashScreen(pixmap,Qt::WindowStaysOnTopHint);

  splash->show();
 
  app.processEvents();
  splash->showMessage("Application loading...please wait");
  //splash.showMessage("Ready");
  

  QGoMainWindow form;
  if( argc == 2 )
    form.SetFileName( argv[1] );
 
  app.setMainWidget( &form );
  
  form.show( );
    
  splash->showMessage("Application ready");
 // app.processEvents();
  splash->finish(&form);
  delete splash;
  return app.exec();
}









/*establishConnections();
mainWin.show();
splash->finish(&mainWin);
delete splash;
return app.exec();*/
