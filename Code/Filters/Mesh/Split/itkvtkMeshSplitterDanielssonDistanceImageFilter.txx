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
vtkMeshSplitterDanielssonDistanceImageFilter() : Superclass()
{}

template< class TImage >
void
vtkMeshSplitterDanielssonDistanceImageFilter< TImage >::
SplitBinaryImage()
{
  ImagePixelType zero = NumericTraits< ImagePixelType >::Zero;

  ImagePointer seed_image = ImageType::New();
  seed_image->SetRegions( this->m_BinaryImage->GetLargestPossibleRegion() );
  seed_image->SetOrigin( this->m_BinaryImage->GetOrigin() );
  seed_image->SetSpacing( this->m_BinaryImage->GetSpacing() );
  seed_image->Allocate();
  seed_image->FillBuffer( zero );
  seed_image->Update();

  // Fill the seeds
  ImageIndexType index;
  ImagePointType pt;

  PointsContainerPointer points = this->m_Seeds->GetPoints();
  PointsContainerConstIterator it = points->Begin();
  PointsContainerConstIterator end = points->End();

  while( it != end )
    {
    pt.CastFrom( it->Value() );
    seed_image->TransformPhysicalPointToIndex( pt, index );
    seed_image->SetPixel( index, 1 + it->Index() );
    ++it;
    }

  //Compute the voronoi map
  DistanceFilterPointer distance_filter = DistanceFilterType::New();
  distance_filter->SetInput( seed_image );
  distance_filter->UseImageSpacingOn();
  distance_filter->SetInputIsBinary( true );
  distance_filter->UpdateLargestPossibleRegion();
  distance_filter->Update();

  this->m_OutputImage = distance_filter->GetVoronoiMap();
  this->m_OutputImage->DisconnectPipeline();

  IteratorType Vor_it( this->m_OutputImage,
                    this->m_OutputImage->GetLargestPossibleRegion() );

  ConstIteratorType Bin_it( this->m_BinaryImage,
                         this->m_BinaryImage->GetLargestPossibleRegion() );

  Vor_it.GoToBegin();
  Bin_it.GoToBegin();

  while( !Vor_it.IsAtEnd() )
    {
    if( Bin_it.Get() == zero )
      {
      Vor_it.Set( zero );
      }
    ++Vor_it;
    ++Bin_it;
    }
  }
}

#endif // __itkvtkMeshSplitterDanielssonDistanceImageFilter_txx
