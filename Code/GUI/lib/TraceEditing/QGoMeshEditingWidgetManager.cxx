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
#include "QGoMeshEditingWidgetManager.h"
#include "QGoAlgorithmWidget.h"
#include "QGoAlgoParameter.h"
#include "QGoAlgorithmsManagerWidget.h"
#include "vtkSmartPointer.h"
#include "vtkImageExport.h"
#include "vtkImageData.h"
#include <iostream>


QGoMeshEditingWidgetManager::QGoMeshEditingWidgetManager(
  std::vector<QString> iVectChannels, 
  int iTimeMin,
  int iTimeMax,
  vtkPoints* iSeeds, 
  std::vector< vtkSmartPointer< vtkImageData > >* iImages, 
  int* iCurrentTimePoint,
  QWidget* iParent)
{
  
  this->m_Seeds = iSeeds;
  this->m_Images = iImages;
  this->m_CurrentTimePoint = iCurrentTimePoint;

  this->SetTheMeshWidget(iVectChannels, iTimeMin, iTimeMax, iParent);
  this->SetTheDockWidget(iParent); 

  this->SetSemiAutomatedAlgorithms(iParent);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoMeshEditingWidgetManager::~QGoMeshEditingWidgetManager()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::ApplyLevelSetAlgo()
{
  this->GetPolydatasFromAlgo<QGoMeshLevelSetAlgo>(this->m_LevelSetAlgo);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::ApplyShapeAlgo()
{
  this->GetPolydatasFromAlgo<QGoMeshShapeAlgo>(this->m_ShapeAlgo);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::ApplyWaterShedAlgo()
{
  this->GetPolydatasFromAlgo<QGoMeshWaterShedAlgo>(this->m_WaterShedAlgo);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetTheMeshWidget(
  std::vector<QString> iVectChannels, int iTimeMin, 
  int iTimeMax, QWidget* iParent)
{
  QStringList ListTimePoints;
  for (int i = iTimeMin; i < iTimeMax+1; ++i)
    {
    ListTimePoints.push_back(tr("%1").arg(i));
    }

  this->m_MeshEditingWidget = new QGoTraceEditingWidget(
   "Mesh", iVectChannels, ListTimePoints, iParent);

  this->SetTSliceForClassicView();
  this->SetSplitMergeMode(iParent);

  QObject::connect( this->m_MeshEditingWidget, 
                    SIGNAL(SetSeedInteractorBehaviour(bool) ),
                    this,
                    SIGNAL(SetSeedInteractorBehaviour(bool) ) );

  QObject::connect( this->m_MeshEditingWidget,
                    SIGNAL(ResetClicked() ),
                    this, SIGNAL(ClearAllSeeds() ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetTheDockWidget(QWidget* iParent)
{

  this->m_MeshEditingDockWidget = new QDockWidget(iParent);
  this->m_MeshEditingDockWidget->setWindowTitle("Mesh Editing");
  this->m_MeshEditingDockWidget->setWidget(this->m_MeshEditingWidget);
  
  QIcon MeshSegmentationIcon;
  MeshSegmentationIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/MeshEditing.png") ),
                                 QIcon::Normal, QIcon::Off);

 this->m_MeshEditingDockWidget->toggleViewAction()->setIcon(MeshSegmentationIcon);
 this->m_MeshEditingDockWidget->toggleViewAction()->setToolTip( tr("Mesh Editing") );
 this->m_MeshEditingDockWidget->toggleViewAction()->setStatusTip( 
    tr("Create meshes manually, semi-automatically or automatically") );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QAction* QGoMeshEditingWidgetManager::GetToggleViewAction()
{
  return this->m_MeshEditingDockWidget->toggleViewAction();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QDockWidget* QGoMeshEditingWidgetManager::GetDockWidget()
{
  return this->m_MeshEditingDockWidget;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetVisible(bool isVisible)
{
  this->m_MeshEditingDockWidget->setVisible(isVisible);
  this->m_MeshEditingWidget->CheckTheCurrentMode(isVisible);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetTSliceForClassicView()
{
  this->m_MeshEditingWidget->SetTSliceForClassicView(*this->m_CurrentTimePoint);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetTSliceForDopplerView(
  QStringList iListTimePoints, int iChannelNumber)
{
  this->m_MeshEditingWidget->SetTSliceForDopplerView(
    iListTimePoints, iChannelNumber);
} 
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoMeshEditingWidgetManager::GetSelectedTimePoint()
{
  return this->m_MeshEditingWidget->GetSelectedTimePoint();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetSemiAutomatedAlgorithms(QWidget* iParent)
{
  //level set:
  m_LevelSetAlgo = new QGoMeshLevelSetAlgo(iParent);
  QGoAlgorithmWidget* LevelSetWidget = m_LevelSetAlgo->GetAlgoWidget();
  this->m_MeshEditingWidget->AddAlgoWidgetForSemiAutomatedMode(LevelSetWidget);

  QObject::connect(LevelSetWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplyLevelSetAlgo() ) );

  //shape:
  this->m_ShapeAlgo = new QGoMeshShapeAlgo(iParent);
  QGoAlgorithmWidget* ShapeWidget = this->m_ShapeAlgo->GetAlgoWidget();
  this->m_MeshEditingWidget->AddAlgoWidgetForSemiAutomatedMode(ShapeWidget);

  QObject::connect(ShapeWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplyShapeAlgo() ) );

  //watershed:
  this->m_WaterShedAlgo = new QGoMeshWaterShedAlgo(iParent);
  QGoAlgorithmWidget* WaterShedWidget = m_WaterShedAlgo->GetAlgoWidget();
  this->m_MeshEditingWidget->AddAlgoWidgetForSemiAutomatedMode(WaterShedWidget);

  QObject::connect(WaterShedWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplyWaterShedAlgo() ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetSplitMergeMode(QWidget* iParent)
{
  QGoAlgorithmsManagerWidget* SplitAlgoWidget = 
    new QGoAlgorithmsManagerWidget("Split", iParent);
  this->m_MeshEditingWidget->AddMode(SplitAlgoWidget);
  
  m_DanielAlgo = new QGoMeshSplitDanielssonDistanceAlgo(iParent);
  QGoAlgorithmWidget * DanielWidget = m_DanielAlgo->GetAlgoWidget();
  SplitAlgoWidget->AddMethod(DanielWidget );

  QObject::connect( DanielWidget, SIGNAL(ApplyAlgo() ) , 
                    this, SLOT(ApplyDanielAlgo() ) );

  QGoAlgorithmsManagerWidget* MergeAlgoWidget = 
    new QGoAlgorithmsManagerWidget("Merge", iParent);
  this->m_MeshEditingWidget->AddMode(MergeAlgoWidget);

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::ApplyDanielAlgo()
{
  this->GetPolydatasFromAlgo<QGoMeshSplitDanielssonDistanceAlgo>(this->m_DanielAlgo);
}