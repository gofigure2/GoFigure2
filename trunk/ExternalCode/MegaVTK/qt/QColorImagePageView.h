/*=========================================================================
 Authors: Thhe GoFigure Dev. Team.
 while at Megason Lab, Systems biology, Harvard Medical school, 2009 
 
 Copyright (c) 2009, President and Fellows of Harvard College.
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 Redistributions of source code must retain the above copyright notice, 
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation 
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College 
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written 
 permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 =========================================================================*/

#ifndef __QColorImagePageView
#define __QColorImagePageView

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

#include "vtkViewColorImage3D.h"
#include "vtkViewColorImage2D.h"
#include "vtkViewColorImage2DCollection.h"
#include "QSplitterchild.h"

/**
\class QColorImagePageView
*/
class QColorImagePageView : public QWidget
{
  Q_OBJECT
public:
  explicit QColorImagePageView( QWidget* parent = 0 );
  ~QColorImagePageView();

  void SetImage( vtkImageData* input );

  void setupUi( QWidget *Form );
  void retranslateUi(QWidget *Form);

  int GetFullScreenView( ) const;

  QVTKWidget* GetActiveQVTKWidget( );
  vtkViewColorImage* GetActiveView();

  enum SnapshotImageType
    {
    BMP = 0,
    EPS,
    JPEG,
    PNG,
    TIFF
    };

  bool BuildScreenshotFromImage(vtkImageData*, vtkImageData*, int);
  bool BuildScreenshotFromRenderWindow(vtkRenderWindow*, vtkImageData*, int);
  void SnapshotView(QVTKWidget*, const QColorImagePageView::SnapshotImageType&, const QString&);

  void SetTag( const QString& iTag );
  QString GetTag( ) const;

  void SetCellId( const unsigned int& iId );
  unsigned int GetCellId( ) const;

  void GetBackgroundColor( double& r, double& g, double& b );
  double* GetBackgroundColor();

public slots:

  void SetBackgroundColor( const double& r, const double& g, const double& b );
  void SetBackgroundColor( double rgb[3] );
  void SetBackgroundColor( const QColor& iColor );

  void SetLookupTable( vtkLookupTable* lut );
  void SetShowAnnotations( const bool& );
  void SetShowScalarBar( const bool& ) {};
  void SetColorWindow( const double& ) {};
  void SetColorLevel( const double& ) {};

  void SetFullScreenView( const int& iS );
  void quadview();
  void FullScreenViewXY();
  void FullScreenView2();
  void FullScreenView3();
  void FullScreenViewXYZ();
  void SetView3DToTriPlanarMode();
  void SetView3DToVolumeRenderingMode();

  void SnapshotViewXY( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  void SnapshotView2( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  void SnapshotView3( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  void SnapshotViewXYZ( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );

  void SetSlideView1( const int& );
  void SetSlideView2( const int& );
  void SetSlideView3( const int& );

  void ValidateContour(
    const int& iId,
    const QColor& iColor,
    const bool& iSave );
  void ReinitializeContour( );

protected:
  QSplitter*    vSplitter;
  QSplitterchild*    htSplitter;
  QSplitterchild*    hbSplitter;

  QWidget*      LayOutWidget1;
  QHBoxLayout*  LayOut1;
  QSlider*      slider1;
  QVTKWidget*   qvtkWidget_XY;

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
  QVTKWidget*   qvtkWidget_XYZ;
  QSpacerItem*  Spacer;
  vtkViewColorImage3D* View3D;

  vtkImageData* Image;

  vtkViewColorImage2DCollection* Pool;

  QString Tag;

  virtual void resizeEvent( QResizeEvent* event );

  double BackgroundRGB[3];
  
  unsigned int CellId;
  unsigned int SnapshotId;
  int IsFullScreen;
};

#endif
