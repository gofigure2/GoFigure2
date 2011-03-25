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
#include "QGoMeshLevelSetAlgo.h"
#include "QGoFilterChanAndVese.h"


QGoMeshLevelSetAlgo::QGoMeshLevelSetAlgo(QWidget* iParent)
{
  this->SetAlgoWidget(iParent);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoMeshLevelSetAlgo::~QGoMeshLevelSetAlgo()
{
  delete m_Radius;
  delete m_Curvature;
  delete m_Iterations;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshLevelSetAlgo::SetAlgoWidget(QWidget* iParent)
{
  this->m_AlgoWidget = 
    new QGoAlgorithmWidget("LevelSet 3D", iParent);

  m_Radius = new QGoAlgoParameter<double>("Radius", false, 0.1, 99.99, 2, 3);
  this->m_AlgoWidget->AddParameter(m_Radius);

  m_Curvature = new QGoAlgoParameter<int>("Curvature", true, 0, 1000, 20);
  this->m_AlgoWidget->AddParameter(m_Curvature);

  m_Iterations = new QGoAlgoParameter<int> ("Iterations", true, 0, 1000, 100);
  this->m_AlgoWidget->AddParameter(m_Iterations);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkPolyData*> QGoMeshLevelSetAlgo::ApplyAlgo(
  vtkPoints* iSeeds, std::vector<vtkSmartPointer< vtkImageData > >* iImages,
    int iChannel)
{
  QGoFilterChanAndVese LevelSetFilter;

  std::vector<vtkPolyData*> NewMeshes = 
    LevelSetFilter.ApplyFilterLevelSet3D(m_Radius->GetValue(), 
    iSeeds, m_Iterations->GetValue(),
    m_Curvature->GetValue(), iImages, iChannel);
 
  return NewMeshes;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
