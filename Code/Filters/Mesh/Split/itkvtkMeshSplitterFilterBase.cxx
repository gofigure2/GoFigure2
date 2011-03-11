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

#include "itkvtkMeshSplitterFilterBase.h"

namespace itk
{
vtkMeshSplitterFilterBase::
vtkMeshSplitterFilterBase() : m_Mesh( NULL ), m_Outputs( 0 ), m_Seeds( NULL )
  {
  for( unsigned int dim = 0; dim < 3; ++dim )
    {
    m_Bounds[2 * dim] = NumericTraits< double >::max();
    m_Bounds[2 * dim + 1] = NumericTraits< double >::min();
    }
  }

void
vtkMeshSplitterFilterBase::
SetMesh( vtkPolyData* iMesh )
{
  if( ( iMesh ) && ( iMesh != m_Mesh ) )
    {
    m_Mesh = iMesh;
    m_Mesh->GetBounds( m_Bounds );
    this->Modified();
    }
}

void
vtkMeshSplitterFilterBase::
SetSeeds( PointSetType* iSeeds )
{
  if( iSeeds )
    {
    m_Seeds = iSeeds;
    this->Modified();
    }
}

void
vtkMeshSplitterFilterBase::
Update()
{
  GenerateData();
}

std::vector< vtkPolyData* >
vtkMeshSplitterFilterBase::
GetOutputs()
{
  return m_Outputs;
}

bool
vtkMeshSplitterFilterBase::
IsPointInMeshBounds( const PointType& iP ) const
{
  for( unsigned int i = 0; i < 3; ++i )
    {
    if( ( iP[i] < m_Bounds[2*i] ) || ( iP[i] > m_Bounds[2*i+1] ) )
      {
      return false;
      }
    }
  return true;
}

bool
vtkMeshSplitterFilterBase::
CheckAllSeeds() const
{
  PointsContainerPointer points = m_Seeds->GetPoints();

  PointsContainerConstIterator it = points->Begin();
  PointsContainerConstIterator end = points->End();

  while( it != end )
    {
    if( !IsPointInMeshBounds( it->Value() ) )
      {
      std::cout << it->Value() << " is out of bounds" << std::endl;
      return false;
      }
    ++it;
    }
  return true;
  }

void
vtkMeshSplitterFilterBase::
GenerateData()
{
  if( !m_Mesh )
    {
    itkGenericExceptionMacro( << "m_Mesh is NULL" );
    }

  if( m_Seeds.IsNull() )
    {
    itkGenericExceptionMacro( << "m_Seeds is NULL" );
    }

  if( !CheckAllSeeds() )
    {
    itkGenericExceptionMacro( <<"Out of bounds" );
    }

  this->Split();
  }
}
