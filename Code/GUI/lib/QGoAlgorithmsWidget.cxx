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

#include "QGoAlgorithmsWidget.h"
#include <QStackedLayout>
#include <QLabel>
#include <QGroupBox>
#include "QGoAdvancedParametersWidget.h"


QGoAlgorithmsWidget::QGoAlgorithmsWidget(QWidget *iParent )
  :QWidget(iParent)
{
  this->Initialize();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoAlgorithmsWidget::~QGoAlgorithmsWidget()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAlgorithmsWidget::Initialize()
{
  this->m_VBoxLayout = new QVBoxLayout;
  this->m_MethodComboBox = new QComboBox(this);
  this->m_MethodWidgets = new QStackedWidget(this);
  
  this->m_VBoxLayout->addWidget(this->m_MethodComboBox);
  this->m_VBoxLayout->addWidget(this->m_MethodWidgets);
  this->setLayout(this->m_VBoxLayout);
  this->m_VBoxLayout->setSizeConstraint(QLayout::SetFixedSize);
  
  QObject::connect(this->m_MethodComboBox, SIGNAL(activated(int)),
             this->m_MethodWidgets, SLOT(setCurrentIndex(int)));

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAlgorithmsWidget::AddMethod(std::string iNameMethod, 
  QWidget* iParametersWidget, QWidget* iAdvParamWidget)
{
  QWidget* MethodWidget = new QWidget(this);

  QVBoxLayout* MethodLayout = new QVBoxLayout;
  MethodLayout->addWidget(iParametersWidget);
  
  QGoAdvancedParametersWidget* AdvParamWidget = 
    new QGoAdvancedParametersWidget(this);
  AdvParamWidget->AddAdvancedParamWidget(iAdvParamWidget);
  

  MethodLayout->addWidget(AdvParamWidget);

  MethodWidget->setLayout(MethodLayout);
  this->m_MethodWidgets->addWidget(MethodWidget);

  int Index = this->m_MethodWidgets->indexOf(MethodWidget);
  this->m_MethodComboBox->insertItem(Index,iNameMethod.c_str());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------