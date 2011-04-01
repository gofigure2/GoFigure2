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
#include "QGoMeshShapeAlgo.h"
#include "QGoFilterShape.h"


QGoMeshShapeAlgo::QGoMeshShapeAlgo(vtkPoints* iSeeds, QWidget* iParent)
  :QGoShapeAlgo(iSeeds, iParent)
{
  this->SetAlgoWidget(iParent);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoMeshShapeAlgo::~QGoMeshShapeAlgo()
{
  this->DeleteParameters();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkPolyData*> QGoMeshShapeAlgo::ApplyAlgo(
  std::vector<vtkSmartPointer< vtkImageData > >* iImages,
    int iChannel)
{
  QGoFilterShape ShapeFilter;

  std::vector<vtkPolyData*> NewMeshes = 
    ShapeFilter.ApplyFilter3D(m_Radius->GetValue(), 
    this->m_Seeds, this->m_Shape->Getvalue(), iImages, iChannel);

  return NewMeshes;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
