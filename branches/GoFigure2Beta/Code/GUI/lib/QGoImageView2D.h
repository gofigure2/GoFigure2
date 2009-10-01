#ifndef __QGoImageView2D_h
#define __QGoImageView2D_h

#include <QtGui/QWidget>
#include <QHBoxLayout>

#include "SnapshotHelper.h"

class vtkImageData;
class vtkViewImage2DCollection;
class QVTKWidget;
class vtkEventQtSlotConnect;
class vtkLookupTable;

/**
\class QGoImageView2D
\brief Widget to visualize a 2D image (represented as vtkImageData).
*/
class QGoImageView2D : public QWidget
{
  Q_OBJECT
  public:
    explicit QGoImageView2D( QWidget* parent = 0 );
    virtual ~QGoImageView2D();

    void SetImage( vtkImageData* iImage );
    void SetColor( const bool& );
    void Update();

    void setupUi( QWidget* parent );
    void retranslateUi(QWidget *parent);

    void GetBackgroundColor( double& r, double& g, double& b );
    double* GetBackgroundColor();

  public slots:
    void SetBackgroundColor( const double& r, const double& g, const double& b );
    void SetBackgroundColor( double rgb[3] );
    void SetBackgroundColor( const QColor& iColor );

    void SetLookupTable( vtkLookupTable* iLut );
    void ShowScalarBar( const bool& );

    QString SnapshotViewXY( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName );

  protected:
    vtkViewImage2DCollection*         m_Pool;
    QHBoxLayout*                      m_LayOut;
    QVTKWidget*                       m_QVTKWidgetXY;
    vtkImageData*                     m_Image;
    vtkEventQtSlotConnect*            m_VTKEventQtConnector;
    unsigned int                      m_SnapshotId;

};
#endif

