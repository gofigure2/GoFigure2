#include "medianplugin.h"

//#include <typeinfo>
#include <QDir>
#include <QPluginLoader>

int main( int argc, char** argv )
{
  (void) argc;
  (void) argv;
//   QDir PluginDir( "plugins/" );
// 
//   // Here open one image
// 
//   foreach( QString fileName, PluginDir.entryList( QDir::Files ) )
//     {
//     QPluginLoader loader( PluginDir.absoluteFilePath( fileName ) );
// 
//     QObject* plugin = loader.instance();
// 
//     if( plugin )
//       {
//       medianplugin* temp = dynamic_cast< medianplugin* >( plugin );
// 
//       if( temp )
//         {
//         std::cout <<"median plugin correctly loaded" <<std::endl;
// 
//         // process the image through the plugin
// 
//         // simulate the interaction with the interface
// 
//         // write the outputs
//         }
//       }
//     }

  return 0;
}
