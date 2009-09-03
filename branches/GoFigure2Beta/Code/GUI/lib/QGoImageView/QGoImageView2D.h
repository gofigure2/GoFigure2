#ifndef __QGoImageView2D_h
#define __QGoImageView2D_h

#include <QtGui/QWidget>
#include <QHBoxLayout>

class vtkImageData;
class vtkViewImage2DCollection;
class QVTKWidget;
class vtkEventQtSlotConnect;


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
    void Update();

    void setupUi( QWidget* parent );
    void retranslateUi(QWidget *parent);
  public slots:
    void yoyo() {}

  protected:
    vtkViewImage2DCollection*         m_Pool;
    QHBoxLayout*                      m_LayOut;
    QVTKWidget*                       m_QVTKWidgetXY;
    vtkImageData*                     m_Image;
    vtkEventQtSlotConnect*            m_VTKEventQtConnector;

};
#endif

