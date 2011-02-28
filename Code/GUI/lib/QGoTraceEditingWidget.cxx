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
#include "QGoTraceEditingWidget.h"
#include "QGoAdvancedParametersWidget.h"
#include "QGoModeEditingWidget.h"
#include <QPushButton>

QGoTraceEditingWidget::QGoTraceEditingWidget(
  std::string iTraceName, QWidget *iParent)
{
  this->Initialize(iParent);
  this->setWindowTitle(tr("%1 Editing").arg(iTraceName.c_str()));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTraceEditingWidget::~QGoTraceEditingWidget()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceEditingWidget::Initialize(QWidget *iParent)
{
  this->m_ModeEditingWidget = new QGoModeEditingWidget(this);
  this->m_VLayout = new QVBoxLayout;
  this->m_VLayout->addWidget(this->m_ModeEditingWidget);
  QPushButton* ApplyButton = new QPushButton(tr("Apply"),this);
  this->m_VLayout->addWidget(ApplyButton);
  this->setLayout(this->m_VLayout);
  this->m_VLayout->setSizeConstraint(QLayout::SetFixedSize);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceEditingWidget:: AddMode(
  std::string iModeName, QWidget* iModeWidget)
{
  QWidget* ModeWidget = new QWidget;
  if (iModeWidget != 0)
    {
    ModeWidget = iModeWidget;
    }
  this->m_ModeEditingWidget->AddWidgetWithModeName(iModeName, ModeWidget); 
}