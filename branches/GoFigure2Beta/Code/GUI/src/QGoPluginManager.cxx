#include "QGoPluginManager.h"

#include <iostream>

#include <QApplication>
#include <QDir>
#include <QMap>
#include <QPluginLoader>
#include <QAction>
#include <QDockWidget>
#include <QMainWindow>
#include <QToolBar>

#include "QGoPlugin.h"
#include "QGoMainWindow.h"

class QGoPluginManager::Interior
{
public:
  Interior()
  {
//     m_MainWindow = 0;
  }
  ~Interior() {}

  /** \todo check later on if it is already loaded. */
  bool AddPlugin( QGoPlugin* plugin )
  {
    m_Plugins.push_back( plugin );
    return true;
  }

//   QGoMainWindow* m_MainWindow;
  std::list< QGoPlugin* > m_Plugins;

  //	stores and manages dock windows (shows and hides when active engine is changed
  std::list< QDockWidget* > m_Docks;

  //	stores all plugins' context menu actions
  std::list< QAction* > m_ContextMenuActions;
};


QGoPluginManager::QGoPluginManager( /*QGoMainWindow* iMw*/ ) : QObject()
{
  m_InternalManager = new Interior();
//   m_InternalManager->m_MainWindow = iMw;
}

QGoPluginManager::~QGoPluginManager()
{
  delete m_InternalManager;
}

////////////////////////////////////////////////////////////
//	Plugins Events

void QGoPluginManager::NotifyTabActivated( const int& iId )
{
  for( std::list< QGoPlugin* >::iterator
        it = m_InternalManager->m_Plugins.begin();
       it != m_InternalManager->m_Plugins.end();
       ++it )
    {
    if( ( *it ) )
      {
      (*it)->OnTabActivated( iId );
      }
    }
}

void QGoPluginManager::NotifyTabClosed( const int& iId )
{
  for( std::list< QGoPlugin* >::iterator
        it = m_InternalManager->m_Plugins.begin();
       it != m_InternalManager->m_Plugins.end();
       ++it )
    {
    if( ( *it ) )
      {
      (*it)->OnTabClosed( iId );
      }
    }
}

#if QT_VERSION >= 0x040500
void QGoPluginManager::NotifyTabMoved( const int& from, const int& to )
{
  for( std::list< QGoPlugin* >::iterator
        it = m_InternalManager->m_Plugins.begin();
       it != m_InternalManager->m_Plugins.end();
       ++it )
    {
    if( ( *it ) )
      {
      (*it)->OnTabMoved( from, to );
      }
    }
}
#endif

////////////////////////////////////////////////////////////

bool QGoPluginManager::IsPluginAlreadyLoaded( const QString& iName )
{
  for( std::list< QGoPlugin* >::iterator
        it = m_InternalManager->m_Plugins.begin();
       it != m_InternalManager->m_Plugins.end();
       ++it )
    {
    if( ( *it ) )
      {
      if( ( *it )->Name() == iName )
        {
        return true;
        }
      }
    }
  return false;
}

void QGoPluginManager::LoadPlugin( const QString& path )
{
  QPluginLoader loader( path );
  if ( !loader.load() )
    {
    std::cout <<"Plugin was not loaded" <<std::endl;
    return;
    }

  QObject *obj = loader.instance();
  if( obj )
    {
    QGoPlugin* plugin = qobject_cast<QGoPlugin*>(obj);
    if ( plugin )
      {

      //	Check if plugin with the same name was already loaded.
      //	If is was then exit.
      if ( IsPluginAlreadyLoaded( plugin->Name() ) )
        {
        return;
        }

      if ( m_InternalManager->AddPlugin(plugin) )
        {
        std::cout <<"Plugin LOADED" <<std::endl;

        //	context menu actions
//         QString type = plugin->targetEngine();
//         m_InternalManager->m_ContextMenuActions[type] << plugin->contextMenuActions();

        //	settings page
//         m_InternalManager->m_MainWindow->addPluginSettingsPage(plugin->Name(), plugin->settingsPage());
        }
      else
        {
        loader.unload();
        }
      }
    else
      {
      std::cout <<"Error while casting" <<std::endl;
      }
    }
  else
    {
    std::cout <<"Null plugin" <<std::endl;
    }
}

void QGoPluginManager::LoadPlugins()
{
  //	user's plugins
//   QDir pluginDir( AppInfo::configDirPath() + "/plugins" );
//   foreach (QString fileName, pluginDir.entryList(QDir::Files))
//     {
//     QString path = pluginDir.absoluteFilePath(fileName);
//     LoadPlugin( path );
//     }

  //	global plugins
  QDir gPluginDir(QApplication::applicationDirPath() + "/plugins");
  foreach (QString fileName, gPluginDir.entryList(QDir::Files))
    {
    QString path = gPluginDir.absoluteFilePath(fileName);
    LoadPlugin(path);
    }

  std::list< QDockWidget* >::iterator it
    = m_InternalManager->m_Docks.begin();
  std::list< QDockWidget* >::iterator end
    = m_InternalManager->m_Docks.end();

  for( ; it != end; ++it )
    {
//     m_InternalManager->m_MainWindow->AddDockWidget( *it );
    }
}

void QGoPluginManager::applySettings()
{
  std::list< QGoPlugin* >::iterator it = m_InternalManager->m_Plugins.begin();
  std::list< QGoPlugin* >::iterator end = m_InternalManager->m_Plugins.end();

  for( ; it != end; ++it )
    {
//     (*it)->applySettings();
    }
}



////////////////////////////////////////////////////////////
//	GUI controls

std::list< QMenu* > QGoPluginManager::GetMenus( )
{
  std::list< QMenu* > oMenuList;

  std::list< QGoPlugin* >::iterator it = m_InternalManager->m_Plugins.begin();
  std::list< QGoPlugin* >::iterator end = m_InternalManager->m_Plugins.end();

  for( ; it!= end; ++it )
    {
    if( (*it)->Menu() )
      {
      oMenuList.push_back( (*it)->Menu() );
      }
    }

  return oMenuList;
}

// ActionList QGoPluginManager::GetMainMenuActions(const QString& engine, MenuID id)
// {
//   ActionList list;
//   foreach (QGoPlugin* plugin, m_InternalManager->m_Plugins[engine])
//     {
//     list << plugin->mainMenuActions(id);
//     }
//   return list;
// }

// ToolBarList QGoPluginManager::getToolBars(const QString& engine)
// {
//   ToolBarList list;
//   foreach (QGoPlugin* plugin, m_InternalManager->m_Plugins[engine])
//     {
//     if ( plugin->toolBar() )
//       {
//       list << plugin->toolBar();
//       }
//     }
//   return list;
// }

// QWidgetList QGoPluginManager::getDocks(const QString& engine)
// {
//   QWidgetList list;
//   foreach (QGoPlugin* plugin, m_InternalManager->m_Plugins[engine])
//     {
//     list << plugin->dockList();
//     }
//   return list;
// }

// ActionList QGoPluginManager::getContextMenuActions(const QString& engine)
// {
//   if ( m_InternalManager->m_ContextMenuActions.contains(engine) )
//     {
//     return m_InternalManager->m_ContextMenuActions[engine];
//     }
//   else
//     {
//     return ActionList();
//     }
// }

