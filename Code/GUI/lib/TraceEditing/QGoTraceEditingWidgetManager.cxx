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
#include "QGoTraceEditingWidgetManager.h"
#include "QGoAlgorithmWidget.h"
#include "QGoAlgoParameter.h"
#include "QGoAlgorithmsManagerWidget.h"
#include "vtkSmartPointer.h"
#include "vtkImageExport.h"
#include "vtkImageData.h"
#include <iostream>

#include "GoImageProcessor.h"


QGoTraceEditingWidgetManager::QGoTraceEditingWidgetManager(
  std::string iTraceName,
  std::vector<QString> iVectChannels,
  int iTimeMin,
  int iTimeMax,
  std::vector< vtkPoints* >* iSeeds,
  GoImageProcessor* iImages,
  int* iCurrentTimePoint,
  QWidget* iParent)
{
  this->m_TraceName = iTraceName;
  this->m_Seeds = iSeeds;
  this->m_Images = iImages;
  this->m_CurrentTimePoint = iCurrentTimePoint;

  this->SetTheTraceWidget(iVectChannels, iTimeMin, iTimeMax, iParent);
  this->SetTheDockWidget(iParent);

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTraceEditingWidgetManager::~QGoTraceEditingWidgetManager()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceEditingWidgetManager::SetTheTraceWidget(
  std::vector<QString> iVectChannels, int iTimeMin,
  int iTimeMax, QWidget* iParent)
{

  QStringList ListTimePoints;
  for (int i = iTimeMin; i < iTimeMax+1; ++i)
    {
    ListTimePoints.push_back(tr("%1").arg(i));
    }
  this->m_ListTimePoint = ListTimePoints;

  this->m_TraceEditingWidget = new QGoTraceEditingWidget(
   this->m_TraceName.c_str(), iVectChannels, ListTimePoints, iParent);

  QObject::connect( this->m_TraceEditingWidget,
                    SIGNAL(SetSeedInteractorBehaviour(bool) ),
                    this,
                    SIGNAL(SetSeedInteractorBehaviour(bool) ) );

  QObject::connect( this->m_TraceEditingWidget,
                    SIGNAL(ResetClicked() ),
                    this, SIGNAL(ClearAllSeeds() ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceEditingWidgetManager::SetTheDockWidget(QWidget* iParent)
{

  this->m_TraceEditingDockWidget = new QGoDockWidget(iParent);
  std::string WindowTitle = this->m_TraceName;
  WindowTitle += " Editing";
  this->m_TraceEditingDockWidget->setWindowTitle(WindowTitle.c_str());
  this->m_TraceEditingDockWidget->setWidget(this->m_TraceEditingWidget);

  QIcon TraceSegmentationIcon;
  std::string PathIcon = ":/fig/";
  PathIcon += this->m_TraceName;
  PathIcon += "Editing.png";

  TraceSegmentationIcon.addPixmap(QPixmap( QString::fromUtf8(PathIcon.c_str() ) ),
                                 QIcon::Normal, QIcon::Off);

 this->m_TraceEditingDockWidget->toggleViewAction()->setIcon(TraceSegmentationIcon);
 this->m_TraceEditingDockWidget->toggleViewAction()->setToolTip( tr("%1 Editing").arg(this->m_TraceName.c_str() ) );
 this->m_TraceEditingDockWidget->toggleViewAction()->setStatusTip(
    tr("Create %1s manually, semi-automatically or automatically").arg(this->m_TraceName.c_str() ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QAction* QGoTraceEditingWidgetManager::GetToggleViewAction()
{
  return this->m_TraceEditingDockWidget->toggleViewAction();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoDockWidget* QGoTraceEditingWidgetManager::GetDockWidget()
{
  return this->m_TraceEditingDockWidget;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceEditingWidgetManager::SetVisible(bool isVisible)
{
  this->m_TraceEditingDockWidget->setVisible(isVisible);
  this->m_TraceEditingWidget->CheckTheCurrentMode(isVisible);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceEditingWidgetManager::SetTSliceForClassicView()
{
  this->m_TraceEditingWidget->SetTSliceForClassicView(
    *this->m_CurrentTimePoint);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceEditingWidgetManager::SetTSliceForDopplerView(
  std::map<QString, QColor> iListTimePoints, int iChannelNumber)
{
  this->m_TraceEditingWidget->SetTSliceForDopplerView(
    iListTimePoints, iChannelNumber);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoTraceEditingWidgetManager::GetSelectedTimePoint()
{
  return this->m_TraceEditingWidget->GetSelectedTimePoint();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoMeshEditingWidgetManager::SetSemiAutomaticAlgorithms(QWidget* iParent)
{
  //level set:
  m_LevelSetAlgo = new QGoMeshLevelSetAlgo(this->m_Seeds, iParent);
  QGoAlgorithmWidget* LevelSetWidget = m_LevelSetAlgo->GetAlgoWidget();
  this->m_MeshEditingWidget->AddAlgoWidgetForSemiAutomaticMode(LevelSetWidget);

  QObject::connect(LevelSetWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplyLevelSetAlgo() ) );

  //shape:
  this->m_ShapeAlgo = new QGoMeshShapeAlgo(this->m_Seeds, iParent);
  QGoAlgorithmWidget* ShapeWidget = this->m_ShapeAlgo->GetAlgoWidget();
  this->m_MeshEditingWidget->AddAlgoWidgetForSemiAutomaticMode(ShapeWidget);

  QObject::connect(ShapeWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplyShapeAlgo() ) );

  //watershed:
  this->m_WaterShedAlgo = new QGoMeshWaterShedAlgo(this->m_Seeds, iParent);
  QGoAlgorithmWidget* WaterShedWidget = m_WaterShedAlgo->GetAlgoWidget();
  this->m_MeshEditingWidget->AddAlgoWidgetForSemiAutomaticMode(WaterShedWidget);

  QObject::connect(WaterShedWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplyWaterShedAlgo() ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetSetOfContoursAlgorithms(
   std::vector<QString> iVectChannels, QStringList iListTime,
   QWidget* iParent)
{
  m_SetOfContoursWidget =
    new QGoAlgorithmsManagerWidget("Set of Contours",
    iParent, iVectChannels, iListTime);

  this->m_SetOfContoursWaterShedAlgo =
    new QGoSetOfContoursWaterShedAlgo(this->m_Seeds, iParent);
  QGoAlgorithmWidget* SetOfContoursWaterShedWidget =
    this->m_SetOfContoursWaterShedAlgo->GetAlgoWidget();
  this->m_SetOfContoursWidget->AddMethod(SetOfContoursWaterShedWidget);

  this->m_SetOfContoursLevelSetAlgo =
    new QGoSetOfContoursLevelSetAlgo(this->m_Seeds, iParent);
  QGoAlgorithmWidget* SetOfContoursLevelSetWidget =
    this->m_SetOfContoursLevelSetAlgo->GetAlgoWidget();
  this->m_SetOfContoursWidget->AddMethod(SetOfContoursLevelSetWidget);

  this->m_SetOfContoursShapeAlgo =
    new QGoSetOfContoursShapeAlgo(this->m_Seeds, iParent);
  QGoAlgorithmWidget* SetOfContoursShapeWidget =
    this->m_SetOfContoursShapeAlgo->GetAlgoWidget();
  this->m_SetOfContoursWidget->AddMethod(SetOfContoursShapeWidget);

  this->m_MeshEditingWidget->AddMode(m_SetOfContoursWidget, true);

  QObject::connect(SetOfContoursWaterShedWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplySetOfContoursWaterShedAlgo() ) );

  QObject::connect(SetOfContoursLevelSetWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplySetOfContoursLevelSetAlgo() ) );

  QObject::connect(SetOfContoursShapeWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplySetOfContoursShapeAlgo() ) );


}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetSplitMergeMode(QWidget* iParent)
{
  QGoAlgorithmsManagerWidget* SplitAlgoWidget =
    new QGoAlgorithmsManagerWidget("Split", iParent);
  this->m_MeshEditingWidget->AddMode(SplitAlgoWidget, true);

  m_DanielAlgo = new QGoMeshSplitDanielssonDistanceAlgo(iParent);
  QGoAlgorithmWidget * DanielWidget = m_DanielAlgo->GetAlgoWidget();
  SplitAlgoWidget->AddMethod(DanielWidget );

  QObject::connect( DanielWidget, SIGNAL(ApplyAlgo() ) ,
                    this, SLOT(ApplyDanielAlgo() ) );

  QGoAlgorithmsManagerWidget* MergeAlgoWidget =
    new QGoAlgorithmsManagerWidget("Merge", iParent);
  this->m_MeshEditingWidget->AddMode(MergeAlgoWidget, true);

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::ApplyDanielAlgo()
{
  this->GetPolydatasFromAlgo<QGoMeshSplitDanielssonDistanceAlgo>(this->m_DanielAlgo);
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
void QGoMeshEditingWidgetManager::ApplySetOfContoursWaterShedAlgo()
{
  this->GetSetOfPolydatasFromAlgo<QGoSetOfContoursWaterShedAlgo>(
    this->m_SetOfContoursWaterShedAlgo);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::ApplySetOfContoursLevelSetAlgo()
{
  this->GetSetOfPolydatasFromAlgo<QGoSetOfContoursLevelSetAlgo>(
    this->m_SetOfContoursLevelSetAlgo);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::ApplySetOfContoursShapeAlgo()
{
  this->GetSetOfPolydatasFromAlgo<QGoSetOfContoursShapeAlgo>(
    this->m_SetOfContoursShapeAlgo);
}*/
