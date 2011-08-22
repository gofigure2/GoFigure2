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
#ifndef __QGoAlgoParameter_h
#define __QGoAlgoParameter_h

#include <string>
#include <stdlib.h>
#include <QStringList>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>

/**
\brief specialized class for a parameter to be added in an
algorithm widget
*/
template< class T >
class QGoAlgoParameter
{
  public:
    QGoAlgoParameter()
      {};
    ~QGoAlgoParameter(){};

  protected:
    T m_Value;
    T m_DefaultValue;
    std::string m_ParamName;
};
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template<>
class QGoAlgoParameter< int >
{
  public:
    typedef QSpinBox RepresentationType;
    QGoAlgoParameter<int>(std::string iParamName,
      bool iAdvParam, int iMin, int iMax,
      int iDefaultValue = 0, int iDefaultStep= 1)
    {
    m_ParamName = iParamName;
    m_Box = new RepresentationType;
    m_AdvParam = iAdvParam;
    SetRangeValues(iMin, iMax, iDefaultValue, iDefaultStep);
    };

    ~QGoAlgoParameter<int>()
    {
      /*if (m_Box != NULL)
      {
      delete m_Box;
      }*/
    };

    std::string m_ParamName;
    bool m_AdvParam;
    RepresentationType* m_Box;

    int GetValue()
    {
    return m_Box->value();
    };

  protected:

    void SetRangeValues(int iMin, int iMax, int iDefaultValue, int iDefaultStep)
    {
      m_Box->setRange(iMin, iMax);
      m_Box->setSingleStep(iDefaultStep);
      if (iDefaultValue != 0)
        {
        m_Box->setValue(iDefaultValue);
        }
    };

};
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template<>
class QGoAlgoParameter< double >
{
  public:
    typedef QDoubleSpinBox RepresentationType;
    QGoAlgoParameter<double>(std::string iParamName,
      bool iAdvParam, double iMin, double iMax, 
      int iNbDecimal, double iDefaultValue = 0, double iDefaultStep = 0.1)
      {
      m_ParamName = iParamName;
      m_Box = new RepresentationType;
      m_AdvParam = iAdvParam;
      SetRangeValues(iMin, iMax, iNbDecimal, iDefaultValue, iDefaultStep);
      };

    ~QGoAlgoParameter<double>()
    {
      /*if (m_Box != NULL)
      {
      delete m_Box;
      }*/
    };

    std::string m_ParamName;
    bool m_AdvParam;
    RepresentationType* m_Box; 

    double GetValue()
    {
    return m_Box->value();
    };
    
  protected:

    void SetRangeValues(double iMin, double iMax, 
      int iNbDecimal, double iDefaultValue, double iDefaultStep)
    {
      m_Box->setRange(iMin, iMax);
      m_Box->setDecimals(iNbDecimal);
      m_Box->setSingleStep(iDefaultStep);
      if (iDefaultValue != 0)
        {
        m_Box->setValue(iDefaultValue);
        }
    };
};
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template<>
class QGoAlgoParameter< std::string >
{
  public:
    typedef QComboBox RepresentationType;
    QGoAlgoParameter<std::string>(std::string iParamName,
      bool iAdvParam,
      QStringList iListValues, std::string iDefaultValue = "")
      {
      m_ParamName = iParamName;
      m_Box = new RepresentationType;
      m_AdvParam = iAdvParam;
      SetListValues(iListValues, iDefaultValue);
      }

    ~QGoAlgoParameter<std::string>()
    {
      /*if (m_Box != NULL)
      {
      delete m_Box;
      }*/
    };

    std::string m_ParamName;
    bool m_AdvParam;
    RepresentationType* m_Box;

    std::string Getvalue()
      {
      return m_Box->currentText().toStdString();
      }

  protected:

    void SetListValues(QStringList iListValues, std::string iDefaultValue)
      {
      m_Box->addItems(iListValues);
      if (!iDefaultValue.empty())
        {
        m_Box->setCurrentIndex(m_Box->findText(iDefaultValue.c_str()));
        }
      };
};


#endif
