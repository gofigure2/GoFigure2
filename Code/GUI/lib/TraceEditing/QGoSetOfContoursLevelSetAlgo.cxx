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
#include "QGoSetOfContoursLevelSetAlgo.h"
#include "QGoFilterChanAndVese.h"

#include "GoImageProcessor.h"

QGoSetOfContoursLevelSetAlgo::QGoSetOfContoursLevelSetAlgo(
  std::vector< vtkPoints* >* iSeeds, QWidget* iParent)
  :QGoLevelSetAlgo(iSeeds, iParent)
{
  m_Sampling = new QGoAlgoParameter<int>("Sampling", false, 0, 999, 3);
  this->m_AlgoWidget->AddParameter(m_Sampling);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoSetOfContoursLevelSetAlgo::~QGoSetOfContoursLevelSetAlgo()
{
  //this->DeleteParameters();
  delete m_Sampling;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkPolyData*> QGoSetOfContoursLevelSetAlgo::ApplyAlgo(
  GoImageProcessor* iImages,
    int iChannel, bool iIsInvertedOn)
{
  std::vector<vtkPolyData*> NewContours = std::vector<vtkPolyData*>();
  return NewContours;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<std::vector<vtkPolyData*> > QGoSetOfContoursLevelSetAlgo::
  ApplyAlgoSeveralSeeds(
    GoImageProcessor* iImages, int iChannel)
{
  std::vector<std::vector<vtkPolyData*> > NewContours;
  QGoFilterChanAndVese LevelSetFilter;
  //double *center = new double[3];

    NewContours =
      LevelSetFilter.ApplyFilterSetOf2D(this->m_Radius->GetValue(),
      this->m_Seeds, this->m_Iterations->GetValue(),
      this->m_Curvature->GetValue(),this->m_Sampling->GetValue(),
      iImages, iChannel );

   return NewContours;
}
