/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

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

#ifndef __QImagePageViewTracer_h
#define __QImagePageViewTracer_h

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
#include <vtkPoints.h>
#include <vtkEventQtSlotConnect.h>

#include "vtkViewImage3D.h"
#include "vtkViewImage2DWithContourWidgetCollection.h"
#include "QSplitterchild.h"
#include "MegaVTK2Configure.h"

#include <vtkPointHandleRepresentation2D.h>
#include <vtkSeedRepresentation.h>
#include <vtkSeedWidget.h>
#include <vtkProperty2D.h>

/**
\class QImagePageViewTracer
\bug vtkViewImage3D MUST be the last element inserted in Splitters (temporary solution)...
The bug is at the destruction of the QImagePageViewTracer:
the vtkViewImage3D must be destructed last.
*/
class QImagePageViewTracer : public QWidget
{
  Q_OBJECT
public:
  explicit QMEGAVTKADDON2_EXPORT QImagePageViewTracer( QWidget* parent = 0 );
  QMEGAVTKADDON2_EXPORT  ~QImagePageViewTracer();

  QMEGAVTKADDON2_EXPORT void SetImage( vtkImageData* input );

#ifdef MegaVTK_USE_ITK
  QMEGAVTKADDON2_EXPORT template< class TImage >
  void SetITKImage (TImage::Pointer);
#endif

  QMEGAVTKADDON2_EXPORT void setupUi( QWidget *Form );
  QMEGAVTKADDON2_EXPORT void retranslateUi(QWidget *Form);

  QMEGAVTKADDON2_EXPORT int GetFullScreenView( ) const;
  QMEGAVTKADDON2_EXPORT bool GetVolumeRendering( ) const;

  QMEGAVTKADDON2_EXPORT QVTKWidget* GetActiveQVTKWidget( );
  QMEGAVTKADDON2_EXPORT vtkViewImage* GetActiveView();

  enum SnapshotImageType
    {
    BMP = 0,
    EPS,
    JPEG,
    PNG,
    TIFF
    };

  QMEGAVTKADDON2_EXPORT void SetTag( const QString& iTag );
  QMEGAVTKADDON2_EXPORT QString GetTag( ) const;

  QMEGAVTKADDON2_EXPORT int GetCellId( ) const;

  QMEGAVTKADDON2_EXPORT void SetTracerON();
  QMEGAVTKADDON2_EXPORT void SetTracerOFF();
  QMEGAVTKADDON2_EXPORT void SetTracer( const bool& iState );
  QMEGAVTKADDON2_EXPORT bool GetTracerStatus( ) const;

  QMEGAVTKADDON2_EXPORT void SetSeedingON();
  QMEGAVTKADDON2_EXPORT void SetSeedingOFF();
  QMEGAVTKADDON2_EXPORT void SetSeeding( const bool& iState );
  QMEGAVTKADDON2_EXPORT bool GetSeedingStatus( ) const;

  QMEGAVTKADDON2_EXPORT vtkPoints* GetAllSeeds();
  QMEGAVTKADDON2_EXPORT void ClearAllSeeds();

  QMEGAVTKADDON2_EXPORT void GetBackgroundColor( double& r, double& g, double& b );
  QMEGAVTKADDON2_EXPORT double* GetBackgroundColor();

  QMEGAVTKADDON2_EXPORT void SetDatabaseRelatedVariables( const QString& iServer,
    const QString& iLogin, const QString& iPassword, const QString& iDatabaseName,
    const int& iExperimentID, const QString& iExperimentName );


  template< typename TContourContainer,
            typename TPropertyContainer >
  QMEGAVTKADDON2_EXPORT
  void AddContours( TContourContainer& iContours,
    TPropertyContainer& iProperty,
    const bool& iIntersection = true,
    const bool& iVizu3D = false )
    {
    this->Pool->SyncAddContours( iContours, iProperty, iIntersection );

    if( iVizu3D )
      {
      typename TContourContainer::iterator c_it = iContours.begin();
      typename TPropertyContainer::iterator p_it = iProperty.begin();

      for(; c_it != iContours.end(); ++c_it, ++p_it )
        {
        this->View3D->AddDataSet( *c_it, *p_it, false );
        }
      }
    }

  template< typename TPolyDataContainer >
  QMEGAVTKADDON2_EXPORT
  void RemoveContours( TPolyDataContainer& iContours )
    {
    this->Pool->SyncRemoveContours( iContours );
    typename TPolyDataContainer::iterator c_it = iContours.begin();

    for(; c_it != iContours.end(); ++c_it )
      {
      this->View3D->RemoveDataSet( *c_it );
      }
    }

  QMEGAVTKADDON2_EXPORT void SaveStateSplitters();

  QMEGAVTKADDON2_EXPORT QString m_DBServer;
  QMEGAVTKADDON2_EXPORT QString m_DBLogin;
  QMEGAVTKADDON2_EXPORT QString m_DBPassword;
  QMEGAVTKADDON2_EXPORT QString m_DBName;
  QMEGAVTKADDON2_EXPORT int     m_DBExperimentID;
  QMEGAVTKADDON2_EXPORT QString m_DBExperimentName;

public slots:

  QMEGAVTKADDON2_EXPORT void SetBackgroundColor( const double& r, const double& g, const double& b );
  QMEGAVTKADDON2_EXPORT void SetBackgroundColor( double rgb[3] );
  QMEGAVTKADDON2_EXPORT void SetBackgroundColor( const QColor& iColor );

  QMEGAVTKADDON2_EXPORT void SetLookupTable( vtkLookupTable* lut );
  QMEGAVTKADDON2_EXPORT void SetShowAnnotations( const bool& );
  QMEGAVTKADDON2_EXPORT void SetShowScalarBar( const bool& );
  QMEGAVTKADDON2_EXPORT void SetColorWindow( const double& );
  QMEGAVTKADDON2_EXPORT void SetColorLevel( const double& );

  QMEGAVTKADDON2_EXPORT void SetFullScreenView( const int& iS );
  QMEGAVTKADDON2_EXPORT void quadview();
  QMEGAVTKADDON2_EXPORT void FullScreenViewXY();
  QMEGAVTKADDON2_EXPORT void FullScreenView2();
  QMEGAVTKADDON2_EXPORT void FullScreenView3();
  QMEGAVTKADDON2_EXPORT void FullScreenViewXYZ();
  QMEGAVTKADDON2_EXPORT void SetView3DToTriPlanarMode();
  QMEGAVTKADDON2_EXPORT void SetView3DToVolumeRenderingMode();

  QMEGAVTKADDON2_EXPORT QString SnapshotViewXY( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QMEGAVTKADDON2_EXPORT QString SnapshotView2( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QMEGAVTKADDON2_EXPORT QString SnapshotView3( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QMEGAVTKADDON2_EXPORT QString SnapshotViewXYZ( const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );

  QMEGAVTKADDON2_EXPORT void SetSlideView1( const int& );
  QMEGAVTKADDON2_EXPORT void SetSlideView2( const int& );
  QMEGAVTKADDON2_EXPORT void SetSlideView3( const int& );

  QMEGAVTKADDON2_EXPORT void ValidateContour(
    const int& iId,
    const QColor& iColor,
    const bool& iSave );
  QMEGAVTKADDON2_EXPORT void ReinitializeContour( );
  //   void Render( );

protected:
  QSplitter*          VSplitter;
  QSplitterchild*     HtSplitter;
  QSplitterchild*     HbSplitter;

  QWidget*      LayOutWidget1;
  QHBoxLayout*  LayOut1;
  QSlider*      Slider1;
  QVTKWidget*   QvtkWidget_XY;

  QWidget*      LayOutWidget2;
  QHBoxLayout*  LayOut2;
  QSlider*      Slider2;
  QVTKWidget*   QvtkWidget_2;

  QWidget*      LayOutWidget3;
  QHBoxLayout*  LayOut3;
  QSlider*      Slider3;
  QVTKWidget*   QvtkWidget_3;

  QWidget*      LayOutWidget4;
  QHBoxLayout*  LayOut4;
  QVTKWidget*   QvtkWidget_XYZ;
  QSpacerItem*  Spacer;

  vtkViewImage3D* View3D;

  vtkImageData*           Image;
  vtkEventQtSlotConnect*  VtkEventQtConnector;

  vtkViewImage2DWithContourWidgetCollection*      Pool;
  std::vector< vtkPointHandleRepresentation2D* >  Handle;
  std::vector< vtkSeedRepresentation* >           SeedRep;
  std::vector< vtkSeedWidget* >                   SeedWidget;

  QString Tag;

/*  QString m_DBServer;
  QString m_DBLogin;
  QString m_DBPassword;
  QString m_DBName;
  int     m_DBExperimentID;
  QString m_DBExperimentName;*/

#ifdef MegaVTK_USE_ITK
  /**
  This pointer is used to store internally a reference to the
  current ITK->VTK converter, in order to prevent the image buffer
  to be deleted unexpectdely. See the SetITKImageInXXX for more
  information. */
  //BTX
  itk::ProcessObject::Pointer ImageConverter;
  //ETX
#endif

  virtual void resizeEvent( QResizeEvent* event );
  //   virtual void dragEnterEvent ( QDragEnterEvent * event );

  QString SnapshotView( QVTKWidget* iWidget,
    const SnapshotImageType& iType,
    const QString& iBaseName = QString( "snapshot" ) );

  bool BuildScreenshotFromImage( vtkImageData* image,
    vtkImageData* screenshot, int size = 0 );
  bool BuildScreenshotFromRenderWindow( vtkRenderWindow *win,
    vtkImageData* screenshot, int size = 0 );

  void Set2DImage( vtkImageData* input );
  void Set3DImage( vtkImageData* input );

  int   SnapshotId;
  int   IsFullScreen;
  bool  IsVolumeRendering;
  int   CellId;
  bool  Is2DImage;

protected slots:
  void MoveSlider1();
  void MoveSlider2();
  void MoveSlider3();

};


#endif
