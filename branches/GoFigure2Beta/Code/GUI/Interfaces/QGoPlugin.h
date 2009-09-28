#ifndef __QGoPlugin_h
#define __QGoPlugin_h

#include <vector>
#include <list>

#include <QString>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include <QWidget>

#include "PluginInformationBase.h"

/**
\class QGoPlugin
\brief Abstract class for any kind of GoFigure2 plugins
*/
class QGoPlugin
{
  public:
    QGoPlugin();
    virtual ~QGoPlugin();

    QString Name() const;
    QString Version() const;
    QString GoFigureCompatibilityVersion() const;
    QString Distributor() const;
    QString Copyright() const;
    QString License() const;
    QString Description() const;

    virtual void WriteSettings() = 0;
    virtual void ReadSettings() = 0;

    virtual std::vector< QAction* > Actions() = 0;
    virtual QMenu* Menu() = 0;
    virtual QToolBar* ToolBar() = 0;
    virtual QDockWidget* DockWidget() = 0;
    virtual QWidget* AdditionalWidget() = 0;

    virtual void OnTabActivated( const int& ) = 0;
    virtual void OnTabMoved( const int& , const int& ) = 0;
    virtual void OnTabClosed( const int& ) = 0;

  protected:
    PluginInformationBase m_Information;

    /** \brief */
    virtual void SetAllRequiredInformation( ) = 0;

  private:
    QGoPlugin( const QGoPlugin& );
    void operator = ( const QGoPlugin& );
};


Q_DECLARE_INTERFACE( QGoPlugin, "GoFigure2.QGoPlugin/1.0" )

#endif
