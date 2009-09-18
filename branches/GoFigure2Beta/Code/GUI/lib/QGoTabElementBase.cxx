#include "QGoTabElementBase.h"

#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>

QGoTabElementBase::QGoTabElementBase( QWidget* parent ) : QWidget( parent )
{}

QGoTabElementBase::~QGoTabElementBase()
{}

std::vector< QMenu* > QGoTabElementBase::Menus()
{
  QMenu* ViewMenu = new QMenu( QObject::tr( "&View" ) );

  std::vector< QMenu* > oMenuVector;
  oMenuVector.push_back( ViewMenu );

  return oMenuVector;
}

std::list< QToolBar* > QGoTabElementBase::ToolBar()
{
  return std::list< QToolBar* >();
}
std::list< QDockWidget* > QGoTabElementBase::DockWidget()
{
  return std::list< QDockWidget* >();
}

std::list< QWidget* > QGoTabElementBase::AdditionalWidget()
{
  return std::list< QWidget* >();
}

void QGoTabElementBase::LoadPlugins()
{
//   foreach( QObject *plugin, QPluginLoader::staticInstances() )
//     {
//     populateMenus(plugin);
//     }
//
//   m_PluginsDir = QDir( qApp->applicationDirPath() );
//
// #if defined(Q_OS_WIN)
//   if( ( m_PluginsDir.dirName().toLower() == "debug" ) ||
//       ( m_PluginsDir.dirName().toLower() == "release" ) )
//     {
//     m_PluginsDir.cdUp();
//     }
//  #elif defined(Q_OS_MAC)
//   if( m_PluginsDir.dirName() == "MacOS" )
//     {
//     m_PluginsDir.cdUp();
//     m_PluginsDir.cdUp();
//     m_PluginsDir.cdUp();
//     }
//  #endif
//
//   m_PluginsDir.cd( "plugins" );
//
//   foreach( QString fileName, m_PluginsDir.entryList( QDir::Files ) )
//     {
//     QPluginLoader loader( m_PluginsDir.absoluteFilePath( fileName ) );
//     QObject* plugin = loader.instance();
//     if( plugin )
//       {
//       populateMenus( plugin );
//       m_PluginFileNames += fileName;
//       }
//     }
}

// void QGoTabElementBase::populateMenus( QObject *plugin )
// {
//   BrushInterface *iBrush = qobject_cast<BrushInterface *>(plugin);
//   if( iBrush )
//     {
//     addToMenu( plugin, iBrush->brushes(), brushMenu,
//       SLOT( changeBrush() ), brushActionGroup );
//     }

//   ShapeInterface *iShape = qobject_cast<ShapeInterface *>(plugin);
//   if( iShape )
//     {
//     addToMenu(plugin, iShape->shapes(), shapesMenu, SLOT(insertShape()));
//     }

//   FilterInterface *iFilter = qobject_cast<FilterInterface *>(plugin);
//   if( iFilter )
//     {
//     addToMenu(plugin, iFilter->filters(), filterMenu, SLOT(applyFilter()));
//     }
//  }


// QStatusBar* QGoTabElementBase::StatusBar()
// {
//   return 0;
// }
