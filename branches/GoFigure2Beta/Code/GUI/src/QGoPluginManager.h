#ifndef __QGoPluginManager_h
#define __QGoPluginManager_h

class QGoMainWindow;
class QMenu;
class QGoPlugin;

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
  void ApplySettings();

  /** \brief Returns list of menus
  */
  std::list< QMenu* > GetMenus( );

public slots:
  virtual void NotifyTabActivated(const int&);
  virtual void NotifyTabClosed(const int&);
  virtual void NotifyTabMoved(const int&, const int&);

protected:
  std::list< QGoPlugin* > m_PluginList;
};

#endif
