/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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

#ifndef QGOMESHSPLITFILTERBASE_H
#define QGOMESHSPLITFILTERBASE_H

namespace itk
{
class vtkMeshSplitterFilterBase : public LightObject
  {
public:
  typedef LightObject Superclass;
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

  std::list< vtkPolyData* > GetOutputs()
    {
    return m_Outputs;
    }

protected:
  vtkMeshSplitterFilterBase() : m_Mesh( NULL ) {}
  ~vtkMeshSplitterFilterBase() {}

  vtkPolyData* m_Mesh;
  std::list< vtkPolyData* > m_Outputs;

  double m_Bounds[6];
  PointSetPointer m_Seeds;

  void IsPointInMeshBounds( const PointType& iP ) const
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

  bool CheckAllSeeds()
    {
    PointsContainerPointer points = m_Seeds->GetPoints();

    PointsContainerConstIterator it = points->Begin();

    while( it != points->End() )
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

  virtual void Split() = 0;

  virtual void GenerateData()
    {
    if( !CheckAllSeeds() )
      {
      std::cout <<"Out of bounds" <<std::endl;
      return;
      }
    else
      {
      this->Split();
      }
    }

private:
  vtkMeshSplitterFilterBase( const Self& );
  void operator = ( const Self& );
  };
}

#endif // QGOMESHSPLITFILTERBASE_H
