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

#ifndef __itkvtkMeshMergeFilterBase_h
#define __itkvtkMeshMergeFilterBase_h

#include "itkvtkMeshFilterBase.h"
#include <list>

class vtkPolyData;

namespace itk
{
template< class TFeatureImage >
class vtkMeshMergeFilterBase :
    public vtkMeshFilterBase< TFeatureImage >
{
public:
  typedef Object Superclass;
  typedef vtkMeshMergeFilterBase Self;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  typedef TFeatureImage FeatureImageType;
  typedef typename FeatureImageType FeatureImagePointer;

  void SetInputs( std::list< vtkPolyData* > iMeshes )
    {
    m_Inputs = iMeshes;
    this->Modified();
    }

  vtkPolyData* GetOutput()
    {
    return m_Output;
    }

protected:
  vtkMeshMergeFilterBase() : Superclass(), m_Output( NULL ) {}
  virtual ~vtkMeshMergeFilterBase() {}

  std::list< vtkPolyData* > m_Inputs;
  vtkPolyData* m_Output;

  virtual void GenerateData();

private:
  vtkMeshMergeFilterBase( const Self& );
  void operator = ( const Self& );
};
}
#endif // __itkvtkMeshMergeFilterBase_h
