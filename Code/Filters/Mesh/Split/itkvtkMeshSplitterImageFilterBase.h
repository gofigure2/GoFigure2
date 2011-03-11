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

#ifndef __itkvtkMeshSplitterImageFilterBase_h
#define __itkvtkMeshSplitterImageFilterBase_h

#include "itkLightObject.h"
#include "itkPointSet.h"
#include "vtkPolyData.h"
#include "itkvtkMeshSplitterFilterBase.h"
#include "itkvtkPolyDataToBinaryMaskImageFilter.h"
#include "itkExtractMeshesFromLabelImageFilter.h"
#include "itkQuadEdgeMeshTovtkPolyData.h"
#include "itkImage.h"
#include <list>

namespace itk
{
/**
  \class vtkMeshSplitterImageFilterBase
  \brief
*/
template< class TImage >
class vtkMeshSplitterImageFilterBase : public vtkMeshSplitterFilterBase
  {
public:
  typedef vtkMeshSplitterImageFilterBase Self;
  typedef vtkMeshSplitterFilterBase Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( vtkMeshSplitterFilterBase, vtkMeshSplitterFilterBase );

  typedef typename Superclass::PointSetType PointSetType;
  typedef typename PointSetType::Pointer PointSetPointer;
  typedef typename PointSetType::PointsContainerPointer PointsContainerPointer;
  typedef typename PointSetType::PointsContainerConstIterator
    PointsContainerConstIterator;
  typedef typename PointSetType::PointType PointType;

  typedef TImage ImageType;
  typedef typename ImageType::Pointer ImagePointer;
  typedef typename ImageType::PixelType ImagePixelType;
  typedef typename ImageType::IndexType ImageIndexType;
  typedef typename ImageType::PointType ImagePointType;

  typedef Image< bool, 3 > BinaryMaskImageType;
  typedef typename BinaryMaskImageType::Pointer BinaryMaskImagePointer;

  typedef vtkPolyDataToBinaryMaskImageFilter< ImageType, BinaryMaskImageType >
    BinarizerType;
  typedef typename BinarizerType::Pointer BinarizerPointer;

  typedef ExtractMeshesFromLabelImageFilter< ImageType > ExtracMeshFilterType;
  typedef typename ExtracMeshFilterType::Pointer ExtracMeshFilterPointer;
  typedef typename ExtracMeshFilterType::MeshType MeshType;

  typedef QuadEdgeMeshTovtkPolyData< MeshType > ITKVTKMeshConverterType;
  typedef typename ITKVTKMeshConverterType::Pointer ITKVTKMeshConverterPointer;

  virtual void SetImage( ImageType* iImage );

protected:
  vtkMeshSplitterImageFilterBase();

  ~vtkMeshSplitterImageFilterBase() {}

  BinaryMaskImagePointer m_BinaryImage;
  ImagePointer m_Image;
  ImagePointer m_OutputImage;
  unsigned int m_NumberOfThreads;
  unsigned int m_NumberOfTrianglesPerMesh;
  unsigned int m_NumberOfSmoothingIterations;
  unsigned int m_SmoothingRelaxationFactor;

  bool m_DelaunayConforming;
  bool m_UseSmoothing;
  bool m_UseDecimation;

  virtual void ComputBinaryImageFromInputMesh();


  virtual void Split();

  virtual void SplitBinaryImage() = 0;

  void GenerateMeshesFromOutputImage();

private:
  vtkMeshSplitterImageFilterBase( const Self& );
  void operator = ( const Self& );
  };
}
#include "itkvtkMeshSplitterImageFilterBase.txx"
#endif // __itkvtkMeshSplitterImageFilterBase_h
