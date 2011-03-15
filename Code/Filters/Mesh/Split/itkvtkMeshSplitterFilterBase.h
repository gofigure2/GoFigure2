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

#include "itkvtkMeshFilterBase.h"
#include "itkPointSet.h"
#include "vtkPolyData.h"

#include <vector>

namespace itk
{
/**
  \class vtkMeshSplitterFilterBase
  \brief
*/
template< class TFeatureImage >
class vtkMeshSplitterFilterBase : public vtkMeshFilterBase< TFeatureImage >
  {
public:
  typedef vtkMeshFilterBase< TFeatureImage > Superclass;
  typedef vtkMeshSplitterFilterBase Self;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( vtkMeshSplitterFilterBase, vtkMeshFilterBase );

  typedef typename Superclass::FeatureImageType FeatureImageType;
  typedef typename Superclass::FeatureImagePointer FeatureImagePointer;

  void SetMesh( vtkPolyData* iMesh );

  std::vector< vtkPolyData* > GetOutputs();

protected:
  /** \brief Constructor */
  vtkMeshSplitterFilterBase();

  /** \brief Destructor */
  ~vtkMeshSplitterFilterBase() {}

  vtkPolyData* m_Mesh;

  std::vector< vtkPolyData* > m_Outputs;

  double m_Bounds[6];


  template< class TPoint >
  bool IsPointInMeshBounds( const TPoint& iP ) const
    {
    for( unsigned int i = 0; i < 3; ++i )
      {
      double t = static_cast< double >( iP[i] );

      if( ( t < m_Bounds[2*i] ) || ( t > m_Bounds[2*i+1] ) )
        {
        return false;
        }
      }
    return true;
    }


  /** \brief Main method to be reimplemented in inherited classes */
  virtual void Split() = 0;

  virtual void GenerateData();

private:
  vtkMeshSplitterFilterBase( const Self& );
  void operator = ( const Self& );
  };
}

#include "itkvtkMeshSplitterFilterBase.txx"
#endif // __itkvtkMeshSplitterFilterBase_h
