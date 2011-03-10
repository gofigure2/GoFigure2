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

#include "QGoAlgorithmsManagerWidget.h"
#include <QStackedLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include "QGoAdvancedParametersWidget.h"


QGoAlgorithmsManagerWidget::QGoAlgorithmsManagerWidget(std::string iModeName,
  QStringList iListChannels, QStringList iListTime, QWidget *iParent )
  :QWidget(iParent)
{
  this->m_ModeName = iModeName;
  this->Initialize(iListChannels, iListTime);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoAlgorithmsManagerWidget::~QGoAlgorithmsManagerWidget()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAlgorithmsManagerWidget::Initialize(QStringList iListChannels, 
  QStringList iListTime)
{
  this->m_VBoxLayout = new QVBoxLayout;
  QFormLayout* FormLayout = new QFormLayout;
 
  this->m_ChannelComboBox = new QComboBox(this);
  this->m_ChannelComboBox->addItems(iListChannels);
  FormLayout->addRow(tr("Channel:"), this->m_ChannelComboBox);

  this->m_TimeComboBox = new QComboBox(this);
  this->m_TimeComboBox->addItems(iListTime);
  FormLayout->addRow(tr("TSlice:"), this->m_TimeComboBox);

  this->m_MethodComboBox = new QComboBox(this);
  FormLayout->addRow(tr("Method:"), this->m_MethodComboBox);

  this->m_MethodWidgets = new QStackedWidget(this);
  //this->m_MethodsLayout = new QStackedLayout(this->m_VBoxLayout);

  QHBoxLayout* ButtonLayout = new QHBoxLayout;
  QPushButton* ApplyButton = new QPushButton(tr("Apply"),this);
  QPushButton* ResetButton = new QPushButton(tr("Reset"), this);
  ButtonLayout->addWidget(ApplyButton);
  ButtonLayout->addWidget(ResetButton);

  this->m_VBoxLayout->addLayout(FormLayout);
  this->m_VBoxLayout->addWidget(this->m_MethodWidgets);
  this->m_VBoxLayout->addLayout(ButtonLayout);
  //this->m_VBoxLayout->addLayout(this->m_MethodsLayout);
  
 

  this->setLayout(this->m_VBoxLayout);
  this->m_VBoxLayout->setSizeConstraint(QLayout::SetFixedSize);

  QObject::connect(this->m_MethodComboBox, SIGNAL(activated(int)),
            this->m_MethodWidgets, SLOT(setCurrentIndex(int)));

  QObject::connect(ApplyButton, SIGNAL(clicked()), this, SIGNAL(ApplyClicked()));
  QObject::connect(ResetButton, SIGNAL(clicked()), this, SIGNAL(ResetClicked()));
  //QObject::connect(this->m_MethodComboBox, SIGNAL(activated(int)),
  //           this->m_MethodsLayout, SLOT(setCurrentIndex(int)));

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAlgorithmsManagerWidget::AddMethod(QGoAlgorithmWidget* iAlgoWidget)
{
  this->m_MethodWidgets->addWidget(iAlgoWidget);
  //this->m_MethodsLayout->addWidget(iAlgoWidget);
  int Index = this->m_MethodWidgets->indexOf(iAlgoWidget);
  //int Index = this->m_MethodsLayout->indexOf(iAlgoWidget);
  this->m_MethodComboBox->insertItem(Index, iAlgoWidget->GetMethodName().c_str());
  //this->m_MethodComboBox->addItem(iAlgoWidget->GetMethodName().c_str());
  iAlgoWidget->show();
  this->m_MethodComboBox->setCurrentIndex(Index);
  this->m_MethodWidgets->setCurrentWidget(iAlgoWidget);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAlgorithmsManagerWidget::SetCurrentIndex(int iIndex)
{
  this->m_MethodComboBox->setCurrentIndex(iIndex);
  this->m_MethodWidgets->setCurrentIndex(iIndex);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoAlgorithmsManagerWidget::GetModeName()
{
  return this->m_ModeName;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAlgorithmsManagerWidget::SetCurrentChannel(QString iChannel)
{
  this->m_ChannelComboBox->setCurrentIndex(
    this->m_ChannelComboBox->findText(iChannel) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAlgorithmsManagerWidget::SetCurrentTimePoint(QString iTimePoint)
{
  this->m_TimeComboBox->setCurrentIndex(
    this->m_TimeComboBox->findText(iTimePoint) );
}
/*void QGoAlgorithmsManagerWidget::AddMethod(std::string iNameMethod, 
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
}*/
