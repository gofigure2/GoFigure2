#ifndef __QGoImageSegmentationPluginBase_h
#define __QGoImageSegmentationPluginBase_h

#include <vector>
#include <QtPlugin>

class vtkImageData;
class vtkPolyData;

class QAction;
class QToolBar;
class QDockWidget;
class QWidget;

#include "QGoPlugin.h"

/**
 * \class QGoImageSegmentationPluginBase
 * \brief
*/
class QGoImageSegmentationPluginBase : public QGoPlugin
{
  public:
    typedef vtkPolyData                     SubCellularType;

    typedef std::vector< SubCellularType* > SubCellularVectorType;
    typedef SubCellularVectorType::iterator SubCellularVectorIterator;

    typedef SubCellularType                 CellType;

    typedef std::vector< CellType >     CellVectorType;
    typedef CellVectorType::iterator    CellVectorIterator;

    QGoImageSegmentationPluginBase();
    virtual ~QGoImageSegmentationPluginBase();

    virtual void SetInput( vtkImageData* iInput );
    virtual void Update();
    virtual CellVectorType GetOutput();

  protected:
    vtkImageData*   m_VTKInput;
    CellVectorType  m_Output;

    virtual void Process( ) = 0;

  private:
    QGoImageSegmentationPluginBase( const QGoImageSegmentationPluginBase& );
    void operator = ( const QGoImageSegmentationPluginBase& );
};

Q_DECLARE_INTERFACE( QGoImageSegmentationPluginBase,
  "GoFigure2.QGoImageSegmentationPluginBase/1.0" )

#endif

