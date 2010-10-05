/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMultiScaleLoGDistanceImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2007/06/20 16:03:23 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMultiScaleLoGDistanceImageFilter_txx
#define __itkMultiScaleLoGDistanceImageFilter_txx

#include "itkMultiScaleLoGDistanceImageFilter.h"
#include "vnl/vnl_math.h"

#define EPSILON  1e-03

namespace itk
{
/**
 * Constructor
 */

template <class InputImageType, class DistanceMapImageType, class OutputImageType >
MultiScaleLoGDistanceImageFilter
<InputImageType, DistanceMapImageType, OutputImageType>
::MultiScaleLoGDistanceImageFilter()
{
  // default sigma min-max in world coordinate?
  m_SigmaMin = 0.2;
  m_SigmaMax = 2.0;
  m_NumberOfSigmaSteps = 4;
  m_ComputeOutsideForeground = false;

  // we create the laplacian filter
  m_LoGFilter = LoGFilterType::New();

  this->Superclass::SetNumberOfRequiredInputs ( 1 );
  this->Superclass::SetNumberOfRequiredOutputs ( 1 );
  this->Superclass::SetNthOutput ( 0, OutputImageType::New() );
}

/**
 * compute LoG at several sigma and give outputs to UpdateMaximumResponse
 */
template <class InputImageType, class DistanceMapImageType, class OutputImageType >
void
MultiScaleLoGDistanceImageFilter
<InputImageType, DistanceMapImageType, OutputImageType>
::GenerateData()
{

  // Allocate the output, and fill it with zeros
  OutputImagePointer outputImage = this->GetOutput();

  // Zero the output
  outputImage->SetBufferedRegion( outputImage->GetRequestedRegion() );
  outputImage->Allocate();
  outputImage->FillBuffer ( 0);//NumericTraits<OutputPixelType>::Zero );

  InputImageConstPointer inputImage = this->GetInput();

  // we set the laplacian filter's input as the input image
  m_LoGFilter->SetInput( inputImage );
  // true for multiscale analysis :
  // one doesn't want the image to fade away with high scale parameters
  m_LoGFilter->SetNormalizeAcrossScale( true );

  // multiscale analysis :
  double sigma = m_SigmaMin; // we start at sigma min
  int scaleLevel = 1;// sigma min corresponds to scale 1 (first blurring)

  while ( sigma <= m_SigmaMax )
    {
    std::cout << "Computing LoG for sigma = " << sigma << std::endl;
    m_LoGFilter->SetSigma( sigma );
    m_LoGFilter->Update();
    std::cout << "Filtering LoG with distancemap " << std::endl;
    this->UpdateMaximumResponse(scaleLevel);
    sigma  = this->ComputeSigmaValue( scaleLevel );// compute sigma according to the scale level
    scaleLevel++;// next scale level
    }
}


/**
 * Select maximal output of LoG accross cales,
 * depending on the Signed Distance Map.
 */
template <class InputImageType, class DistanceMapImageType, class OutputImageType >
void
MultiScaleLoGDistanceImageFilter
<InputImageType, DistanceMapImageType, OutputImageType>
::UpdateMaximumResponse(const int& scaleLevel)
{


  /** define iterators : */
  // over the output
  OutputIteratorType oit(this->GetOutput(),
    this->GetOutput()->GetLargestPossibleRegion());
  oit.GoToBegin();

  // over the Laplacian of Gaussian output
  ConstLoGIteratorType it(m_LoGFilter->GetOutput(),
    m_LoGFilter->GetOutput()->GetLargestPossibleRegion());
  it.GoToBegin();

  // over the SDF map
  ConstDistanceMapIteratorType sdfit( m_DistanceMap,
    m_DistanceMap->GetLargestPossibleRegion());
  sdfit.GoToBegin();

  double m_SigmaSquare = m_Sigma*m_Sigma;

  /** main loop */
  if (m_ComputeOutsideForeground) // if we allow computation in positive sdf area
                            // (outside foreground)
    {
    while( !oit.IsAtEnd() )
      {
      // if the current sigma is bellow 2*distancemap (<=>Sigma^2 < 4*distmap^2),
      // or if it is the first sigma :
      if ( (m_SigmaSquare  <= 4*vcl_abs( sdfit.Value() ) ) || ( scaleLevel == 1) )
        {
        // we take the LoG
        // if the output at this scale is larger than the previous value stored
        // Note: the LoG of a bright blob is a negative value
        if( ( oit.Value() < -( it.Value() ) ) || ( scaleLevel == 1) )
          {
          oit.Value() = -it.Value();
          }
        }
      // next voxel
      ++oit;
      ++it;
      ++sdfit;
      }
    }
  else // if we don't allow computation in positive sdf areas
    {
    while( !oit.IsAtEnd() )
      {
      if ( sdfit.Value() <= 0 )
        {
        // if the current sigma is bellow 2*distancemap (<=>Sigma^2 < 4*distmap^2),
        // or if it is the first sigma :
        if ( ( m_SigmaSquare <= -4*sdfit.Value() ) || ( scaleLevel == 1) )
          {
          // we take the LoG
          // if the output at this scale is bigger (negative) than the previous output
          if( ( oit.Value() < -( it.Value() ) ) || ( scaleLevel == 1) )
            {
            oit.Value() = -(it.Value());
            }
          }
        // next voxel
        }
      ++oit;
      ++it;
      ++sdfit;
      }
    }
}


template <class InputImageType, class DistanceMapImageType, class OutputImageType >
double
MultiScaleLoGDistanceImageFilter
<InputImageType, DistanceMapImageType, OutputImageType>
::ComputeSigmaValue( int ScaleLevel )
{
  double stepSize =
     ( vcl_log( m_SigmaMax ) - vcl_log( m_SigmaMin) ) / m_NumberOfSigmaSteps;

  if( stepSize <= 1e-10 )
    {
    stepSize = 1e-10;
    }

  return ( vcl_exp( vcl_log (m_SigmaMin) + stepSize * ScaleLevel) );
}


template <class InputImageType, class DistanceMapImageType, class OutputImageType >
void
MultiScaleLoGDistanceImageFilter
<InputImageType, DistanceMapImageType, OutputImageType>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "SigmaMin:  " << m_SigmaMin << std::endl;
  os << indent << "SigmaMax:  " << m_SigmaMax  << std::endl;
  os << indent << "NumberOfSigmaSteps:  " << m_NumberOfSigmaSteps  << std::endl;
  os << indent << "ComputeOutsideForeground:  " << m_ComputeOutsideForeground  << std::endl;
}


} // end namespace itk

#endif
