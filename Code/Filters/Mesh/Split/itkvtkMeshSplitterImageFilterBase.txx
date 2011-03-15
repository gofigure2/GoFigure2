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
#ifndef __itkvtkMeshSplitterImageFilterBase_txx
#define __itkvtkMeshSplitterImageFilterBase_txx

#include "itkvtkMeshSplitterImageFilterBase.h"

#include "itkConvertMeshesToLabelImageFilter.h"
#include "itkvtkPolyDataToitkQuadEdgeMesh.h"

namespace itk
{
template< class TImage >
vtkMeshSplitterImageFilterBase< TImage >::
vtkMeshSplitterImageFilterBase() : Superclass( ),
    m_NumberOfThreads( 1 ), m_NumberOfTrianglesPerMesh( 200 ),
    m_NumberOfSmoothingIterations( 8 ), m_SmoothingRelaxationFactor( 0.75 ),
    m_DelaunayConforming( false ), m_UseSmoothing( true ),
    m_UseDecimation( true )
{}

template< class TImage >
void
vtkMeshSplitterImageFilterBase< TImage >::
SetNumberOfImages( const size_t& iN )
{
  m_Images.resize( iN );
}

template< class TImage >
void
vtkMeshSplitterImageFilterBase< TImage >::
SetImage( const size_t& iId, ImageType* iImage )
{
  if( iId < m_Images.size() )
    {
    m_Images[iId] = iImage;
    this->Modified();
    }
}

template< class TImage >
void
vtkMeshSplitterImageFilterBase< TImage >::
ComputeBinaryImageFromInputMesh()
{
  typedef ConvertMeshesToLabelImageFilter< ImageType > MeshToLabelFilterType;
  typedef typename MeshToLabelFilterType::Pointer MeshToLabelFilterPointer;
  typedef typename MeshToLabelFilterType::MeshType MeshType;

  typedef vtkPolyDataToitkQuadEdgeMesh< MeshType > MeshConverterType;
  typedef typename MeshConverterType::Pointer MeshConverterPointer;

  MeshConverterPointer converter = MeshConverterType::New();
  converter->SetInput( this->m_Mesh );
  converter->Update();

  typename MeshToLabelFilterType::MeshVectorType mesh_vector( 1 );
  mesh_vector[0] = converter->GetOutput();

  m_BinaryImage = ImageType::New();
  m_BinaryImage->CopyInformation( m_Images.front() );
  m_BinaryImage->SetRegions( m_Images.front()->GetLargestPossibleRegion() );
  m_BinaryImage->Allocate();
  m_BinaryImage->FillBuffer( 0 );

  typename MeshToLabelFilterType::Pointer binarizer = MeshToLabelFilterType::New();
  binarizer->SetInput( m_BinaryImage );
  binarizer->SetMeshes( mesh_vector );
  binarizer->Update();
}


template< class TImage >
void
vtkMeshSplitterImageFilterBase< TImage >::
Split()
{
  ComputeBinaryImageFromInputMesh();

  this->SplitBinaryImage();
  GenerateMeshesFromOutputImage();
}

template< class TImage >
void
vtkMeshSplitterImageFilterBase< TImage >::
GenerateMeshesFromOutputImage()
{
  ExtracMeshFilterPointer extractor = ExtracMeshFilterType::New();
  extractor->SetInput( m_OutputImage );

  size_t i = 0;
  size_t NumberOfImages = m_Images.size();

  extractor->SetNumberOfFeatureImages( NumberOfImages );

  for( ; i < NumberOfImages; ++i )
    {
    extractor->SetFeatureImage( i, m_Images[i] );
    }

  extractor->SetNumberOfThreads( m_NumberOfThreads );
  extractor->SetUseSmoothing( m_UseSmoothing );
  extractor->SetUseDecimation( m_UseDecimation );
  extractor->SetNumberOfTrianglesPerMesh( m_NumberOfTrianglesPerMesh );
  extractor->SetNumberOfSmoothingIterations( m_NumberOfSmoothingIterations );
  extractor->SetSmoothingRelaxationFactor( m_SmoothingRelaxationFactor );
  extractor->SetDelaunayConforming( m_DelaunayConforming );
  extractor->Update();

  typedef typename ExtracMeshFilterType::MeshVectorType MeshVectorType;
  MeshVectorType MeshVector = extractor->GetOutputs();

  typename MeshVectorType::const_iterator it = MeshVector.begin();
  typename MeshVectorType::const_iterator end = MeshVector.end();

  m_Outputs.resize( MeshVector.size() );

  while( it != end )
    {
    ITKVTKMeshConverterPointer converter = ITKVTKMeshConverterType::New();
    converter->SetInput( *it );
    converter->Update();

    m_Outputs[i] = vtkPolyData::New();
    m_Outputs[i]->DeepCopy( converter->GetOutput() );
    ++it;
    ++i;
    }
}

}

#endif // __itkvtkMeshSplitterImageFilterBase_txx
