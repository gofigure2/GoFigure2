#ifndef __medianplugin_h
#define __medianplugin_h

#include "QGoImageFilterPluginBase.h"
#include "QCellPreprocess.h"

class medianplugin : public QObject, public QGoImageFilterPluginBase
{
  Q_OBJECT
  Q_INTERFACES( QGoImageFilterPluginBase )

  public:
    medianplugin();
    ~medianplugin();

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

    std::list< GoFigure::TabDimensionType > TabElementCompatibility() const;

  protected:
    virtual void Process( );
    void SetAllRequiredInformation();

    QCellPreprocess* preprocess;

  private:
    medianplugin( const medianplugin& );
    void operator = ( const medianplugin& );
};

#endif
