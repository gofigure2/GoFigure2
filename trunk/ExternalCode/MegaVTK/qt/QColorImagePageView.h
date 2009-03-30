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
#include "MegaVTK2Configure.h"

/**
\class QColorImagePageView
*/
class QColorImagePageView : public QWidget
{
  Q_OBJECT
public:
  explicit QMEGAVTKADDON2_EXPORT QColorImagePageView( QWidget* parent = 0 );
  QMEGAVTKADDON2_EXPORT ~QColorImagePageView();

  QMEGAVTKADDON2_EXPORT void SetImage( vtkImageData* input );

  QMEGAVTKADDON2_EXPORT void setupUi( QWidget *Form );
  QMEGAVTKADDON2_EXPORT void retranslateUi(QWidget *Form);

  QMEGAVTKADDON2_EXPORT int GetFullScreenView( ) const;

  QMEGAVTKADDON2_EXPORT QVTKWidget* GetActiveQVTKWidget( );
  QMEGAVTKADDON2_EXPORT vtkViewColorImage* GetActiveView();

  enum SnapshotImageType
    {
    BMP = 0,
    EPS,
    JPEG,
    PNG,
    TIFF
    };

  QMEGAVTKADDON2_EXPORT bool BuildScreenshotFromImage(vtkImageData*, vtkImageData*, int);
  QMEGAVTKADDON2_EXPORT bool BuildScreenshotFromRenderWindow(vtkRenderWindow*, vtkImageData*, int);
  QMEGAVTKADDON2_EXPORT void SnapshotView(QVTKWidget*, const QColorImagePageView::SnapshotImageType&, const QString&);

  QMEGAVTKADDON2_EXPORT void SetTag( const QString& iTag );
  QMEGAVTKADDON2_EXPORT QString GetTag( ) const;

  QMEGAVTKADDON2_EXPORT void SetCellId( const unsigned int& iId );
  QMEGAVTKADDON2_EXPORT unsigned int GetCellId( ) const;

  QMEGAVTKADDON2_EXPORT void GetBackgroundColor( double& r, double& g, double& b );
  QMEGAVTKADDON2_EXPORT double* GetBackgroundColor();

public slots:

  QMEGAVTKADDON2_EXPORT void SetBackgroundColor( const double& r, const double& g, const double& b );
  QMEGAVTKADDON2_EXPORT void SetBackgroundColor( double rgb[3] );
  QMEGAVTKADDON2_EXPORT void SetBackgroundColor( const QColor& iColor );

  QMEGAVTKADDON2_EXPORT void SetLookupTable( vtkLookupTable* lut );
  QMEGAVTKADDON2_EXPORT void SetShowAnnotations( const bool& );
  QMEGAVTKADDON2_EXPORT void SetShowScalarBar( const bool& ) {};
  QMEGAVTKADDON2_EXPORT void SetColorWindow( const double& ) {};
  QMEGAVTKADDON2_EXPORT void SetColorLevel( const double& ) {};

  QMEGAVTKADDON2_EXPORT void SetFullScreenView( const int& iS );
  QMEGAVTKADDON2_EXPORT void quadview();
  QMEGAVTKADDON2_EXPORT void FullScreenViewXY();
  QMEGAVTKADDON2_EXPORT void FullScreenView2();
  QMEGAVTKADDON2_EXPORT void FullScreenView3();
  QMEGAVTKADDON2_EXPORT void FullScreenViewXYZ();
  QMEGAVTKADDON2_EXPORT void SetView3DToTriPlanarMode();
  QMEGAVTKADDON2_EXPORT void SetView3DToVolumeRenderingMode();

  QMEGAVTKADDON2_EXPORT void SnapshotViewXY( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QMEGAVTKADDON2_EXPORT void SnapshotView2( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QMEGAVTKADDON2_EXPORT void SnapshotView3( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QMEGAVTKADDON2_EXPORT void SnapshotViewXYZ( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );

  QMEGAVTKADDON2_EXPORT void SetSlideView1( const int& );
  QMEGAVTKADDON2_EXPORT void SetSlideView2( const int& );
  QMEGAVTKADDON2_EXPORT void SetSlideView3( const int& );

  QMEGAVTKADDON2_EXPORT void ValidateContour(
    const int& iId,
    const QColor& iColor,
    const bool& iSave );
  QMEGAVTKADDON2_EXPORT void ReinitializeContour( );

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
