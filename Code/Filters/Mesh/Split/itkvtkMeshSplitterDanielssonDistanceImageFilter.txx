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

namespace itk
{
template< class TImage >
vtkMeshSplitterDanielssonDistanceImageFilter< TImage >::
vtkMeshSplitterDanielssonDistanceImageFilter() : Superclass(),
  m_SeedImage( NULL ), m_ForegroundValue( 1 ) {}

template< class TImage >
void
vtkMeshSplitterDanielssonDistanceImageFilter< TImage >::
SplitBinaryImage()
{
  if( m_SeedImage.IsNull() )
    {
    m_SeedImage = ImageType::New();
    m_SeedImage->SetRegions( this->m_Image->GetLargestPossibleRegion() );
    m_SeedImage->SetOrigin(  this->m_Image->GetOrigin() );
    m_SeedImage->SetSpacing( this->m_Image->GetSpacing() );
    m_SeedImage->Allocate();
    m_SeedImage->FillBuffer( 0 );
    m_SeedImage->Update();

    // Fill the seeds
    ImageIndexType index;
    ImagePointType pt;

    PointsContainerPointer points = this->m_Seeds->GetPoints();
    PointsContainerConstIterator it = points->Begin();
    PointsContainerConstIterator end = points->Begin();

    while( it != end )
      {
      pt.CastFrom( it->Value() );
      m_SeedImage->TransformPhysicalPointToIndex( pt, index );
      m_SeedImage->SetPixel( index, it->Index() );
      ++it;
      }
    }

  //Compute the voronoi map
  DistanceFilterPointer m_Dist = DistanceFilterType::New();
  m_Dist->SetInput( m_SeedImage );
  m_Dist->UseImageSpacingOn();
  m_Dist->SetInputIsBinary( true );
  m_Dist->UpdateLargestPossibleRegion();

  this->m_OutputImage = m_Dist->GetVoronoiMap();
  this->m_OutputImage->DisconnectPipeline();

  IteratorType It1( this->m_OutputImage,
                    this->m_OutputImage->GetLargestPossibleRegion() );

  ConstIteratorType It2( this->m_Image,
                         this->m_OutputImage->GetLargestPossibleRegion() );
  It1.GoToBegin();
  It2.GoToBegin();

  while( !It1.IsAtEnd() )
    {
    if( It2.Get() != m_ForegroundValue )
      {
      It1.Set( 0 );
      }
    ++It1;
    ++It2;
    }
  }
}

#endif // __itkvtkMeshSplitterDanielssonDistanceImageFilter_txx
