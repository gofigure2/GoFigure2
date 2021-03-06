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
#include "QGoModesManagerWidget.h"
#include <QLabel>
#include <QColor>

QGoModesManagerWidget::QGoModesManagerWidget(std::vector<QString> iVectChannels,
  QStringList iListTimePoints, QWidget *iParent)
  :QWidget(iParent), m_ManualModeManager(NULL)
{
  this->Initialize(iVectChannels, iListTimePoints);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoModesManagerWidget::~QGoModesManagerWidget()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoModesManagerWidget::Initialize(std::vector<QString> iVectChannels,
  QStringList iListTimePoints)
{
  this->m_ModeAlreadyCleaned = false;
  this->m_VBoxLayout = new QVBoxLayout;
  this->m_ModeComboBox = new QComboBox(this);

  QHBoxLayout* ModeLayout = new QHBoxLayout;
  QLabel* ModeLabel = new QLabel(tr("Mode:"), this);
  ModeLayout->addWidget(ModeLabel);
  ModeLayout->addWidget(this->m_ModeComboBox);

  this->m_VBoxLayout->addLayout(ModeLayout);

  this->m_ModeWidgets = new QStackedWidget;
  this->m_VBoxLayout->addWidget(this->m_ModeWidgets);

  this->setLayout(this->m_VBoxLayout);
  this->m_VBoxLayout->setSizeConstraint(QLayout::SetFixedSize);

  //add default modes:
  this->m_SemiAutoAlgoManagerWidget = new
    QGoAlgorithmsManagerWidget("SemiAutomatic", this, iVectChannels,
    iListTimePoints);
  this->AddAlgoManagerWidget(this->m_SemiAutoAlgoManagerWidget, true);

  this->m_AutoAlgoManagerWidget = new
    QGoAlgorithmsManagerWidget("Automatic",this, iVectChannels,
    iListTimePoints);
  this->AddAlgoManagerWidget(this->m_AutoAlgoManagerWidget, false);

  m_ModesWhoNeedSeeds.append("SemiAutomatic");
  m_ModesWhoNeedSeeds.append("Automatic");

  QObject::connect(this->m_ModeComboBox, SIGNAL(activated(int)),
                   this, SLOT(SetTheRightMode(int)));

  QObject::connect(this->m_AutoAlgoManagerWidget, SIGNAL(ResetClicked() ),
                   this, SIGNAL(ResetClicked() ) );

  QObject::connect(this->m_SemiAutoAlgoManagerWidget, SIGNAL(ResetClicked() ),
                   this, SIGNAL(ResetClicked() ) );

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoModesManagerWidget::AddWidgetWithModeName(
  std::string iModeName, QWidget* iWidget, bool ModeNeedSeeds )
{
  int Index = 0;
  if (iWidget != 0)
    {
    this->m_ModeWidgets->addWidget(iWidget);
    Index = this->m_ModeWidgets->indexOf(iWidget);
    }
  this->m_ModeComboBox->insertItem(Index,iModeName.c_str());

  if (ModeNeedSeeds)
    {
    this->m_ModesWhoNeedSeeds.append(iModeName.c_str() );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoModesManagerWidget::AddAlgoManagerWidget(
    QGoAlgorithmsManagerWidget* iAlgoManagerWidget, bool ModeNeedSeeds,
    int iDefaultIndex)
{
  this->AddWidgetWithModeName(iAlgoManagerWidget->GetModeName(),
    iAlgoManagerWidget, ModeNeedSeeds);
  iAlgoManagerWidget->SetCurrentIndex(iDefaultIndex);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoModesManagerWidget::CheckDefaultModes()
{
  if (!this->m_AutoAlgoManagerWidget->HasMethod())
    {
    this->m_ModeComboBox->removeItem(
      this->m_ModeComboBox->findText("Automatic") );
    this->m_ModeWidgets->removeWidget(this->m_AutoAlgoManagerWidget);
    }
   if (!this->m_SemiAutoAlgoManagerWidget->HasMethod())
    {
    this->m_ModeComboBox->removeItem(
      this->m_ModeComboBox->findText("SemiAutomatic") );
    this->m_ModeWidgets->removeWidget(this->m_SemiAutoAlgoManagerWidget);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoModesManagerWidget::AddAlgoWidgetForSemiAutomaticMode(
  QGoAlgorithmWidget* iAlgoWidget)
{
  this->m_SemiAutoAlgoManagerWidget->AddMethod(iAlgoWidget);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoModesManagerWidget::AddAlgoWidgetForAutomaticMode(
  QGoAlgorithmWidget* iAlgoWidget)
{
  this->m_AutoAlgoManagerWidget->AddMethod(iAlgoWidget);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoModesManagerWidget::AddWidgetForManualMode(QWidget* iWidget,
  QStringList iListTimePoint, bool ModeNeedSeeds)
{
  std::vector<QString> Channels = std::vector<QString>();
  this->m_ManualModeManager = new QGoAlgorithmsManagerWidget(
    "Manual",this, Channels, iListTimePoint, true, false);
  this->m_ManualModeManager->AddWidgetForOnlyOneMethod(iWidget);
  this->AddAlgoManagerWidget(this->m_ManualModeManager, ModeNeedSeeds);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoModesManagerWidget::GetCurrentImageName()
{
  QGoAlgorithmsManagerWidget* CurrentWidget =
    dynamic_cast<QGoAlgorithmsManagerWidget*>
      (this->m_ModeWidgets->currentWidget());
  return CurrentWidget->GetCurrentImageName();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoModesManagerWidget::GetSelectedTimePoint()
{
  QGoAlgorithmsManagerWidget* CurrentWidget =
    dynamic_cast<QGoAlgorithmsManagerWidget*>
      (this->m_ModeWidgets->currentWidget());
  return CurrentWidget->GetSelectedTimePoint();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoModesManagerWidget::GetIsInvertedOn()
{
  QGoAlgorithmsManagerWidget* CurrentWidget =
    dynamic_cast<QGoAlgorithmsManagerWidget*>
      (this->m_ModeWidgets->currentWidget());
  return CurrentWidget->IsInvertChecked();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoModesManagerWidget::SetTheRightMode(int iIndex)
{
  if(! m_ModeAlreadyCleaned)
    {
      this->CheckDefaultModes();
      m_ModeAlreadyCleaned = true;
    }

  if (iIndex != -1)
    {
    this->m_ModeWidgets->setCurrentIndex(iIndex);
    }

  if (this->m_ModesWhoNeedSeeds.indexOf(
        this->m_ModeComboBox->currentText() ) != -1)
    {
    emit SetSeedInteractorBehaviour(true);
    }
  else
    {
    emit SetSeedInteractorBehaviour(false);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoModesManagerWidget::SetTSliceForClassicViewInAllAlgoModes(int iTimePoint)
{
  this->m_AutoAlgoManagerWidget->SetTSliceForClassicView(tr("%1").arg(iTimePoint) );
  this->m_SemiAutoAlgoManagerWidget->SetTSliceForClassicView(tr("%1").arg(iTimePoint) );
  if (this->m_ManualModeManager)
    {
    this->m_ManualModeManager->SetTSliceForClassicView(tr("%1").arg(iTimePoint) );
    }

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoModesManagerWidget::SetTSliceForDopplerViewInAllAlgoModes(
  QHash<QString, QColor> iListTimePoints, int iChannelNumber)
{
  this->m_AutoAlgoManagerWidget->SetTSliceForDopplerView(
    iListTimePoints, iChannelNumber);
  this->m_SemiAutoAlgoManagerWidget->SetTSliceForDopplerView(
    iListTimePoints, iChannelNumber);
  if (this->m_ManualModeManager)
    {
    this->m_ManualModeManager->SetTSliceForDopplerView(
      iListTimePoints, iChannelNumber);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
 std::string QGoModesManagerWidget::GetCurrentModeName()
{
  return this->m_ModeComboBox->currentText().toStdString();
}
