/*=========================================================================
  Author: $Author: krm15 $  // Author of last commit
  Version: $Rev: 720 $  // Revision of last commit
  Date: $Date: 2009-09-30 20:23:54 -0400 (Wed, 30 Sep 2009) $  // Date of last commit
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

#ifndef __itkGradientWeightedIntensityImageFilter_txx
#define __itkGradientWeightedIntensityImageFilter_txx

#include "itkGradientWeightedIntensityImageFilter.h"

namespace itk
{
//  Software Guide : BeginCodeSnippet
template< class TFeatureImage,class TInputImage,class TSegmentImage >
GradientWeightedIntensityImageFilter< TFeatureImage, TInputImage, TSegmentImage >
::GradientWeightedIntensityImageFilter()
{
  m_LargestCellRadius = 5.0;
  m_NucleiSigma = 0.4;
  m_Alpha = 1.0;
  m_Beta = 3.0;
  m_Gamma = 2.0;

  m_Blob = 0;

  this->Superclass::SetNumberOfRequiredInputs ( 1 );
  this->Superclass::SetNumberOfRequiredOutputs ( 1 );
  this->Superclass::SetNthOutput ( 0, TInputImage::New() );
}


template< class TFeatureImage,class TInputImage,class TSegmentImage >
void
GradientWeightedIntensityImageFilter< TFeatureImage, TInputImage, TSegmentImage >::
GenerateData()
{
  ImagePointer outputImg = ImageType::New();
  outputImg->CopyInformation( this->GetInput() );
  outputImg->SetRegions( this->GetInput()->GetLargestPossibleRegion() );
  outputImg->Allocate();
  outputImg->FillBuffer( 1.0 );

	IteratorType oIt ( outputImg, outputImg->GetLargestPossibleRegion() );

	double q;
  if ( m_Beta > 0 )
  {
  	FIteratorType lIt ( m_Blob, m_Blob->GetLargestPossibleRegion() );

		oIt.GoToBegin();
		lIt.GoToBegin();
		while( !oIt.IsAtEnd() )
		{
      q = vcl_exp( m_Beta * static_cast<double>( lIt.Get() )/255 );
		  oIt.Set( q * oIt.Get() );
		  ++oIt;
			++lIt;
		}
  }

  if ( m_Gamma > 0 )
  {
    IteratorType lIt ( m_Form, m_Form->GetLargestPossibleRegion() );

    oIt.GoToBegin();
    lIt.GoToBegin();
    while( !oIt.IsAtEnd() )
    {
      q = vcl_exp( -m_Gamma * static_cast<double>( lIt.Get() ) );
      oIt.Set( q * oIt.Get() );
      ++oIt;
      ++lIt;
    }
  }

  // Compute the gradient magnitude
  if ( m_Alpha > 0 )
  {
    GradientFilterPointer m_gradientMagnitude = GradientFilterType::New();
    m_gradientMagnitude->SetInput ( this->GetInput () );
    m_gradientMagnitude->SetSigma ( m_NucleiSigma );
    m_gradientMagnitude->Update();

    RescaleFilterPointer rescale = RescaleFilterType::New();
    rescale->SetInput( m_gradientMagnitude->GetOutput() );
    rescale->SetOutputMinimum( 0 );
    rescale->SetOutputMaximum( 1 );
    rescale->Update();

		IteratorType gIt ( rescale->GetOutput(),
	    rescale->GetOutput()->GetLargestPossibleRegion() );

		oIt.GoToBegin();
		gIt.GoToBegin();
		while( !oIt.IsAtEnd() )
		{
      q = vcl_exp( m_Alpha * static_cast<double>( gIt.Get() ) );
		  oIt.Set( q * oIt.Get() );
		  ++oIt;
			++gIt;
		}
  }

  ConstIteratorType dIt ( this->GetInput(),
	  this->GetInput()->GetLargestPossibleRegion() );

  ImagePixelType p;
  oIt.GoToBegin();
  dIt.GoToBegin();
  while ( !oIt.IsAtEnd() )
    {
    p = ( 255 - dIt.Get() ) * oIt.Get();
    p = p > 255 ? 255 : p;
    oIt.Set ( p );
    ++oIt;
    ++dIt;
    }

    this->GraftOutput ( outputImg );
}

template< class TFeatureImage,class TInputImage,class TSegmentImage >
void
GradientWeightedIntensityImageFilter< TFeatureImage, TInputImage, TSegmentImage >::
PrintSelf ( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf ( os,indent );
  os << indent << "Class Name:              " << this->GetNameOfClass( ) <<
    std::endl;
}

} /* end namespace itk */

#endif
