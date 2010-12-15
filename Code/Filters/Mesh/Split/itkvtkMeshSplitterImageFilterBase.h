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

#ifndef ITKVTKMESHSPLITTERIMAGEFILTERBASE_H
#define ITKVTKMESHSPLITTERIMAGEFILTERBASE_H

#include "itkLightObject.h"
#include "itkPointSet.h"
#include "vtkPolyData.h"

#include <list>

namespace itk
{
template< class TImage >
class vtkMeshSplitterFilterBase : public vtkMeshSplitterFilterBase
  {
public:
  typedef vtkMeshSplitterFilterBase Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  typedef Superclass::PointSetType PointSetType;
  typedef PointSetType::Pointer PointSetPointer;
  typedef PointSetType::PointsContainerPointer PointsContainerPointer;
  typedef PointSetType::PointsContainerConstIterator PointsContainerConstIterator;
  typedef typename PointSetType::PointType PointType;

  typedef TImage ImageType;
  typedef typename ImageType::Pointer ImagePointer;

  typedef Image< bool, 3 > BinaryMaskImageType;


  virtual void SetImage( ImageType* iImage )
    {
    m_Image = iImage;
    }

protected:
  vtkMeshSplitterFilterBase() : m_Mesh( NULL ) {}
  ~vtkMeshSplitterFilterBase() {}

  BinaryMaskImagePointer m_BinaryImage;

  virtual void ComputBinaryImageFromInputMesh()
    {
    typedef vtkPolyDataToBinaryMaskImageFilter< ImageType, BinaryMaskImageType >
      BinarizerType;
    BinarizerType::Pointer binarizer = BinarizerType::New();
    binarizer->SetInput( m_Image );
    binarizer->SetPolyData( this->m_Mesh );

    try
      {
      binarizer->Update();
      m_BinaryImage = binarizer->GetOutput();
      m_BinaryImage->DisconnectPipeline();
      }
    catch( itk::ExceptionObject e )
      {
      std::cerr << "Error: " << e << std::endl;
      return;
      }
    }


  virtual void Split()
    {
    ComputBinaryImageFromInputMesh();

    if( m_BinaryImage.IsNotNull() )
      {
      SplitBinaryImage();
      }

    }

  virtual void SplitBinaryImage() = 0;

private:
  vtkMeshSplitterFilterBase( const Self& );
  void operator = ( const Self& );
  };
}

#endif // ITKVTKMESHSPLITTERIMAGEFILTERBASE_H
