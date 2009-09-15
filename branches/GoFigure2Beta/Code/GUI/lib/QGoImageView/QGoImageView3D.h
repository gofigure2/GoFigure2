#ifndef __QGoImageView3D_h
#define __QGoImageView3D_h

#include <QtGui/QWidget>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QSplitter>
#include <QSlider>
#include <QSpacerItem>

#include "vtkImageData.h"
#include "vtkViewImage2D.h"
#include "vtkViewImage3D.h"
#include "QVTKWidget.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkViewImage2DCollection.h"

#include "QSplitterChild.h"

class QGoImageView3D : public QWidget
{
  Q_OBJECT
public:
  QGoImageView3D( QWidget* parent = 0 );
  ~QGoImageView3D();

  enum SnapshotImageType
    {
    BMP = 0,
    EPS,
    JPEG,
    PNG,
    TIFF
    };

  void SetImage( vtkImageData* iImage );
  void Update();

  void setupUi( QWidget* parent );
  void retranslateUi(QWidget *parent);

  int GetFullScreenView( ) const;

  void GetBackgroundColor( double& r, double& g, double& b );
  double* GetBackgroundColor();

  void SaveStateSplitters();

  int GetSliceViewXY() const;
  int GetSliceViewXZ() const;
  int GetSliceViewYZ() const;

public slots:
  QString SnapshotViewXY( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QString SnapshotView2( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QString SnapshotView3( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QString SnapshotViewXYZ( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );

  void SetSliceViewXY( const int& );
  void SetSliceViewXZ( const int& );
  void SetSliceViewYZ( const int& );

  void SetFullScreenView( const int& iS );
  void Quadview();
  void FullScreenViewXY();
  void FullScreenViewXZ();
  void FullScreenViewYZ();
  void FullScreenViewXYZ();

  void SetBackgroundColor( const double& r, const double& g, const double& b );
  void SetBackgroundColor( double rgb[3] );
  void SetBackgroundColor( const QColor& iColor );

protected:
  QSplitter*          VSplitter;
  QSplitterChild*     HtSplitter;
  QSplitterChild*     HbSplitter;

  QWidget*      LayOutWidget1;
  QHBoxLayout*  LayOut1;
  QSlider*      SliderXY;
  QVTKWidget*   QvtkWidget_XY;

  QWidget*      LayOutWidget2;
  QHBoxLayout*  LayOut2;
  QSlider*      SliderXZ;
  QVTKWidget*   QvtkWidget_XZ;

  QWidget*      LayOutWidget3;
  QHBoxLayout*  LayOut3;
  QSlider*      SliderYZ;
  QVTKWidget*   QvtkWidget_YZ;

  QWidget*      LayOutWidget4;
  QHBoxLayout*  LayOut4;
  QVTKWidget*   QvtkWidget_XYZ;
  QSpacerItem*  Spacer;

  vtkViewImage3D* View3D;

  vtkViewImage2DCollection* Pool;
  vtkImageData* m_Image;
  vtkEventQtSlotConnect* VtkEventQtConnector;
  unsigned int  m_SnapshotId;
  int           IsFullScreen;
  QString       m_Tag;

  virtual void resizeEvent( QResizeEvent* event );

  void SetupViewGivenQVTKWidget( vtkViewImage2D* iView, QVTKWidget* iWidget );
  void SetupVTKtoQtConnections();

  QString SnapshotView( QVTKWidget* iWidget,
    const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );

  bool BuildScreenshotFromImage( vtkImageData* image,
    vtkImageData* screenshot, int size = 0 );
  bool BuildScreenshotFromRenderWindow( vtkRenderWindow *win,
    vtkImageData* screenshot, int size = 0 );

protected slots:
  void MoveSliderXY();
  void MoveSliderXZ();
  void MoveSliderYZ();

private:
};
#endif
