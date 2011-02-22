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

#include "QGoAdvancedParametersWidget.h"
#include <QStackedLayout>
#include <QLabel>
#include <QGroupBox>


QGoAdvancedParametersWidget::QGoAdvancedParametersWidget(QWidget *iParent )
  :QWidget(iParent)
{
  //this->Initialize();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoAdvancedParametersWidget::~QGoAdvancedParametersWidget()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAdvancedParametersWidget::setLayout(QLayout* iLayout)
{
  //this->m_VBoxLayout = new QVBoxLayout;
  //this->m_ComboBox = new QComboBox(this);
  //this->m_VBoxLayout->addWidget(this->m_ComboBox);
  
  m_ExpandableBox = new ctkCollapsibleGroupBox(tr("Advanced") );
  //this->m_AdvParamStackedLayout = new QStackedLayout(this);
  this->m_ExpandableBox->setFlat(true);
  this->m_ExpandableBox->setChecked(false);
//  this->m_ExpandableBox->setLayout(this->m_AdvParamStackedLayout);
  
  this->m_VBoxLayout->addWidget(this->m_ExpandableBox);
  this->m_VBoxLayout->setSizeConstraint(QLayout::SetFixedSize);
  this->setLayout(this->m_VBoxLayout);

  //QObject::connect(this->m_ComboBox, SIGNAL(activated(int)),
       //      this->m_AdvParamStackedLayout, SLOT(setCurrentIndex(int)));

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAdvancedParametersWidget::AddAdvancedParamWidget(QWidget* iWidget)
{
  this->m_ExpandableBox->setLayout(iWidget->layout());
  this->m_ExpandableBox->setChecked(false);
  //this->m_AdvParamStackedLayout->addWidget(iWidget);
  //int Index = this->m_AdvParamStackedLayout->indexOf(iWidget);
  //this->m_AdvParamStackedLayout
  //this->m_ComboBox->insertItem(Index,iName.c_str());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
