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

#ifndef __itkvtkMeshMergeConvexHullFilter_txx
#define __itkvtkMeshMergeConvexHullFilter_txx

#include "itkvtkMeshMergeConvexHullFilter.h"

#include "vtkSmartPointer.h"
#include "vtkAppendPolyData.h"
#include "vtkDelaunay3D.h"
#include "vtkDataSetSurfaceFilter.h"

namespace itk
{
template< class TFeatureImage, class TPolyDataContainer >
vtkMeshMergeConvexHullFilter< TFeatureImage, TPolyDataContainer >::
vtkMeshMergeConvexHullFilter() : Superclass()
{
  this->SetRequiredAttributeComputationFlags();
}

template< class TFeatureImage, class TPolyDataContainer >
void
vtkMeshMergeConvexHullFilter< TFeatureImage, TPolyDataContainer >::
GenerateData()
{
  vtkSmartPointer< vtkAppendPolyData > append =
      vtkSmartPointer< vtkAppendPolyData >::New();

  typename std::list< vtkPolyData* >::iterator it = this->m_Inputs.begin();
  typename std::list< vtkPolyData* >::iterator end = this->m_Inputs.end();

  while( it != end )
    {
    append->AddInput( *it );
    ++it;
    }

  append->Update();

  vtkSmartPointer< vtkDelaunay3D > delaunay =
      vtkSmartPointer< vtkDelaunay3D >::New();
  delaunay->SetInputConnection( append->GetOutputPort() );
  delaunay->Update();

  vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter =
      vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
  surfaceFilter->SetInputConnection(delaunay->GetOutputPort());
  surfaceFilter->Update();

  this->m_Outputs.front()->DeepCopy( surfaceFilter->GetOutput() );
}

template< class TFeatureImage, class TPolyDataContainer >
void
vtkMeshMergeConvexHullFilter< TFeatureImage, TPolyDataContainer >::
SetRequiredAttributeComputationFlags()
{
  this->m_ShapeComputation = true;
  this->m_IntensityComputation = true;
}

}
#endif // __itkvtkMeshMergeConvexHullFilter_h
