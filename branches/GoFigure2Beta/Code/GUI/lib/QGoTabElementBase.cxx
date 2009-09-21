#include "QGoTabElementBase.h"

#include <QApplication>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include <QPluginLoader>

#include "QGoImageFilterPluginBase.h"

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

QDir QGoTabElementBase::DirectoryOf( const QString& iSubdir )
{
  QDir dir = QDir( QApplication::applicationDirPath() );

#if defined(Q_OS_WIN)
  if( ( dir.dirName().toLower() == "debug" ) ||
      ( dir.dirName().toLower() == "release" ) )
    {
    dir.cdUp();
    }
#elif defined(Q_OS_MAC)
  if( dir.dirName() == "MacOS" )
    {
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    }
#endif
  dir.cd( iSubdir );

  return dir;
}

void QGoTabElementBase::LoadPlugins()
{
  foreach( QObject *plugin, QPluginLoader::staticInstances() )
    {
    this->PopulateMenus( plugin );
    }

  m_PluginsDir = DirectoryOf( "plugins" );

  foreach( QString fileName, m_PluginsDir.entryList( QDir::Files ) )
    {
    QPluginLoader loader( m_PluginsDir.absoluteFilePath( fileName ) );
    QObject* plugin = loader.instance();
    if( plugin )
      {
      this->PopulateMenus( plugin );
      m_PluginFileNames += fileName;
      }
    }
}

void QGoTabElementBase::AddToMenu(
  QObject *plugin, const QStringList &texts,
  QMenu* menu, const char *member,
  QActionGroup *actionGroup )
{
  foreach( QString text, texts )
    {
    QAction *action = new QAction(text, plugin);
    connect(action, SIGNAL(triggered()), this, member);
    menu->addAction(action);

    if (actionGroup)
      {
      action->setCheckable(true);
      actionGroup->addAction(action);
      }
    }
}
// QStatusBar* QGoTabElementBase::StatusBar()
// {
//   return 0;
// }
