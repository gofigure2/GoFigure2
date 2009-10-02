#ifndef __QGoImageView2D_h
#define __QGoImageView2D_h

#include <QtGui/QWidget>
#include <QHBoxLayout>

#include "SnapshotHelper.h"

#include "QGoImageView.h"

class vtkImageData;
class vtkViewImage2DCollection;
class QVTKWidget;
class vtkEventQtSlotConnect;


/**
\class QGoImageView2D
\brief Widget to visualize a 2D image (represented as vtkImageData).
*/
class QGoImageView2D : public QGoImageView
{
  Q_OBJECT
  public:
    explicit QGoImageView2D( QWidget* parent = 0 );
    ~QGoImageView2D();

    void SetImage( vtkImageData* iImage );
    void Update();

    void setupUi( QWidget* parent );
    void retranslateUi(QWidget *parent);

  public slots:
    void SetLookupTable( vtkLookupTable* iLut );
    void ShowScalarBar( const bool& );

    QString SnapshotViewXY( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName );

  protected:
    QHBoxLayout*                      m_LayOut;
    QVTKWidget*                       m_QVTKWidgetXY;
    vtkEventQtSlotConnect*            m_VTKEventQtConnector;
};
#endif

