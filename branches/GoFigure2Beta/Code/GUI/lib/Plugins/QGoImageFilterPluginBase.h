#ifndef __QGoImageFilterPluginBase_h
#define __QGoImageFilterPluginBase_h

#include <vector>
#include <QtPlugin>

#include "PluginInformationBase.h"

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
    QGoImageFilterPluginBase();
    virtual ~QGoImageFilterPluginBase();

    void SetInput( vtkImageData* iInput );
    vtkImageData* GetOutput();

    virtual std::vector< QAction* > Actions() = 0;
    virtual QToolBar* ToolBar() = 0;
    virtual QDockWidget* DockWidget() = 0;
    virtual QWidget* AdditionalWidget() = 0;

    virtual void WriteSettings() = 0;
    virtual void ReadSettings() = 0;

    QString Name() const;
    QString Version() const;
    QString GoFigureCompatibilityVersion() const;
    QString Distributor() const;
    QString Copyright() const;
    QString License() const;
    QString Description() const;

  protected:
    vtkImageData* m_VTKInput;
    vtkImageData* m_VTKOutput;

    PluginInformationBase m_Information;

    virtual void SetAllRequiredInformation( ) = 0;
    virtual void Process( ) = 0;

  private:
    QGoImageFilterPluginBase( const QGoImageFilterPluginBase& );
    void operator = ( const QGoImageFilterPluginBase& );
};

Q_DECLARE_INTERFACE( QGoImageFilterPluginBase, "GoFigure2.QGoImageFilterPluingBase/1.0" )

#endif

