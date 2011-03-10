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
#include "QGoModeEditingWidget.h"
#include <QLabel>

QGoModeEditingWidget::QGoModeEditingWidget(QWidget *iParent)
  :QWidget(iParent)
{
  this->Initialize();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoModeEditingWidget::~QGoModeEditingWidget()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoModeEditingWidget::Initialize()
{
  this->m_VBoxLayout = new QVBoxLayout;
  this->m_ModeComboBox = new QComboBox(this);

  QHBoxLayout* ModeLayout = new QHBoxLayout;
  QLabel* ModeLabel = new QLabel(tr("Mode:"), this);
  ModeLayout->addWidget(ModeLabel);
  ModeLayout->addWidget(this->m_ModeComboBox);

  //this->m_VBoxLayout->addWidget(this->m_ModeComboBox);
  this->m_VBoxLayout->addLayout(ModeLayout);

  this->m_ModeWidgets = new QStackedWidget;
  this->m_VBoxLayout->addWidget(this->m_ModeWidgets);

  this->m_VBoxLayout->setSizeConstraint(QLayout::SetFixedSize);

  this->setLayout(this->m_VBoxLayout);

  QObject::connect(this->m_ModeComboBox, SIGNAL(activated(int)),
             this->m_ModeWidgets, SLOT(setCurrentIndex(int)));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoModeEditingWidget::AddWidgetWithModeName(
  std::string iModeName, QWidget* iWidget )
{
  int Index = 0;
  if (iWidget != 0)
  {
  this->m_ModeWidgets->addWidget(iWidget);  
  Index = this->m_ModeWidgets->indexOf(iWidget);
  }
  this->m_ModeComboBox->insertItem(Index,iModeName.c_str());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoModeEditingWidget::AddAlgoManagerWidget(
    QGoAlgorithmsManagerWidget* iAlgoManagerWidget,int iCurrentIndex)
{
  this->AddWidgetWithModeName(iAlgoManagerWidget->GetModeName(), 
    iAlgoManagerWidget);
  iAlgoManagerWidget->SetCurrentIndex(iCurrentIndex);
}