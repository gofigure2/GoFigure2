/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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
#ifndef __QGoMeshEditingWidgetManager_h
#define __QGoMeshEditingWidgetManager_h

#include "QGoTraceEditingWidget.h"
#include "QGoGUILibConfigure.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "QGoMeshLevelSetAlgo.h"
#include "QGoMeshShapeAlgo.h"
#include <QAction>
#include <QDockWidget>


/**
\class QGoMeshEditingWidgetManager
\brief
*/
class QGOGUILIB_EXPORT QGoMeshEditingWidgetManager: public QObject
{
  Q_OBJECT
public:
  QGoMeshEditingWidgetManager(std::vector<QString> iVectChannels, 
    int iTimeMin, int iTimeMax, vtkPoints* iSeeds, 
    std::vector< vtkSmartPointer< vtkImageData > >* iImages, 
    int* iCurrentTimePoint,
    QWidget* iParent=0);
  ~QGoMeshEditingWidgetManager();

  QAction* GetToggleViewAction();
  QDockWidget* GetDockWidget();

  void SetTSliceForClassicView();
  void SetTSliceForDopplerView(QStringList iListTimePoints, int iChannelNumber);

public slots:
  void SetVisible(bool isVisible);

signals:

  void UpdateSeeds();
  void ClearAllSeeds();
  void MeshesCreatedFromAlgo(std::vector<vtkPolyData *> iVectPolydata, int iTCoord);

protected:
  QDockWidget*           m_MeshEditingDockWidget;
  QGoTraceEditingWidget* m_MeshEditingWidget;
 
  vtkPoints*                                      m_Seeds;
  std::vector< vtkSmartPointer< vtkImageData > >* m_Images;
  int*                                            m_CurrentTimePoint;

  QGoMeshLevelSetAlgo*            m_LevelSetAlgo;
  QGoMeshShapeAlgo*               m_ShapeAlgo;

  void SetTheMeshWidget(std::vector<QString> iVectChannels, int iTimeMin, 
    int iTimeMax, QWidget* iParent);
  void SetTheDockWidget(QWidget* iParent);
  int GetSelectedTimePoint();

  void SetLevelSetAlgo(QWidget* iParent=0);
  void SetShapeAlgo(QWidget* iParent=0);

  template<typename T>
  void GetPolydatasFromAlgo(T* iAlgo)
    {
    emit UpdateSeeds();
    std::vector<vtkPolyData*> NewMeshes = iAlgo->ApplyAlgo(this->m_Seeds,
      this->m_Images, this->m_MeshEditingWidget->GetChannelNumber() );
    emit MeshesCreatedFromAlgo(NewMeshes, this->GetSelectedTimePoint() );
    emit ClearAllSeeds();
    }
  
signals:
  void SetSeedInteractorBehaviour(bool enable);

protected slots:
  void ApplyLevelSetAlgo();
  void ApplyShapeAlgo();

};

#endif