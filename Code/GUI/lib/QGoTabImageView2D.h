/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
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
#ifndef __QGoTabImageView2D_h
#define __QGoTabImageView2D_h

class vtkImageData;

class QAction;
class QToolBar;
class QDockWidget;
class QGoImageView2D;

#include "QGoTabImageViewNDBase.h"

/**
\class QGoTabImageView2D
\brief Element of the QTabWidget to be used to visualized 2D images.
\example GUI/lib/qgotabimageview2d.cxx
*/
class QGoTabImageView2D : public QGoTabImageViewNDBase
{
  Q_OBJECT
public:
  explicit QGoTabImageView2D( QWidget* parent = 0 );
  virtual ~QGoTabImageView2D();

  typedef QGoTabImageViewNDBase::QGoDockWidgetStatusPair QGoDockWidgetStatusPair;

  GoFigure::TabDimensionType GetTabDimensionType( ) const;

  virtual void Update();

  void setupUi( QWidget* parent );
  void retranslateUi( QWidget *parent );

  virtual std::list< QWidget* > AdditionalWidget();

  virtual void WriteSettings();
  virtual void ReadSettings();

public slots:
  void ChangeLookupTable();
  void ShowScalarBar( const bool& );
  void ChangeBackgroundColor();
  void TakeSnapshot();

  void DefaultMode();
  void ZoomMode();
  void PanMode();

protected:
  QGoImageView2D*       m_ImageView;
  QAction*              m_TakeSnapshotAction;

  void GetBackgroundColorFromImageViewer( );
  void SetBackgroundColorToImageViewer( );

  void SetImageToImageViewer( vtkImageData* image );
  int* GetImageCoordinatesFromWorldCoordinates( double pos[3] );
//   std::vector< vtkQuadricLODActor* >
  std::vector< vtkActor* > AddContour( const int& iId, vtkPolyData* dataset,
      vtkProperty* property = NULL );

  QAction*              m_BackgroundColorAction;

  virtual void RemoveActorFromViewer( const int& iId, vtkActor* iActor );
  virtual void DisplayActorInViewer( const int& iId, vtkActor* iActor );

  virtual void SetSlice( int iDir, int* iIdx );

  void CreateToolsActions();
  void CreateModeActions();

private:
  Q_DISABLE_COPY( QGoTabImageView2D );
};
#endif
