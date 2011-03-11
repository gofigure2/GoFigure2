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

#ifndef __QGoAlgorithmWidget_h
#define __QGoAlgorithmWidget_h

#include <QWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <map>
#include "ctkCollapsibleGroupBox.h"
#include "QGoAlgoParameter.h"

/**
 * \class QGoAlgorithmWidget
 * \ingroup GUI 
 * \brief 
*/
class QGoAlgorithmWidget:
  public QWidget
{
  Q_OBJECT
public:
  explicit QGoAlgorithmWidget(std::string iMethodName, QWidget *iParent = 0);
  ~QGoAlgorithmWidget();

  /**
  \brief
  \return the name of the algorithms 
  */
  std::string GetMethodName();

  //void AddGeneralParameter(AlgoParameterStructure iParameter);

  //void AddGeneralParameter(QGoAlgoParameter<int>* iParameter);

  /**
  \brief add the Advanced parameters box if there are parameters inside and reduce it
  before showing the widget    
  */
  void show();

  template<typename T>
  void AddGeneralParameter(QGoAlgoParameter<T>* iParameter)
    {
    if (iParameter->m_AdvParam)
      {
        this->m_AdvParamLayout->addRow(tr("%1:").arg(iParameter->m_ParamName.c_str() ), 
        iParameter->m_Box);
      }    
    else
      {
        this->m_ParamLayout->addRow(tr("%1:").arg(iParameter->m_ParamName.c_str() ), 
        iParameter->m_Box);
      }
    }

protected:
  QVBoxLayout*                             m_VBoxLayout;
  std::string                              m_MethodName;
  QFormLayout*                             m_ParamLayout;
  QFormLayout*                             m_AdvParamLayout;
  bool                                     m_AdvParamAlreadySetUp;

  void Initialize();

  /**
  \brief add the paramater described with the arguments in the parameters area
  of the widget, with a spinbox 
  \param[in] iParamName name of the parameter to be displayed 
  \param[in] iMinValue min value of the spinbox
  \param[in] iMaxValue max value of the spinbox
  \param[in] iDefaultValue default value of the spinbox
  */
  //void AddParameter(std::string iParamName, int iMinValue = 0, int iMaxValue = 0, 
  //  int iDefaultValue = 0);

  /**
  \overload
  */
  //void AddParameter(std::string iParamName, double iMinValue = 0, double iMaxValue = 0, 
  //  double iDefaultValue = 0, int iNbDecimal = 0);

  /**
  \overload
  */
  //void AddParameter(std::string iParamName, QStringList iListValues, std::string iDefaultValue = "");

  /**
  \brief add the paramater described with the arguments in an expandable box 
  with a spinbox 
  \param[in] iParamName name of the parameter to be displayed 
  \param[in] iMinValue min value of the spinbox
  \param[in] iMaxValue max value of the spinbox
  \param[in] iDefaultValue default value of the spinbox
  */
  //void AddAdvParameter(std::string iParamName, int iMinValue = 0, int iMaxValue = 0, 
  //  int iDefaultValue = 0);

  /**
  \overload
  */
  //void AddAdvParameter(std::string iParamName, double iMinValue = 0, double iMaxValue = 0, 
  //  double iDefaultValue = 0, int iNbDecimal = 0);

  /**
  \overload
  */
  //void AddAdvParameter(std::string iAdvParamName, QStringList iListValues, std::string iDefaultValue = "");

  /**
  \brief set the min, max and default values of the spinbox and 
  add it with the parameter name in the layout
  \param[in] iBox spinbox to be set up
  \param[in] iParamName name of the parameter
  \param[in] iMinValue min value of the spinbox
  \param[in] iMaxValue max value of the spinbox
  \param[in] iDefaultValue default value of the spinbox
  \tparam T  int or double
  \tparam B  SpinBox or DoubleSpinBox
  */
  /*template<typename B, typename T>
  void AddParameterInLayout(B* iBox, std::string iParamName,
    QFormLayout* iLayout, T iMinValue = 0, T iMaxValue = 0, 
    T iDefaultValue = 0)
  {
    if (!(iMinValue == 0 && iMaxValue ==0) )
      {
      iBox->setRange(iMinValue, iMaxValue);
      }
    if(iDefaultValue != 0)
      {
      iBox->setValue(iDefaultValue);
      }
    int NbRow = iLayout->rowCount();
    iLayout->addRow(tr("%1:").arg(iParamName.c_str() ), 
    iBox);

  }*/

  /**
  \brief add a combobox in iLayout with items corresponding to iListValues
  \param[in] iParamName name of the param to be displayed next to the combobox
  \param[in] iListValues list of the values to be displayed in the combobox
  \param[in] iLayout which layout to insert the combobox: Param or advParam
  \param[in] iDefaultValue default selected value in the combobox
  */
  //void AddParamComboBoxinLayout(std::string iParamName, 
  //QStringList iListValues, QFormLayout* iLayout, std::string iDefaultValue);

};
#endif
