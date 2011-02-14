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

#ifndef __itkvtkMeshSplitterFilterBase_h
#define __itkvtkMeshSplitterFilterBase_h

#include "itkLightObject.h"
#include "itkPointSet.h"
#include "vtkPolyData.h"

#include <vector>

namespace itk
{
/**
  \class vtkMeshSplitterFilterBase
  \brief
*/
class vtkMeshSplitterFilterBase : public LightObject
  {
public:
  typedef LightObject Superclass;
  typedef vtkMeshSplitterFilterBase Self;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  typedef PointSet< double, 3 > PointSetType;
  typedef PointSetType::Pointer PointSetPointer;
  typedef PointSetType::PointsContainerPointer PointsContainerPointer;
  typedef PointSetType::PointsContainerConstIterator PointsContainerConstIterator;
  typedef typename PointSetType::PointType PointType;

  void SetMesh( vtkPolyData* iMesh )
    {
    if( ( iMesh ) && ( iMesh != m_Mesh ) )
      {
      m_Mesh = iMesh;
      m_Mesh->GetBounds( m_Bounds );
      }
    }

  void SetSeeds( PointSetType* iSeeds )
    {
    m_Seeds = iSeeds;
    }

  void Update()
    {
    GenerateData();
    }

  std::vector< vtkPolyData* > GetOutputs()
    {
    return m_Outputs;
    }

protected:
  /** \brief Constructor */
  vtkMeshSplitterFilterBase() : m_Mesh( NULL ), m_Outputs( 0 ), m_Seeds( NULL )
    {
    for( unsigned int dim = 0; dim < 3; ++dim )
      {
      m_Bounds[2 * dim] = vcl_numeric_limits< double >::max();
      m_Bounds[2 * dim + 1] = vcl_numeric_limits< double >::min();
      }
    }
  /** \brief Destructor */
  ~vtkMeshSplitterFilterBase() {}

  vtkPolyData* m_Mesh;

  std::vector< vtkPolyData* > m_Outputs;

  PointSetPointer m_Seeds;

  double m_Bounds[6];


  bool IsPointInMeshBounds( const PointType& iP ) const
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

  bool CheckAllSeeds() const
    {
    PointsContainerPointer points = m_Seeds->GetPoints();

    PointsContainerConstIterator it = points->Begin();
    PointsContainerConstIterator end = points->End();

    while( it != end )
      {
      if( IsPointInMeshBounds( it->Value() ) )
        {
        std::cout << it->Value() << " is out of bounds" << std::endl;
        return false;
        }
      ++it;
      }
    return true;
    }

  /** \brief Main method to be reimplemented in inherited classes */
  virtual void Split() = 0;

  virtual void GenerateData()
    {
    if( CheckAllSeeds() )
      {
      this->Split();
      }
    else
      {
      std::cout <<"Out of bounds" <<std::endl;
      return;
      }
    }

private:
  vtkMeshSplitterFilterBase( const Self& );
  void operator = ( const Self& );
  };
}

#endif // __itkvtkMeshSplitterFilterBase_h
