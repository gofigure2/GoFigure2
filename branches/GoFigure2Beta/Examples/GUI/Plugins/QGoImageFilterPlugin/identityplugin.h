#ifndef __identityplugin_h
#define __identityplugin_h

#include "QGoImageFilterPluginBase.h"

class identityplugin : public QObject, public QGoImageFilterPluginBase
{
  Q_OBJECT
  Q_INTERFACES( QGoImageFilterPluginBase )

  public:
    identityplugin();
    ~identityplugin();

    virtual std::vector< QAction* > Actions();
    virtual QToolBar* ToolBar();
    virtual QMenu* Menu();
    virtual QDockWidget* DockWidget();
    virtual QWidget* AdditionalWidget();

    virtual void WriteSettings();
    virtual void ReadSettings();

    virtual void OnTabActivated( const int& ) {}
    virtual void OnTabMoved( const int& , const int& ) {}
    virtual void OnTabClosed( const int& ) {}

  protected:
    virtual void Process( );
    void SetAllRequiredInformation();
  private:
    identityplugin( const identityplugin& );
    void operator = ( const identityplugin& );
};

#endif
