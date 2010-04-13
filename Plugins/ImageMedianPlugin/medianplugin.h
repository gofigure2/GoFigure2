#ifndef __medianplugin_h
#define __medianplugin_h

#include "QGoImageFilterPluginBase.h"
#include "QCellPreprocess.h"

class medianplugin : public QGoImageFilterPluginBase
{
  Q_OBJECT
  Q_INTERFACES( QGoImageFilterPluginBase )

  public:
    explicit medianplugin( QObject* iParent = 0x0 );
    virtual ~medianplugin();

    virtual std::vector< QAction* > Actions();
    virtual QToolBar* ToolBar();
    virtual QMenu* Menu();
    virtual QDockWidget* DockWidget();
    virtual QWidget* AdditionalWidget();

    virtual void WriteSettings() {}
    virtual void ReadSettings() {}

    virtual void OnTabActivated( const int& ) {}
    virtual void OnTabMoved( const int& , const int& ) {}
    virtual void OnTabClosed( const int& ) {}

    virtual std::list< GoFigure::TabDimensionType > TabElementCompatibility() const;

  protected:
    virtual void Process( );
    virtual void SetAllRequiredInformation();

    QCellPreprocess* preprocess;

  private:
    Q_DISABLE_COPY( medianplugin );
};

#endif
