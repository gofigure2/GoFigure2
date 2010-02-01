#include <QApplication>
#include <QTimer>

#include "QGoVideoRecorderFFMPEG.h"

//**************************************************************************//
//                               MAIN                                       //
//**************************************************************************//

int main( int argc, char *argv[] )
  {
	if( argc != 2 )
	    {
	    std::cout <<"Usage : ./VideoRecorderTest " <<std::endl;
	    std::cout << "1-test (boolean)" <<std::endl;
	    return EXIT_FAILURE;
	    }

  QApplication app(argc, argv);
  QTimer* timer = new QTimer;
    timer->setSingleShot( true );

  QGoVideoRecorderFFMPEG window(NULL);

  QObject::connect( timer, SIGNAL( timeout() ), &window, SLOT( close() ) );

    if( atoi( argv[1] ) == 1 )
      {
      timer->start( 1000 );
      }

  window.show();

  app.processEvents();
  app.exec();

  app.closeAllWindows();

  delete timer;

  return 0;
  }
