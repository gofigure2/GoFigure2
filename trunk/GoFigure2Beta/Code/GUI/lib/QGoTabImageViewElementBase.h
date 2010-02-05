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
#include "ContourMeshStructureHelper.h"

#ifdef   ENABLEFFMPEG || ENABLEAVI
#include "QGoVideoRecorder.h"
#endif

class vtkContourWidget;
class vtkOrientedGlyphContourRepresentation;
class vtkDataSet;
class vtkProperty;
class vtkQuadricLODActor;

/**
\class QGoTabImageViewElementBase
\brief
\example GUI/lib/qgotabimageviewelementbase.cxx
*/
class QGoTabImageViewElementBase : public QGoTabElementBase
{
  Q_OBJECT
public:
  explicit QGoTabImageViewElementBase( QWidget* parent = 0 );
  virtual ~QGoTabImageViewElementBase();

  virtual void Update() = 0;

  virtual void SetColor( const bool& iColor );

  virtual void WriteSettings();
  virtual void ReadSettings();

  virtual void ValidateContour( const int& iId );

public slots:
  void ChangeBackgroundColor();
  virtual void ShowAllChannels( bool iChecked ) = 0;
  virtual void ShowOneChannel( int iChannel ) = 0;
  void ActivateManualSegmentationEditor( const bool& iActivate );
  virtual void ValidateContour();
  void ChangeContourRepresentationProperty();
  void ReEditContour( const unsigned int& iId );

protected:
  bool          m_Color;
  QColor        m_BackgroundColor;
  unsigned int  m_ContourId;
  bool          m_ReEditContourMode;

  QHBoxLayout*  m_LayOut;
  
  std::vector< vtkSmartPointer< vtkContourWidget > >                      m_ContourWidget;
  std::vector< vtkSmartPointer< vtkOrientedGlyphContourRepresentation > > m_ContourRepresentation;
  ContourMeshStructureMultiIndexContainer                                 m_ContourMeshContainer;

  QGoVisualizationDockWidget*       m_VisuDockWidget;
  QGoManualSegmentationDockWidget*  m_ManualSegmentationDockWidget;
#ifdef   ENABLEFFMPEG || ENABLEAVI
  QGoVideoRecorder*                 m_VideoRecorderWidget;
#endif

  void CreateManualSegmentationdockWidget();

  virtual void GetBackgroundColorFromImageViewer( ) = 0;
  virtual void SetBackgroundColorToImageViewer( ) = 0;

  virtual int* GetImageCoordinatesFromWorldCoordinates( double pos[3] ) = 0;

  virtual void RemoveActorFromViewer( const int& iId, vtkActor* iActor ) = 0;
  virtual void DisplayActorInViewer( const int& iId, vtkActor* iActor ) = 0;

  virtual std::vector< vtkQuadricLODActor* > AddContour( const int& iId,
      vtkPolyData* dataset,
      vtkProperty* property = NULL ) = 0;

  virtual void SetSlice( int iDir, int* iIdx ) = 0;

private:
  QGoTabImageViewElementBase( const QGoTabImageViewElementBase& );
  void operator = ( const QGoTabImageViewElementBase& );
};
#endif
