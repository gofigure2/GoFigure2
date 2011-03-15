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

#ifndef __itkvtkMeshFilterBase_h
#define __itkvtkMeshFilterBase_h

#include "itkObject.h"

namespace itk
{
template< class TFeatureImage >
class vtkMeshFilterBase : public Object
{
public:
  typedef Object Superclass;
  typedef vtkMeshFilterBase Self;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  typedef TFeatureImage FeatureImageType;
  typedef typename FeatureImageType::Pointer FeatureImagePointer;
  typedef typename FeatureImageType::PixelType FeatureImagePixelType;
  typedef typename FeatureImageType::IndexType FeatureImageIndexType;
  typedef typename FeatureImageType::PointType FeatureImagePointType;

  void SetNumberOfImages( const size_t& iN )
    {
    m_Images.resize( iN );
    }

  void SetFeatureImage( const size_t& iId, FeatureImageType* iImage )
    {
    if( iId < m_Images.size() )
      {
      m_Images[iId] = iImage;
      this->Modified();
      }
    }

  void Update()
    {
    this->GenerateData();
    }

protected:
  vtkMeshFilterBase() : Superclass() {}
  virtual ~vtkMeshFilterBase() {}

  virtual void GenerateData() = 0;

  std::vector< FeatureImagePointer > m_Images;

private:
  vtkMeshFilterBase( const Self& );
  void operator = ( const Self& );
};
}
#endif

