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
#include "QGoWaterShedAlgo.h"


// need param: max threshold
QGoWaterShedAlgo::QGoWaterShedAlgo(std::vector< vtkPoints* >* iSeeds,
                                   int iMaxThreshold,
                                   QWidget* iParent)
  :QGoSemiAutoSegmentationAlgo(iSeeds,iParent)
{
  m_MaxThreshold = iMaxThreshold;
  this->SetAlgoWidget(iParent);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoWaterShedAlgo::~QGoWaterShedAlgo()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoWaterShedAlgo::DeleteParameters()
{
  delete m_ThresMin;
  delete m_ThresMax;
  delete m_CorrThres;
  delete m_Alpha;
  delete m_Beta;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoWaterShedAlgo::SetAlgoWidget(QWidget* iParent)
{
  this->m_AlgoWidget =
    new QGoAlgorithmWidget("WaterShed", iParent);

  m_ThresMin = new QGoAlgoParameter<int>("Background <",true, 0, m_MaxThreshold, 10);
  this->m_AlgoWidget->AddParameter(m_ThresMin);

  m_ThresMax = new QGoAlgoParameter<int>("Nuclei >", true, 0, m_MaxThreshold, 30);
  this->m_AlgoWidget->AddParameter(m_ThresMax);

  m_CorrThres = new QGoAlgoParameter<double>("Gaussian Simile", true, 0, 1, 2, 0.5, 0.01);
  this->m_AlgoWidget->AddParameter(m_CorrThres);

  m_Alpha = new QGoAlgoParameter<double>("Scaling", true, 0, 10, 2, 1.5, 0.1);
  this->m_AlgoWidget->AddParameter(m_Alpha);

  m_Beta = new QGoAlgoParameter<double>("Slope", true, 0, 10, 2, 3, 0.1);
  this->m_AlgoWidget->AddParameter(m_Beta);

  QGoSemiAutoSegmentationAlgo::SetAlgoWidget();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

