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

#ifndef __itkvtkMeshSplitterDanielssonDistanceImageFilter_h
#define __itkvtkMeshSplitterDanielssonDistanceImageFilter_h

#include "itkvtkMeshSplitterImageFilterBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

#include "itkDanielssonDistanceMapImageFilter.h"

namespace itk
{
template< class TImage >
class vtkMeshSplitterDanielssonDistanceImageFilter :
  public vtkMeshSplitterImageFilterBase< TImage >
  {
public:
  typedef vtkMeshSplitterDanielssonDistanceImageFilter Self;
  typedef vtkMeshSplitterImageFilterBase< TImage > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  typedef typename Superclass::ImageType    ImageType;
  typedef typename Superclass::ImagePointer ImagePointer;
  typedef typename Superclass::ImagePixelType ImagePixelType;
  typedef typename Superclass::ImageIndexType ImageIndexType;
  typedef typename Superclass::ImagePointType ImagePointType;

  typedef typename Superclass::PointsContainerPointer
    PointsContainerPointer;
  typedef typename Superclass::PointsContainerConstIterator
    PointsContainerConstIterator;

  typedef ImageRegionIterator< ImageType > IteratorType;
  typedef ImageRegionConstIterator< ImageType > ConstIteratorType;

  typedef DanielssonDistanceMapImageFilter< ImageType, ImageType >
    DistanceFilterType;
  typedef typename DistanceFilterType::Pointer DistanceFilterPointer;

  itkGetConstMacro ( ForegroundValue, ImagePixelType );
  itkSetMacro ( ForegroundValue, ImagePixelType );

protected:
  vtkMeshSplitterDanielssonDistanceImageFilter() :
    Superclass(), m_SeedImage( NULL ), m_ForegroundValue( 1 ) {}
  ~vtkMeshSplitterDanielssonDistanceImageFilter() {}

  ImagePointer m_SeedImage;
  ImagePixelType  m_ForegroundValue;

  void SplitBinaryImage()
    {
    if( m_SeedImage.IsNull() )
      {
      m_SeedImage = ImageType::New();
      m_SeedImage->SetRegions( this->GetInput()->GetLargestPossibleRegion() );
      m_SeedImage->SetOrigin(  this->GetInput()->GetOrigin() );
      m_SeedImage->SetSpacing( this->GetInput()->GetSpacing() );
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

    ImagePointer output = m_Dist->GetVoronoiMap();
    output->DisconnectPipeline();

    IteratorType It1( output, output->GetLargestPossibleRegion() );
    ConstIteratorType It2( this->GetInput(), output->GetLargestPossibleRegion() );
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

private:
  vtkMeshSplitterDanielssonDistanceImageFilter( const Self& );
  void operator = ( const Self& );
  };
}

#endif // __itkvtkMeshSplitterDanielssonDistanceImageFilter_h
