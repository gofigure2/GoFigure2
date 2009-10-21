#ifndef __QGoImageFilterPluginBase_h
#define __QGoImageFilterPluginBase_h

#include <vector>
#include <QtPlugin>

class vtkImageData;

class QAction;
class QToolBar;
class QDockWidget;
class QWidget;

#include "QGoPlugin.h"

/**
 * \class QGoImageFilterPluginBase
 * \brief
*/
class QGoImageFilterPluginBase : public QGoPlugin
{
  public:
    QGoImageFilterPluginBase();
    virtual ~QGoImageFilterPluginBase();

    virtual void SetInput( vtkImageData* iInput );
    virtual void Update();
    virtual vtkImageData* GetOutput();

  protected:
    vtkImageData* m_VTKInput;
    vtkImageData* m_VTKOutput;

    virtual void Process( ) = 0;

  private:
    QGoImageFilterPluginBase( const QGoImageFilterPluginBase& );
    void operator = ( const QGoImageFilterPluginBase& );
};

Q_DECLARE_INTERFACE( QGoImageFilterPluginBase,
  "GoFigure2.QGoImageFilterPluginBase/1.0" )

#endif

