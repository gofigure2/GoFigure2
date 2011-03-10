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
#ifndef __itkConvertMeshesToLabelImageFilter_txx
#define __itkConvertMeshesToLabelImageFilter_txx

#include "itkConvertMeshesToLabelImageFilter.h"

static itk::SimpleFastMutexLock m_MutexCMLF;

namespace itk
{

template< class TImage >
ConvertMeshesToLabelImageFilter< TImage >
::ConvertMeshesToLabelImageFilter()
{
  m_NumberOfMeshes = 0;
  m_NumberOfThreads = 1;
  m_Input = NULL;
}


template< class TImage >
void
ConvertMeshesToLabelImageFilter< TImage >::
SetMeshes( const MeshVectorType& iMeshes )
{
  m_Meshes = iMeshes;
  this->Modified();
}


template< class TImage >
void
ConvertMeshesToLabelImageFilter< TImage >::
Update()
{
  // Create an empty image
  if ( !this->m_Input )
    {
    itkGenericExceptionMacro( << "Input is NULL" );
    }
  this->GenerateData();
}

template< class TImage >
void
ConvertMeshesToLabelImageFilter< TImage >::
GenerateData()
{
  m_NumberOfMeshes = this->m_Meshes.size();

  ThreadStruct str;
  str.Filter  = this;

  ThreaderPointer threader = ThreaderType::New();
  threader->SetNumberOfThreads( m_NumberOfThreads );
  threader->SetSingleMethod( this->ThreaderCallback, &str );
  threader->SingleMethodExecute();
}

template< class TImage >
ITK_THREAD_RETURN_TYPE
ConvertMeshesToLabelImageFilter< TImage >::
ThreaderCallback(void * arg)
{
  unsigned int ThreadId =
    ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  ThreadStruct * str =
    (ThreadStruct *) (((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  size_t numOfMeshes = str->Filter->m_NumberOfMeshes;
  size_t numOfThreads = static_cast< size_t >( str->Filter->m_NumberOfThreads );

  unsigned int MeshChunk =
      static_cast< unsigned int >( numOfMeshes/ numOfThreads);

  ++MeshChunk;

  unsigned int startLabel = ThreadId * MeshChunk + 1;
  unsigned int endLabel = ThreadId * MeshChunk + MeshChunk;

  if ( ThreadId == numOfThreads-1 )
    {
    endLabel = numOfMeshes;
    }

  str->Filter->ThreadedExtractMesh( startLabel, endLabel );

  return ITK_THREAD_RETURN_VALUE;
}


template< class TImage >
void
ConvertMeshesToLabelImageFilter< TImage >::
ThreadedExtractMesh( const unsigned int& startLabel,
                     const unsigned int& endLabel )
{
  //PointType origin = m_Input->GetOrigin();
  SpacingType spacing = m_Input->GetSpacing();
  RegionType m_LargestRegion = m_Input->GetLargestPossibleRegion();

  PointType pmin, pmax;
  IndexType imin, imax, null_index;
  SizeType size;
  RegionType region, newRegion;
  PointType newOrigin;

  for( unsigned int i = 0; i < ImageDimension; i++ )
    {
    null_index[i] = 0;
    }

  unsigned int label = startLabel;

  while( label <= endLabel )
    {
    MeshPointer temp_mesh = m_Meshes[label-1];

    // Convert the bounding box into an image region and origin
    pmin = temp_mesh->GetBoundingBox()->GetMinimum();
    pmax = temp_mesh->GetBoundingBox()->GetMaximum();

    m_Input->TransformPhysicalPointToIndex( pmin, imin );
    m_Input->TransformPhysicalPointToIndex( pmax, imax );

    for( unsigned int i = 0; i < ImageDimension; ++i )
      {
      imin[i] = imin[i] - 1;
      imax[i] = imax[i] + 1;

      size[i] = imax[i] - imin[i] + 1;
      }

    region.SetIndex( imin );
    region.SetSize( size );
    region.Crop( m_LargestRegion );

    m_Input->TransformIndexToPhysicalPoint( imin, newOrigin );

    newRegion.SetIndex( null_index );
    newRegion.SetSize( region.GetSize() );

//     std::cout << region    << std::endl;
//     std::cout << newRegion << std::endl;

    // Call TriangleMeshToBinaryImageFilter
    MeshToImageFilterPointer imageFilter = MeshToImageFilterType::New();
    imageFilter->SetInput( temp_mesh );
    imageFilter->SetInsideValue( label-1 );
    imageFilter->SetOrigin( newOrigin );
    imageFilter->SetSpacing( spacing );
    imageFilter->SetIndex( imin );
    imageFilter->SetSize( size );
    imageFilter->Update();

    ImagePointer comp = imageFilter->GetOutput();

    // Copy the small output image here into the m_Input
    IteratorType iIt( m_Input, region );
    iIt.GoToBegin();

    IteratorType cIt( comp, comp->GetLargestPossibleRegion() );
    cIt.GoToBegin();

    while( !iIt.IsAtEnd() )
      {
      if ( ( cIt.Get() != 0 ) && ( iIt.Get() == 0 ) )
        {
        iIt.Set( cIt.Get() );
        }
      ++iIt;
      ++cIt;
      }

    ++label;
    }
}


/** Print Self information */
template<class TImage >
void
ConvertMeshesToLabelImageFilter< TImage >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "NumberOfMeshes: " << m_NumberOfMeshes << std::endl;
}

} // end namespace

#endif
