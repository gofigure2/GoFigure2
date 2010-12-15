/*=========================================================================
  Author: $Author: krm15 $  // Author of last commit
  Version: $Rev: 738 $  // Revision of last commit
  Date: $Date: 2009-10-10 23:59:10 -0400 (Sat, 10 Oct 2009) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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

#ifndef __itkSingleCellSplitImageFilter_txx
#define __itkSingleCellSplitImageFilter_txx

#include "itkSingleCellSplitImageFilter.h"

namespace itk
{
//  Software Guide : BeginCodeSnippet
template< class TInputImage >
SingleCellSplitImageFilter< TInputImage >
::SingleCellSplitImageFilter()
{
  m_SeedImage = 0;
  m_ForegroundValue = 1;
  
  this->Superclass::SetNumberOfRequiredInputs ( 1 );
  this->Superclass::SetNumberOfRequiredOutputs ( 1 );

  this->Superclass::SetNthOutput ( 0, TInputImage::New() );
}


template< class TInputImage >
void SingleCellSplitImageFilter< TInputImage >::
GenerateData()
{
  if ( !m_SeedImage )
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
    ListIteratorType lIt = m_Seeds.begin();
    unsigned int i = 1;
    while( lIt != m_Seeds.end() )
    {
      pt = *lIt;
      m_SeedImage->TransformPhysicalPointToIndex( pt, index );
      m_SeedImage->SetPixel( index, i );
//       std::cout << index << std::endl;
      lIt++;
      i++;
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
      It1.Set( 0 );
    ++It1;
    ++It2;
  }
  
  this->GraftOutput( output );

  return;
}

template< class TInputImage >
void SingleCellSplitImageFilter< TInputImage >::
PrintSelf ( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf ( os,indent );
  os << indent << "Class Name:            " << GetNameOfClass() << std::endl;
}

} /* end namespace itk */

#endif
