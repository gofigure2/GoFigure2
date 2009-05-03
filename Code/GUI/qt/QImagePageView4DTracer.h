/*=========================================================================
 Authors: The GoFigure Dev. Team.
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

#ifndef __QImagePageView4DTracer_h
#define __QImagePageView4DTracer_h

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
#include <qpushbutton.h>

#include <vtkImageData.h>
#include <vtkEventQtSlotConnect.h>

#include "vtkViewImage3D.h"
#include "vtkViewImage2DWithContourWidgetCollection.h"
#include "QSplitterchild.h"
#include "QImagePageViewTracer.h"

#include "GoFigureFileInfoHelper.h"

/**
\class QImagePageView4DTracer
*/
class QImagePageView4DTracer : public QWidget
{
  Q_OBJECT
public:
  explicit QMEGAVTKADDON2_EXPORT QImagePageView4DTracer( QWidget* parent = 0 );
  QMEGAVTKADDON2_EXPORT ~QImagePageView4DTracer();

  QMEGAVTKADDON2_EXPORT void SetFileName( const char* name );
  QMEGAVTKADDON2_EXPORT void SetColorVizu( const bool& value )
    { this->ColorVizu = value; };
  QMEGAVTKADDON2_EXPORT void SetFileTypeToSerie( )
    { this->IsSerie = true; };
  QMEGAVTKADDON2_EXPORT void SetSerieType( const int& value )
    { 
	if( value == 0 ) 
	  {
	  this->IsLsm = true;
	  this->IsMegaCapture = false;
	  }
	else
	  {
	  this->IsLsm = false;
	  this->IsMegaCapture = true;
      }
	this->IsFileListComputed = false;
    };
  QMEGAVTKADDON2_EXPORT void SetSerieTypeToLsm( )
    { this->SetSerieType( 0 ); };
  QMEGAVTKADDON2_EXPORT void SetSerieTypeToMegaCapture( )
    { this->SetSerieType( 1 ); };

public slots:
  QMEGAVTKADDON2_EXPORT void SetView( const int& value );
  QMEGAVTKADDON2_EXPORT void RunMovie();
  QMEGAVTKADDON2_EXPORT void SwitchColorMode( );

  // decorated, pass-through methods.
  QMEGAVTKADDON2_EXPORT void ValidateContour(
    const int& iId,
    const QColor& iColor,
    const bool& iSave )
    { this->Whatever->ValidateContour( iId, iColor, iSave ); };
  QMEGAVTKADDON2_EXPORT void ReinitializeContour( )
    { this->Whatever->ReinitializeContour( ); };
  QMEGAVTKADDON2_EXPORT void SetCellId( const unsigned int& iId )
    { this->Whatever->SetCellId( iId ); };
  QMEGAVTKADDON2_EXPORT unsigned int GetCellId( ) const
    { return this->Whatever->GetCellId(); };
  QMEGAVTKADDON2_EXPORT void GetBackgroundColor( double& r, double& g, double& b )
    { return this->Whatever->GetBackgroundColor( r, g, b ); };
  QMEGAVTKADDON2_EXPORT double* GetBackgroundColor()
    { return this->Whatever->GetBackgroundColor(); };
  QMEGAVTKADDON2_EXPORT void SetBackgroundColor( const double& r, const double& g, const double& b )
    { this->Whatever->SetBackgroundColor( r, g, b ); };
  QMEGAVTKADDON2_EXPORT void SetBackgroundColor( double rgb[3] )
    { this->Whatever->SetBackgroundColor( rgb ); };
  QMEGAVTKADDON2_EXPORT void SetBackgroundColor( const QColor& iColor )
    { this->Whatever->SetBackgroundColor( iColor ); };
  QMEGAVTKADDON2_EXPORT int GetFullScreenView( ) const
    { return this->Whatever->GetFullScreenView( ); };
  QMEGAVTKADDON2_EXPORT void SetFullScreenView( const int& iS )
    { this->Whatever->SetFullScreenView( iS ); };
  QMEGAVTKADDON2_EXPORT void quadview()
    { this->Whatever->quadview(); };
  QMEGAVTKADDON2_EXPORT void FullScreenViewXY()
    { this->Whatever->FullScreenViewXY(); };
  QMEGAVTKADDON2_EXPORT void FullScreenView2()
    { this->Whatever->FullScreenView2(); };
  QMEGAVTKADDON2_EXPORT void FullScreenView3()
    { this->Whatever->FullScreenView3(); };
  QMEGAVTKADDON2_EXPORT void FullScreenViewXYZ()
    { this->Whatever->FullScreenViewXYZ(); };
  QMEGAVTKADDON2_EXPORT void SetView3DToTriPlanarMode()
    { this->Whatever->SetView3DToTriPlanarMode(); };
  QMEGAVTKADDON2_EXPORT void SetView3DToVolumeRenderingMode()
    { this->Whatever->SetView3DToVolumeRenderingMode(); };
  QMEGAVTKADDON2_EXPORT void SetTracerON()
    { this->Whatever->SetTracerON(); };
  QMEGAVTKADDON2_EXPORT void SetTracerOFF()
    { this->Whatever->SetTracerOFF(); };

protected:
  QWidget*      LayOutWidget1;
  QVBoxLayout*  LayOut1;
  QSlider*      slider1;
  QPushButton*  button1;
  QPushButton*  button2;

  void ReadMultiFile( const int& TimePoint );
  void ReadLSMFile( const int& TimePoint );
  virtual void resizeEvent( QResizeEvent* event );

  QImagePageViewTracer* Whatever;
  FileListType          FileList;
  int                   NumberOfTimePoints;
  char*                 FileName;
  vtkImageData*         Image;
  bool                  ColorVizu;
  bool                  IsSerie;
  bool                  IsLsm;
  bool                  IsMegaCapture;
  bool                  IsFileListComputed;
};

#endif
