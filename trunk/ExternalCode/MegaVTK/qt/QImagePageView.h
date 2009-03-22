#ifndef __QImagePageView_h
#define __QImagePageView_h

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QWidget>
#include <QSplitter>
#include <QSlider>
#include <QSpacerItem>
#include <QVTKWidget.h>

#include <vtkImageData.h>

#include "vtkViewImage.h"
#include "vtkViewImage2D.h"
#include "vtkViewImage3D.h"
#include "vtkViewImage2DCollection.h"

class QImagePageView : public QWidget
{
  Q_OBJECT
public:
  explicit QImagePageView( QWidget* parent = 0 );
  ~QImagePageView();

  void SetImage( vtkImageData* input );
#ifdef MegaVTK_USE_ITK
  template< class TImage >
  void SetITKImage (TImage::Pointer);
#endif

  void setupUi( QWidget *Form );
  void retranslateUi(QWidget *Form);

  int GetFullScreenView( ) const;

  QVTKWidget* GetActiveQVTKWidget( );
  vtkViewImage* GetActiveView();

  enum SnapshotImageType
    {
    BMP = 0,
    EPS,
    JPEG,
    PNG,
    TIFF
    };

  void SetTag( const QString& iTag );
  QString GetTag( ) const;

public slots:

  void SetBackgroundColor( const double& r, const double& g, const double& b );
  void SetBackgroundColor( double rgb[3] );
  void SetBackgroundColor( const QColor& iColor );

  void SetLookupTable( vtkLookupTable* lut );
  void SetShowAnnotations( const bool& );
  void SetShowScalarBar( const bool& );
  void SetColorWindow( const double& );
  void SetColorLevel( const double& );

  void SetFullScreenView( const int& iS );
  void FullScreenView1();
  void FullScreenView2();
  void FullScreenView3();
  void FullScreenView4();
//   void SetView4ToTriPlanarMode();
//   void SetView4ToVolumeRenderingMode();
/*
  void SnapshotView1( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  void SnapshotView2( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  void SnapshotView3( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  void SnapshotView4( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );*/

//   void Render( );

protected:
  QSplitter*    vSplitter;
  QSplitter*    htSplitter;
  QSplitter*    hbSplitter;

  QWidget*      LayOutWidget1;
  QHBoxLayout*  LayOut1;
  QSlider*      slider1;
  QVTKWidget*   qvtkWidget_1;

  QWidget*      LayOutWidget2;
  QHBoxLayout*  LayOut2;
  QSlider*      slider2;
  QVTKWidget*   qvtkWidget_2;

  QWidget*      LayOutWidget3;
  QHBoxLayout*  LayOut3;
  QSlider*      slider3;
  QVTKWidget*   qvtkWidget_3;

  QWidget*      LayOutWidget4;
  QHBoxLayout*  LayOut4;
  QVTKWidget*   qvtkWidget_4;
  QSpacerItem*  Spacer;
  vtkViewImage3D* View3D;

  vtkImageData* Image;

  vtkViewImage2DCollection* Pool;

  QString Tag;

#ifdef MegaVTK_USE_ITK
  /**
     This pointer is used to store internally a reference to the
     current ITK->VTK converter, in order to prevent the image buffer
     to be deleted unexpectdely. See the SetITKImageInXXX for more
     information.
   */
  //BTX
  itk::ProcessObject::Pointer ImageConverter;
  //ETX
#endif

  void CreateAllViews( );

  virtual void resizeEvent( QResizeEvent* event );
//   virtual void dragEnterEvent ( QDragEnterEvent * event );

//   void SnapshotView( QVTKWidget* iWidget,
//     const SnapshotImageType& iType,
//     const QString& iBaseName = QString( "snapshot" ) );
//
//   bool BuildScreenshotFromImage( vtkImageData* image,
//     vtkImageData* screenshot, int size = 0 );
//   bool BuildScreenshotFromRenderWindow( vtkRenderWindow *win,
//     vtkImageData* screenshot, int size = 0 );

  double BackgroundRGB[3];
  int SnapshotId;

  int IsFullScreen;
};

#endif
