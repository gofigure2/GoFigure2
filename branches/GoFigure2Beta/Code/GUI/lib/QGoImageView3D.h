#ifndef __QGoImageView3D_h
#define __QGoImageView3D_h

#include "QGoImageView.h"
#include "SnapshotHelper.h"

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

class QGoImageView3D : public QGoImageView
{
  Q_OBJECT
public:
  explicit QGoImageView3D( QWidget* parent = 0 );
  virtual ~QGoImageView3D();

  virtual void SetImage( vtkImageData* iImage );
  virtual vtkImageData* GetImage();

  virtual void Update();

  vtkImageActor* GetImageActor( const int& );
  QVTKInteractor* GetInteractor( const int& );

  std::vector< vtkActor* > AddDataSet( vtkDataSet* dataset,
      vtkProperty* property = NULL,
      const bool& intersection = true,
      const bool& iDataVisibility = true  );

  virtual void setupUi( QWidget* parent );
  virtual void retranslateUi(QWidget *parent);

  int GetFullScreenView( ) const;

  void SaveStateSplitters();

  int GetSliceViewXY() const;
  int GetSliceViewXZ() const;
  int GetSliceViewYZ() const;

signals:
  void SliceViewXYChanged( int Slice );
  void SliceViewXZChanged( int Slice );
  void SliceViewYZChanged( int Slice );

  void FullScreenViewChanged( int View );

public slots:
  QString SnapshotViewXY( const GoFigure::SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QString SnapshotView2( const GoFigure::SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QString SnapshotView3( const GoFigure::SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QString SnapshotViewXYZ( const GoFigure::SnapshotImageType& iType,
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

  ///\todo MUST be implemented!
  virtual void SetLookupTable( vtkLookupTable* ) {}
  ///\todo MUST be implemented!
  virtual void ShowScalarBar( const bool& ) {}

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

  vtkEventQtSlotConnect*    VtkEventQtConnector;
  int                       IsFullScreen;

  virtual void resizeEvent( QResizeEvent* event );

  void SetupVTKtoQtConnections();

protected slots:
  void MoveSliderXY();
  void MoveSliderXZ();
  void MoveSliderYZ();

private:
};
#endif
