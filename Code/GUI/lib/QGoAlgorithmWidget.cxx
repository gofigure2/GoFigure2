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

#include "QGoAlgorithmWidget.h"
#include <QLabel>
#include <QComboBox>
#include "ctkCollapsibleGroupBox.h"
#include "ConvertToStringHelper.h"


QGoAlgorithmWidget::QGoAlgorithmWidget(std::string iMethodName, QWidget *iParent )
  :QWidget(iParent)
{
  this->m_MethodName = iMethodName;
  this->Initialize();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoAlgorithmWidget::~QGoAlgorithmWidget()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAlgorithmWidget::Initialize()
{
  this->m_VBoxLayout = new QVBoxLayout;
  this->m_ParamLayout = new QFormLayout;
  this->m_AdvParamLayout = new QFormLayout;
  
  this->m_VBoxLayout->addLayout(this->m_ParamLayout);
 
  this->setLayout(this->m_VBoxLayout);
  this->m_VBoxLayout->setSizeConstraint(QLayout::SetFixedSize);
  this->m_AdvParamAlreadySetUp = false;
  //can not setflat + set checked before inserting the parameters
  //or no ones will appear...
  //this->m_AdvParamGroupBox->setFlat(true);
  //this->m_AdvParamGroupBox->setChecked(false);

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoAlgorithmWidget::GetMethodName()
{
  return this->m_MethodName;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAlgorithmWidget::show()
{
  if (!this->m_AdvParamAlreadySetUp) //in order not to add another collapsible box
    //each time the widget is shown
    {
    if (this->m_AdvParamLayout->rowCount()>0 ) //if there is at least one advanced parameter,
      //if not, no need to add the collapsible box
      {
      ctkCollapsibleGroupBox* AdvParamGroupBox =
        new ctkCollapsibleGroupBox(tr("Advanced"));
      AdvParamGroupBox->setLayout(this->m_AdvParamLayout);
      AdvParamGroupBox->setFlat(true);
      AdvParamGroupBox->setChecked(false);
      this->m_VBoxLayout->addWidget(AdvParamGroupBox);
      }
    this->m_AdvParamAlreadySetUp = true; //no need to check again when widget shown another time
    }

  QWidget::show();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
