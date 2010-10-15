/*=========================================================================
  Author: $Author: krm15 $  // Author of last commit
  Version: $Rev: 1483 $  // Revision of last commit
  Date: $Date: 2010-05-20 15:10:30 -0400 (Thu, 20 May 2010) $  // Date of last commit
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

#ifndef __itkCellFeatureGenerator_txx
#define __itkCellFeatureGenerator_txx

#include "itkCellFeatureGenerator.h"

namespace itk
{
//  Software Guide : BeginCodeSnippet
template< class TFeatureImage, class TInputImage, class TSegmentImage >
CellFeatureGenerator< TFeatureImage, TInputImage, TSegmentImage >
::CellFeatureGenerator()
{
  m_NucleiSigma = 0.2;
  m_MembraneSigma = 0.2;
  m_LargestCellRadius = 4.0;
  m_DistanceMapWeight = 1.0;
  m_NucleiGradientWeight = 0.0;
  m_MembraneWeight = 0.0;

  m_ForegroundMap = NULL;
  m_DistanceMap = NULL;
  m_Gradient = NULL;

  this->Superclass::SetNumberOfRequiredInputs (2);
  this->Superclass::SetNumberOfRequiredOutputs (1);
  this->Superclass::SetNthOutput ( 0, TInputImage::New() );
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
CellFeatureGenerator< TFeatureImage, TInputImage, TSegmentImage >
::DistanceMap()
{
  MaurerPointer m_Maurer = MaurerType::New();

  m_Maurer->SetInput (m_ForegroundMap);
  m_Maurer->SetSquaredDistance (0);
  m_Maurer->SetUseImageSpacing (1);
  m_Maurer->SetInsideIsPositive (0);
  m_Maurer->Update();

  AbsFilterPointer m_absFilter = AbsFilterType::New();
  m_absFilter->SetInput ( m_Maurer->GetOutput() );
  m_absFilter->Update();

  ThreshFilterPointer m_thresh = ThreshFilterType::New();
  m_thresh->SetInput ( m_absFilter->GetOutput() );
  m_thresh->ThresholdAbove (m_LargestCellRadius);
  m_thresh->SetOutsideValue (m_LargestCellRadius);
  m_thresh->Update();

  m_DistanceMap = m_thresh->GetOutput();
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
CellFeatureGenerator< TFeatureImage, TInputImage, TSegmentImage >
::Gradient()
{
  // Compute the gradient magnitude
  GradientFilterPointer m_gradientMagnitude = GradientFilterType::New();

  m_gradientMagnitude->SetInput ( this->GetInput (0) );
  m_gradientMagnitude->SetSigma (m_NucleiSigma);
  m_gradientMagnitude->Update();

  IteratorType distIt( m_DistanceMap, m_DistanceMap->GetLargestPossibleRegion() );

  IteratorType gradientIt( m_gradientMagnitude->GetOutput(),
                           m_gradientMagnitude->GetOutput()->GetLargestPossibleRegion() );

  double alpha = -1, beta = 0;
  float  k1 = 0, k2 = 0;
  int    n1 = 0, n2 = 0;
  for ( distIt.GoToBegin(), gradientIt.GoToBegin(); !distIt.IsAtEnd(); ++distIt,
        ++gradientIt )
    {
    if ( distIt.Get() < m_LargestCellRadius / 4 )
      {
      k1 += gradientIt.Get();
      ++n1;
      }
    else
      {
      k2 += gradientIt.Get();
      ++n2;
      }
    }

  if ( ( n1 > 0 ) && ( n2 > 0 ) )
    {
    k1 /= static_cast< float >(n1);
    k2 /= static_cast< float >(n2);
    alpha = ( k2 - k1 ) / 2;
    beta = ( k2 + k1 ) / 2;
    }
//   std::cout << k1 << ' ' << k2 << std::endl;
//   std::cout << "Estimating Alpha and Beta complete..." << std::endl;
//   std::cout << alpha << ' ' << beta << std::endl;

  SigmoidFilterPointer m_sigmoid = SigmoidFilterType::New();
  m_sigmoid->SetInput ( m_gradientMagnitude->GetOutput() );
  m_sigmoid->SetOutputMinimum (0.0);
  m_sigmoid->SetOutputMaximum (1);
  m_sigmoid->SetAlpha (alpha);
  m_sigmoid->SetBeta (beta);
  m_sigmoid->Update();

  m_Gradient = m_sigmoid->GetOutput();
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
CellFeatureGenerator< TFeatureImage, TInputImage, TSegmentImage >::GenerateData()
{
  ImagePointer outputImg = ImageType::New();

  outputImg->SetRegions ( this->GetInput(0)->GetLargestPossibleRegion() );
  outputImg->SetSpacing ( this->GetInput(0)->GetSpacing() );
  outputImg->SetOrigin ( this->GetInput(0)->GetOrigin() );
  outputImg->Allocate();
  outputImg->FillBuffer (0);

  if ( m_DistanceMapWeight > 0 )
    {
    DistanceMap();
    }
  m_ForegroundMap = 0;

  if ( m_NucleiGradientWeight > 0 )
    {
    Gradient();
    m_Gradient->SetSpacing ( m_DistanceMap->GetSpacing() );
    }
  else
    {
    m_Gradient = outputImg;
    }

  FeatureMinMaxCalculatorPointer minMax1 = FeatureMinMaxCalculatorType::New();
  minMax1->SetImage( this->GetInput(1) );
  minMax1->ComputeMaximum();
  double mmax = static_cast< double >( minMax1->GetMaximum() );

  MinMaxCalculatorPointer minMax2 = MinMaxCalculatorType::New();
  minMax2->SetImage(m_DistanceMap);
  minMax2->ComputeMaximum();
  double dmax = static_cast< double >( minMax2->GetMaximum() );

  double weight = m_DistanceMapWeight + m_NucleiGradientWeight + m_MembraneWeight;

  IteratorType It( outputImg,
                   outputImg->GetLargestPossibleRegion() );

  IteratorType dIt( m_DistanceMap,
                    m_DistanceMap->GetLargestPossibleRegion() );

  IteratorType gIt( m_Gradient,
                    m_Gradient->GetLargestPossibleRegion() );

  ConstIteratorType mIt( this->GetInput(1),
                         this->GetInput(1)->GetLargestPossibleRegion() );

  double dterm, gterm, mterm;

  mIt.GoToBegin();
  gIt.GoToBegin();
  dIt.GoToBegin();
  It.GoToBegin();
  while ( !mIt.IsAtEnd() )
    {
    dterm = ( dIt.Get() / dmax ) * m_DistanceMapWeight;
    gterm = gIt.Get() * m_NucleiGradientWeight;
    mterm = ( 1 - mIt.Get() / mmax ) * m_MembraneWeight;

    It.Set ( static_cast< ImagePixelType >(
               ( dterm + gterm + mterm ) / weight) );

    ++mIt;
    ++gIt;
    ++dIt;
    ++It;
    }

  this->GraftOutput (outputImg);
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
CellFeatureGenerator< TFeatureImage, TInputImage, TSegmentImage >::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf (os, indent);
  os << indent << "Class Name:              " << this->GetNameOfClass()
     << std::endl;
  os << indent << "Nuclei Sigma:              " << this->GetNucleiSigma() << std::endl;
  os << indent << "Membrane Sigma:       " << this->GetMembraneSigma()
     << std::endl;
  os << indent << "Largest Cell Radius:     " << this->GetLargestCellRadius()
     << std::endl;
  os << indent << "DistanceMapWeight:       " << this->GetDistanceMapWeight()
     << std::endl;
  os << indent << "NucleiGradientWeight: " << this->GetNucleiGradientWeight()
     << std::endl;
  os << indent << "MembraneWeight:         " << this->GetMembraneWeight()
     << std::endl;
}
} /* end namespace itk */

#endif
