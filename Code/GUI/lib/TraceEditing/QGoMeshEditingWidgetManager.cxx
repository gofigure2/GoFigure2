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

#include <QDebug>

#include "QGoMeshWaterShedAlgo.h"
#include "GoImageProcessor.h"


QGoMeshEditingWidgetManager::QGoMeshEditingWidgetManager(
  std::vector<QString> iVectChannels,
  int iTimeMin,
  int iTimeMax,
  std::vector< vtkPoints* >* iSeeds,
  GoImageProcessor* iImages,
  int* iCurrentTimePoint,
  QWidget* iParent): QGoTraceEditingWidgetManager("Mesh",
  iVectChannels, iTimeMin, iTimeMax, iSeeds, iImages,
  iCurrentTimePoint, iParent)
{
  this->SetSemiAutomaticAlgorithms(iParent);
  this->SetSplitMergeMode(iVectChannels, this->m_ListTimePoint,iParent);
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
  std::map<QString, QColor> iListTimePoints, int iChannelNumber)
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
  this->m_WaterShedAlgo = new QGoMeshWaterShedAlgo(this->m_Seeds,
                                                   this->m_MaxThreshold,
                                                   iParent);
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
  // initialize the widget
  m_SetOfContoursWidget =
    new QGoAlgorithmsManagerWidget("Set of Contours",
    iParent, iVectChannels, iListTime);

  this->SetTSliceForClassicView();

  this->m_TraceEditingWidget->AddMode(m_SetOfContoursWidget, true);

  //watershed
  this->m_SetOfContoursWaterShedAlgo =
    new QGoSetOfContoursWaterShedAlgo(this->m_Seeds,
                                      this->m_MaxThreshold,
                                      iParent);
  QGoAlgorithmWidget* SetOfContoursWaterShedWidget =
    this->m_SetOfContoursWaterShedAlgo->GetAlgoWidget();
  this->m_SetOfContoursWidget->AddMethod(SetOfContoursWaterShedWidget);

  QObject::connect(SetOfContoursWaterShedWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplySetOfContoursWaterShedAlgo() ) );

  //levelset
  this->m_SetOfContoursLevelSetAlgo =
    new QGoSetOfContoursLevelSetAlgo(this->m_Seeds, iParent);
  QGoAlgorithmWidget* SetOfContoursLevelSetWidget =
    this->m_SetOfContoursLevelSetAlgo->GetAlgoWidget();
  this->m_SetOfContoursWidget->AddMethod(SetOfContoursLevelSetWidget);

  QObject::connect(SetOfContoursLevelSetWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplySetOfContoursLevelSetAlgo() ) );

  //shape
  this->m_SetOfContoursShapeAlgo =
    new QGoSetOfContoursShapeAlgo(this->m_Seeds, iParent);
  QGoAlgorithmWidget* SetOfContoursShapeWidget =
    this->m_SetOfContoursShapeAlgo->GetAlgoWidget();
  this->m_SetOfContoursWidget->AddMethod(SetOfContoursShapeWidget);

  QObject::connect(SetOfContoursShapeWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplySetOfContoursShapeAlgo() ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetSplitMergeMode(
    std::vector<QString> iVectChannels, QStringList iListTime,
    QWidget* iParent)
{
  QGoAlgorithmsManagerWidget* SplitAlgoWidget =
    new QGoAlgorithmsManagerWidget("Split", iParent, iVectChannels);
  this->m_TraceEditingWidget->AddMode(SplitAlgoWidget, true);

  m_DanielAlgo = new QGoMeshSplitDanielssonDistanceAlgo(this->m_Seeds,
                                                        iParent);
  QGoAlgorithmWidget * DanielWidget = m_DanielAlgo->GetAlgoWidget();

  SplitAlgoWidget->AddMethod(DanielWidget );

  QObject::connect( DanielWidget, SIGNAL(ApplyAlgo() ) ,
                    this, SLOT(RequestPolydatasForDanielsson() ) );

  QGoAlgorithmsManagerWidget* MergeAlgoWidget =
    new QGoAlgorithmsManagerWidget("Merge", iParent, iVectChannels);
  this->m_TraceEditingWidget->AddMode(MergeAlgoWidget, false);

  m_ConvexHullAlgo = new QGoMeshMergeConvexHullAlgo(this->m_Seeds,
                                                        iParent);
  QGoAlgorithmWidget * ConvexHullWidget = m_ConvexHullAlgo->GetAlgoWidget();

  MergeAlgoWidget->AddMethod(ConvexHullWidget );

  QObject::connect( ConvexHullWidget, SIGNAL(ApplyAlgo() ) ,
                    this, SLOT(RequestPolydatasForConvexHull() ) );

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::RequestPolydatasForDanielsson(){
  m_TempReference = dynamic_cast<QGoSplitSegmentationAlgo*>(m_DanielAlgo);
  emit RequestPolydatas(1);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::RequestPolydatasForConvexHull(){
  m_TempReference = dynamic_cast<QGoSplitSegmentationAlgo*>(m_ConvexHullAlgo);
  emit RequestPolydatas(2);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoMeshEditingWidgetManager::
RequestedPolydatas(std::list< std::pair<unsigned int, vtkPolyData*> > iRequest){
  // in split mode
  if(iRequest.size() == 1)
    {
    std::vector<vtkPolyData*> polys;
    polys.push_back(iRequest.front().second);
    emit UpdateSeeds();
    std::vector<vtkPolyData*> NewTraces = m_TempReference->ApplyAlgo(
      this->m_Images,
      this->m_TraceEditingWidget->GetCurrentImageName(),
      polys,
      this->m_TraceEditingWidget->GetIsInvertedOn());
    emit TracesSplittedFromAlgo(NewTraces);
    emit ClearAllSeeds();
    }
  // merge mode
  else
    {
    std::vector<vtkPolyData*> polys;
    polys.push_back(iRequest.front().second);
    polys.push_back(iRequest.back().second);
    std::vector<vtkPolyData*> NewTraces = m_TempReference->ApplyAlgo(
      this->m_Images,
      this->m_TraceEditingWidget->GetCurrentImageName(),
      polys,
      this->m_TraceEditingWidget->GetIsInvertedOn());
    emit TracesMergedFromAlgo(NewTraces);
    }


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
