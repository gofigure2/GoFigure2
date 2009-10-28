/*=========================================================================
  Author: $Author:$  // Author of last commit
  Version: $Rev:$  // Revision of last commit
  Date: $Date:$  // Date of last commit
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

#ifndef __QGoTabImageViewElementBase_h
#define __QGoTabImageViewElementBase_h

#include "QGoTabElementBase.h"
#include "QGoVisualizationDockWidget.h"
#include "QGoManualSegmentationDockWidget.h"
#include "ContourStructureHelper.h"

class vtkImageData;
class vtkContourWidget;
class vtkOrientedGlyphContourRepresentation;
class vtkDataSet;
class vtkProperty;

class QGoTabImageViewElementBase : public QGoTabElementBase
{
  Q_OBJECT
public:
  explicit QGoTabImageViewElementBase( QWidget* parent = 0 );
  virtual ~QGoTabImageViewElementBase();

  virtual void SetImage( vtkImageData* iImage );
  virtual void Update() = 0;

  virtual void SetColor( const bool& iColor );

  virtual void WriteSettings();
  virtual void ReadSettings();

  virtual void ValidateContour( const int& iId );

public slots:
  void ChangeBackgroundColor();
  void ShowAllChannels( bool iChecked );
  void ShowOneChannel( int iChannel );
  void ActivateManualSegmentationEditor( const bool& iActivate );
  void ValidateContour();

protected:
  bool          m_Color;
  QColor        m_BackgroundColor;
  unsigned int  m_ContourId;

  QHBoxLayout*  m_LayOut;
  vtkImageData* m_Image;

  std::vector< vtkContourWidget* >                      m_ContourWidget;
  std::vector< vtkOrientedGlyphContourRepresentation* > m_ContourRepresentation;
  ContourStructureMultiIndexContainer                   m_ContourContainer;

  QGoVisualizationDockWidget*       m_VisuDockWidget;
  QGoManualSegmentationDockWidget*  m_ManualSegmentationDockWidget;

  virtual void GetBackgroundColorFromImageViewer( ) = 0;
  virtual void SetBackgroundColorToImageViewer( ) = 0;
  virtual void SetImageToImageViewer( vtkImageData* image ) = 0;
  virtual int* GetImageCoordinatesFromWorldCoordinates( double pos[3] ) = 0;

  virtual std::vector< vtkActor* > AddContour( const int& iId,
      vtkPolyData* dataset,
      vtkProperty* property = NULL ) = 0;

private:
  QGoTabImageViewElementBase( const QGoTabImageViewElementBase& );
  void operator = ( const QGoTabImageViewElementBase& );
};
#endif
