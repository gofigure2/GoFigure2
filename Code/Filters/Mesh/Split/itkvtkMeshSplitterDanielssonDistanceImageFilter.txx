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

#ifndef __itkvtkMeshSplitterDanielssonDistanceImageFilter_txx
#define __itkvtkMeshSplitterDanielssonDistanceImageFilter_txx

#include "itkvtkMeshSplitterDanielssonDistanceImageFilter.h"
#include "itkAndImageFilter.h"

namespace itk
{
template< class TImage >
vtkMeshSplitterDanielssonDistanceImageFilter< TImage >::
vtkMeshSplitterDanielssonDistanceImageFilter() : Superclass(),
  m_SeedImage( NULL ) {}

template< class TImage >
void
vtkMeshSplitterDanielssonDistanceImageFilter< TImage >::
SplitBinaryImage()
{
  ImagePixelType zero = NumericTraits< ImagePixelType >::Zero;

  if( m_SeedImage.IsNull() )
    {
    m_SeedImage = ImageType::New();
    m_SeedImage->SetRegions( this->m_BinaryImage->GetLargestPossibleRegion() );
    m_SeedImage->SetOrigin( this->m_BinaryImage->GetOrigin() );
    m_SeedImage->SetSpacing( this->m_BinaryImage->GetSpacing() );
    m_SeedImage->Allocate();
    m_SeedImage->FillBuffer( zero );
    m_SeedImage->Update();

    // Fill the seeds
    ImageIndexType index;
    ImagePointType pt;

    PointsContainerPointer points = this->m_Seeds->GetPoints();
    PointsContainerConstIterator it = points->Begin();
    PointsContainerConstIterator end = points->End();

    while( it != end )
      {
      pt.CastFrom( it->Value() );
      m_SeedImage->TransformPhysicalPointToIndex( pt, index );
      m_SeedImage->SetPixel( index, 1 + it->Index() );
      ++it;
      }
    }

  //Compute the voronoi map
  DistanceFilterPointer m_Dist = DistanceFilterType::New();
  m_Dist->SetInput( m_SeedImage );
  m_Dist->UseImageSpacingOn();
  m_Dist->SetInputIsBinary( true );
  m_Dist->UpdateLargestPossibleRegion();
  m_Dist->Update();

  /*
  typedef ImageFileWriter< ImageType > WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( "Voronoi.mhd" );
  writer->SetInput(  m_Dist->GetVoronoiMap() );
  writer->Write();


  typedef AndImageFilter< ImageType > AndFilterType;
  typedef typename AndFilterType::Pointer AndFilterPointer;
  AndFilterPointer and_filer = AndFilterType::New();
  and_filer->SetInput( 0, m_Dist->GetVoronoiMap() );
  and_filer->SetInput( 1, this->m_BinaryImage );
  and_filer->Update();

  this->m_OutputImage = and_filer->GetOutput();
  this->m_OutputImage->DisconnectPipeline();*/

  this->m_OutputImage = m_Dist->GetVoronoiMap();
  this->m_OutputImage->DisconnectPipeline();

  typedef ImageFileWriter< ImageType > WriterType;

  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( "Voronoi2.mhd" );
  writer->SetInput( this->m_OutputImage );
  writer->Write();

  IteratorType Vor_it( this->m_OutputImage,
                    this->m_OutputImage->GetLargestPossibleRegion() );

  ConstIteratorType Bin_it( this->m_BinaryImage,
                         this->m_BinaryImage->GetLargestPossibleRegion() );

  Vor_it.GoToBegin();
  Bin_it.GoToBegin();

  while( !Vor_it.IsAtEnd() )
    {
    if( ( Vor_it.GetIndex()[0] == 11 ) && ( Vor_it.GetIndex()[1] == 0 ) )
      {
      std::cout << Vor_it.GetIndex() << ' '
              << static_cast< int >( Vor_it.Get() ) << ' '
              << static_cast< int >( Bin_it.Get() ) << std::endl;
      }
    if( Bin_it.Get() == zero )
      {
      Vor_it.Set( zero );
      }
    ++Vor_it;
    ++Bin_it;
    }

  typename WriterType::Pointer writer2 = WriterType::New();
  writer2->SetFileName( "Output2.mhd" );
  writer2->SetInput( this->m_OutputImage );
  writer2->Write();
  }
}

#endif // __itkvtkMeshSplitterDanielssonDistanceImageFilter_txx
