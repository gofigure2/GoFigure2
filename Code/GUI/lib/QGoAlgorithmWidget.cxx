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
#include "ctkCollapsibleGroupBox.h"


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

  ctkCollapsibleGroupBox* AdvParamWidget = 
    new ctkCollapsibleGroupBox(tr("Advanced"),this );
  this->m_ExpandableBox->setFlat(true);
  this->m_ExpandableBox->setChecked(false);

  this->m_AdvParamLayout = new QFormLayout;
  AdvParamWidget->setLayout(this->m_AdvParamLayout);

  this->m_VBoxLayout->addLayout(this->m_ParamLayout);
  this->m_VBoxLayout->addWidget(AdvParamWidget); 

  this->setLayout(this->m_VBoxLayout);
  this->m_VBoxLayout->setSizeConstraint(QLayout::SetFixedSize);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoAlgorithmWidget::GetMethodName()
{
  return this->m_MethodName;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAlgorithmWidget::AddParameter(std::string iParamName, 
  int iMinValue, int iMaxValue, int iDefaultValue)
{
  QSpinBox* ParamBox = new QSpinBox(this);
  this->AddParameterInLayout<QSpinBox, int>(ParamBox, iParamName, 
    this->m_ParamLayout, iMinValue, iMaxValue, iDefaultValue);
  //QSpinBox* ParamBox = this->AddSpinBox(iMinValue, iMaxValue, iDefaultValue);
  //QString ParamName(tr("%1:").arg(iParamName.c_str() ) );
  //this->m_ParamLayout->addRow(tr("%1:").arg(iParamName.c_str() ), 
  //  ParamBox);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAlgorithmWidget::AddParameter(std::string iParamName, 
  double iMinValue, double iMaxValue, double iDefaultValue, 
  int iNbDecimal)
{
  //QDoubleSpinBox* ParamBox = this->GetDoubleSpinBox(iMinValue, iMaxValue, 
  //  iDefaultValue, iNbDecimal);
   QDoubleSpinBox* ParamBox = new QDoubleSpinBox(this);
   if (iNbDecimal != 0)
     {
     ParamBox->setDecimals(iNbDecimal);
     }
   this->AddParameterInLayout<QDoubleSpinBox, int>(ParamBox, iParamName, 
    this->m_ParamLayout, iMinValue, iMaxValue, iDefaultValue);
  //this->m_ParamLayout->addRow(tr("%1:").arg(iParamName.c_str() ), 
   // ParamBox);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*QSpinBox* QGoAlgorithmWidget::GetSpinBox(
  int iMinValue, int iMaxValue, int iDefaultValue)
{
  QSpinBox* SpinBox = new QSpinBox(this);
  this->FillGeneralConditions<QSpinBox, int>(SpinBox, 
    iMinValue, iMaxValue, iDefaultValue);
  return SpinBox;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QDoubleSpinBox* QGoAlgorithmWidget::GetDoubleSpinBox(double iMinValue, 
  double iMaxValue, double iDefaultValue, double iNbDecimal)
{
  QDoubleSpinBox* DoubleSpinBox = new QDoubleSpinBox(this);
  this->FillGeneralConditions<QDoubleSpinBox, double>(DoubleSpinBox, 
    iMinValue, iMaxValue, iDefaultValue);
    if (iNbDecimal != 0)
      {
      DoubleSpinBox->setDecimals(iNbDecimal);
      }
  return DoubleSpinBox;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoAlgorithmWidget::AddAdvParameter(std::string iParamName, int iMinValue, 
  int iMaxValue, int iDefaultValue)
{
  

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
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------