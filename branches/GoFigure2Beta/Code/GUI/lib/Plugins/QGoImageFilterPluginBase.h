#ifndef __QGoImageFilterPluginBase_h
#define __QGoImageFilterPluginBase_h

#include <vector>
#include <QtPlugin>

class vtkImageData;

class QAction;
class QToolBar;
class QDockWidget;
class QWidget;

/**
 * \class QGoImageFilterPluginBase
 * \brief
*/
class QGoImageFilterPluginBase
{
  public:
    QGoImageFilterPluginBase() : m_VTKInput( 0 ), m_VTKOutput( 0 ) {}
    virtual ~QGoImageFilterPluginBase() {}


    void SetInput( vtkImageData* iInput )
      { m_VTKInput = iInput; }
    vtkImageData* GetOutput()
      { return m_VTKOutput; }

    virtual std::vector< QAction* > Actions() = 0;
    virtual QToolBar* ToolBar() = 0;
    virtual QDockWidget* DockWidget() = 0;
    virtual QWidget* AdditionalWidget() = 0;

    virtual void WriteSettings() = 0;
    virtual void ReadSettings() = 0;

  protected:
    vtkImageData* m_VTKInput;
    vtkImageData* m_VTKOutput;

    virtual void Process( ) = 0;

  private:
    QGoImageFilterPluginBase( const QGoImageFilterPluginBase& );
    void operator = ( const QGoImageFilterPluginBase& );
};

Q_DECLARE_INTERFACE( QGoImageFilterPluginBase, "GoFigure2.QGoImageFilterPluingBase" )

#endif

