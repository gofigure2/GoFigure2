#ifndef __QGoPluginManager_h
#define __QGoPluginManager_h

class QGoMainWindow;
class QMenu;

#include <QObject>

class QGoPluginManager : public QObject
{
Q_OBJECT
public:
  QGoPluginManager( /*QGoMainWindow**/ );
  virtual ~QGoPluginManager();

  bool IsPluginAlreadyLoaded( const QString& iName );
  void LoadPlugin(const QString&);
  void LoadPlugins();
  bool pluginExists(const QString&);
  void applySettings();

  /**
  *	getMenus()
  *
  *	Returns list of menus
  */
  std::list< QMenu* > GetMenus( );

  /**
  *	getMenuActions()
  *
  *	Returns list of menu actions added by plugins to
  *	menu with MenuID \par id.
  */
//   ActionList getMainMenuActions(const QString& engine, MenuID id);

  /**
  *	getToolBars()
  *
  *	Returns list of toolbars added by plugins.
  */
//   ToolBarList getToolBars(const QString& engine);

  /**
  *	getDocks()
  *
  *	Returns list of dock widgets added by plugins
  */
//   QWidgetList getDocks(const QString& engine);

  /**
  */
//   ActionList getContextMenuActions(const QString& engine);

public slots:
  virtual void NotifyTabActivated(const int&);
  virtual void NotifyTabClosed(const int&);
#if QT_VERSION >= 0x040500
  virtual void NotifyTabMoved(const int&, const int&);
#endif

private:
  class Interior;
  Interior* m_InternalManager;
};

#endif
