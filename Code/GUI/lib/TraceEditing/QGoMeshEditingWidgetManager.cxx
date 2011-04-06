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
  QWidget* iParent): QGoTraceEditingWidgetManager("Mesh", 
  iVectChannels, iTimeMin, iTimeMax, iSeeds, iImages, 
  iCurrentTimePoint, iParent)
{
  this->SetSemiAutomaticAlgorithms(iParent);
  this->SetSplitMergeMode(iParent);
  this->SetSetOfContoursAlgorithms(iVectChannels, this->m_ListTimePoint, 
    iParent);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoMeshEditingWidgetManager::~QGoMeshEditingWidgetManager()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetTSliceForClassicView()
{
  QGoTraceEditingWidgetManager::SetTSliceForClassicView();
  if (this->m_SetOfContoursWidget != NULL)
    {
    this->m_SetOfContoursWidget->SetTSliceForClassicView(
      tr("%1").arg(*this->m_CurrentTimePoint) );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetTSliceForDopplerView(
  QStringList iListTimePoints, int iChannelNumber)
{
  QGoTraceEditingWidgetManager::SetTSliceForDopplerView(
    iListTimePoints, iChannelNumber);
  this->m_SetOfContoursWidget->SetTSliceForDopplerView(
    iListTimePoints, iChannelNumber);
} 
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetSemiAutomaticAlgorithms(QWidget* iParent)
{
  //level set:
  m_LevelSetAlgo = new QGoMeshLevelSetAlgo(this->m_Seeds, iParent);
  QGoAlgorithmWidget* LevelSetWidget = m_LevelSetAlgo->GetAlgoWidget();
  this->m_TraceEditingWidget->AddAlgoWidgetForSemiAutomaticMode(LevelSetWidget);

  QObject::connect(LevelSetWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplyLevelSetAlgo() ) );

  //shape:
  this->m_ShapeAlgo = new QGoMeshShapeAlgo(this->m_Seeds, iParent);
  QGoAlgorithmWidget* ShapeWidget = this->m_ShapeAlgo->GetAlgoWidget();
  this->m_TraceEditingWidget->AddAlgoWidgetForSemiAutomaticMode(ShapeWidget);

  QObject::connect(ShapeWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplyShapeAlgo() ) );

  //watershed:
  this->m_WaterShedAlgo = new QGoMeshWaterShedAlgo(this->m_Seeds, iParent);
  QGoAlgorithmWidget* WaterShedWidget = m_WaterShedAlgo->GetAlgoWidget();
  this->m_TraceEditingWidget->AddAlgoWidgetForSemiAutomaticMode(WaterShedWidget);

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

  this->SetTSliceForClassicView();

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

  this->m_TraceEditingWidget->AddMode(m_SetOfContoursWidget, true);

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
  this->m_TraceEditingWidget->AddMode(SplitAlgoWidget, true);
  
  m_DanielAlgo = new QGoMeshSplitDanielssonDistanceAlgo(iParent);
  QGoAlgorithmWidget * DanielWidget = m_DanielAlgo->GetAlgoWidget();
  SplitAlgoWidget->AddMethod(DanielWidget );

  QObject::connect( DanielWidget, SIGNAL(ApplyAlgo() ) , 
                    this, SLOT(ApplyDanielAlgo() ) );

  QGoAlgorithmsManagerWidget* MergeAlgoWidget = 
    new QGoAlgorithmsManagerWidget("Merge", iParent);
  this->m_TraceEditingWidget->AddMode(MergeAlgoWidget, true);

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
}