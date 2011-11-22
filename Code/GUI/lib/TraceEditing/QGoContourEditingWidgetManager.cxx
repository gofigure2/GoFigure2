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
#include "QGoContourEditingWidgetManager.h"
#include "QGoAlgorithmWidget.h"
#include "QGoAlgoParameter.h"
#include "QGoAlgorithmsManagerWidget.h"
#include "GoImageProcessor.h"

#include "vtkSmartPointer.h"
#include "vtkImageExport.h"
#include "vtkImageData.h"
#include <iostream>


QGoContourEditingWidgetManager::QGoContourEditingWidgetManager(
  std::vector<QString> iVectChannels,
  int iTimeMin,
  int iTimeMax,
  std::vector< vtkPoints* >* iSeeds,
  GoImageProcessor* iImages,
  int* iCurrentTimePoint,
  QWidget* iParent): QGoTraceEditingWidgetManager("Contour",
    iVectChannels, iTimeMin, iTimeMax, iSeeds, iImages,
    iCurrentTimePoint, iParent)
{
  this->SetSemiAutomaticAlgorithms(iParent);
  this->SetManualMode(this->m_ListTimePoint, iParent);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoContourEditingWidgetManager::~QGoContourEditingWidgetManager()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoContourEditingWidgetManager::SetManualMode(
  QStringList iListTimePoint, QWidget* iParent)
{
  m_ManualMode = new QGoContourManualSegmentation(iParent);

  this->m_TraceEditingWidget->AddWidgetForManualMode(m_ManualMode->getWidget(),
    iListTimePoint, false);

  this->SetTSliceForClassicView();

  QObject::connect( this->m_ManualMode,
                    SIGNAL (changeContourRepresentationProperty(float, QColor,
                                           QColor, QColor) ),
                    this, SIGNAL(changeContourRepresentationProperty(float, QColor,
                                           QColor, QColor) ) );

  QObject::connect( this->m_ManualMode,
                    SIGNAL (validateContour() ),
                    this, SLOT(ContourToValidate() ) );

  QObject::connect( this->m_ManualMode,
                    SIGNAL (reinitializeContour() ),
                    this, SIGNAL(reinitializeContour() ) );

   QObject::connect( this->m_TraceEditingWidget,
                    SIGNAL(SetSeedInteractorBehaviour(bool) ),
                    this,
                    SLOT(StartManualSegmentation(bool) ) );

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoContourEditingWidgetManager::StartManualSegmentation( bool iValue )
{
  // if current is
  if(this->m_TraceEditingWidget->GetCurrentModeName().compare("Manual") == 0)
    {
    emit ManualSegmentationActivated( true );
    }
  else
    {
    emit ManualSegmentationActivated( false );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoContourEditingWidgetManager::ContourToValidate()
{
  int CurrentTimePoint = this->GetSelectedTimePoint(); //for test purpose
  emit ContourValidated( CurrentTimePoint );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoContourEditingWidgetManager::SetSemiAutomaticAlgorithms(QWidget* iParent)
{
  //level set:
  m_LevelSetAlgo = new QGoContourLevelSetAlgo(this->m_Seeds, iParent);
  QGoAlgorithmWidget* LevelSetWidget = m_LevelSetAlgo->GetAlgoWidget();
  this->m_TraceEditingWidget->AddAlgoWidgetForSemiAutomaticMode(LevelSetWidget);

  QObject::connect(LevelSetWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplyLevelSetAlgo() ) );

  /*//shape:
  this->m_ShapeAlgo = new QGoMeshShapeAlgo(this->m_Seeds, iParent);
  QGoAlgorithmWidget* ShapeWidget = this->m_ShapeAlgo->GetAlgoWidget();
  this->m_TraceEditingWidget->AddAlgoWidgetForSemiAutomaticMode(ShapeWidget);

  QObject::connect(ShapeWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplyShapeAlgo() ) );

  //watershed:
  this->m_WaterShedAlgo = new QGoMeshWaterShedAlgo(this->m_Seeds,
this->m_MaxThreshold, iParent);
  QGoAlgorithmWidget* WaterShedWidget = m_WaterShedAlgo->GetAlgoWidget();
  this->m_TraceEditingWidget->AddAlgoWidgetForSemiAutomaticMode(WaterShedWidget);

  QObject::connect(WaterShedWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(ApplyWaterShedAlgo() ) );*/
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoContourEditingWidgetManager::ApplyLevelSetAlgo()
{
  this->GetPolydatasFromAlgo<QGoContourLevelSetAlgo>(this->m_LevelSetAlgo);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoContourEditingWidgetManager::ApplyShapeAlgo()
{
  this->GetPolydatasFromAlgo<QGoContourShapeAlgo>(this->m_ShapeAlgo);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoContourEditingWidgetManager::ApplyWaterShedAlgo()
{
  this->GetPolydatasFromAlgo<QGoMeshWaterShedAlgo>(this->m_WaterShedAlgo);
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoContourEditingWidgetManager::SetReeditMode(bool iReeditMode)
{
  this->m_ManualMode->SetReeditMode(iReeditMode);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoContourEditingWidgetManager::GetReeditMode()
{
  return this->m_ManualMode->GetReeditMode();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoContourEditingWidgetManager::InitializeSettingsForManualMode()
{
  this->m_ManualMode->GenerateContourRepresentationProperties(true);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoContourEditingWidgetManager::SetVisible(bool isVisible)
{
  QGoTraceEditingWidgetManager::SetVisible(isVisible);

  if (this->m_TraceEditingWidget->GetCurrentModeName() == "Manual")
    {
    emit ManualSegmentationActivated(isVisible);
    }
}
