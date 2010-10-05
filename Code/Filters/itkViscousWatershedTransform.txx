/*=========================================================================
  Author: $Author: krm15 $  // Author of last commit
  Version: $Rev: 1658 $  // Revision of last commit
  Date: $Date: 2010-06-14 15:49:25 -0400 (Mon, 14 Jun 2010) $  // Date of last commit
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

#ifndef __itkViscousWatershedTransform_txx
#define __itkViscousWatershedTransform_txx

#include "itkViscousWatershedTransform.h"

namespace itk
{
template < class TFeatureImage, class TInputImage, class TSegmentImage >
ViscousWatershedTransform< TFeatureImage, TInputImage, TSegmentImage >
::ViscousWatershedTransform()
{
  this->Superclass::SetNumberOfRequiredInputs ( 1 );
  this->Superclass::SetNumberOfRequiredOutputs ( 1 );
  
  m_InitialLevel = 0;
  m_FinalLevel = 255;
  m_Increment = 1;
  m_LargestRadius = 10;
  m_Slope = 10;

  this->Superclass::SetNthOutput ( 0,TFeatureImage::New() );
}

template < class TFeatureImage, class TInputImage, class TSegmentImage >
void
ViscousWatershedTransform< TFeatureImage, TInputImage, TSegmentImage >::
GenerateData()
{
  FeatureImageConstPointer m_Input = this->GetInput();
  FeatureImageRegionType region = m_Input->GetLargestPossibleRegion(); 

  FeatureImagePointer m_Output;
  {
    ThresholdFilterPointer m_threshold = ThresholdFilterType::New();
    m_threshold->SetLowerThreshold ( 0 );
    m_threshold->SetUpperThreshold ( m_InitialLevel );
    m_threshold->SetInsideValue ( m_InitialLevel );
    m_threshold->SetOutsideValue ( 0 );
    m_threshold->SetInput ( m_Input );
    m_threshold->Update();
    m_Output = m_threshold->GetOutput();
    m_Output->DisconnectPipeline();
  }
  
  typename StructuringElementType::SizeType ballSize;
  FeatureIteratorType oIt( m_Output, region );
  unsigned int radius;
  for( unsigned int i = m_InitialLevel + m_Increment; i <= m_FinalLevel; i = i + m_Increment )
  {
    std::cout << i << std::endl;
    // Extract levelset of the function
    ThresholdFilterPointer m_threshold = ThresholdFilterType::New();
    m_threshold->SetLowerThreshold ( 0 );
    m_threshold->SetUpperThreshold ( i );
    m_threshold->SetInsideValue ( 0 );
    m_threshold->SetOutsideValue ( 1 );
    m_threshold->SetInput ( m_Input );
    m_threshold->Update();
    
    // Do closing
    if ( i > m_LargestRadius * m_Slope )
    {
      radius = 1;
    }
    else
    {
      radius = static_cast<unsigned int>( m_LargestRadius  - i/m_Slope );
    }
    ballSize.Fill( radius );

    StructuringElementType structuringElement;
    structuringElement.SetRadius( ballSize );
    structuringElement.CreateStructuringElement();

    ClosingFilterPointer close = ClosingFilterType::New();
    close->SetKernel( structuringElement );
    close->SetInput( m_threshold->GetOutput() );
    close->SetForegroundValue( 1 );
    close->Update();

    // Iterate over the output image
    FeatureIteratorType iIt( close->GetOutput(), region );
    iIt.GoToBegin();
    oIt.GoToBegin();
    while( !oIt.IsAtEnd() )
    {
      if ( ( oIt.Get() == 0 ) && ( iIt.Get() == 0 ) )
      {
        oIt.Set( i );
      }
      ++oIt;
      ++iIt;
    }
  }
  
  this->GraftOutput( m_Output );
}

template < class TFeatureImage, class TInputImage, class TSegmentImage >
void
ViscousWatershedTransform< TFeatureImage, TInputImage, TSegmentImage >::
PrintSelf ( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf ( os,indent );
}

} /* end namespace itk */

#endif
