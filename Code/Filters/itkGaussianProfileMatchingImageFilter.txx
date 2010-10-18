/*=========================================================================
  Author: $Author: agouaillard $  // Author of last commit
  Version: $Rev: 868 $  // Revision of last commit
  Date: $Date: 2009-11-20 18:42:47 -0500 (Fri, 20 Nov 2009) $  // Date of last commit
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

#ifndef __itkGaussianProfileMatchingImageFilter_txx
#define __itkGaussianProfileMatchingImageFilter_txx

#include "itkGaussianProfileMatchingImageFilter.h"

namespace itk
{
template< class TFeatureImage, class TInputImage, class TSegmentImage >
GaussianProfileMatchingImageFilter< TFeatureImage, TInputImage, TSegmentImage >
::GaussianProfileMatchingImageFilter():m_SigmaForm (2.0), m_LargestCellRadius (4.0)
{
  m_Blob = 0;

  this->Superclass::SetNumberOfRequiredInputs (1);
  this->Superclass::SetNumberOfRequiredOutputs (1);

  this->Superclass::SetNthOutput ( 0, TInputImage::New() );
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
typename GaussianProfileMatchingImageFilter< TFeatureImage, TInputImage, TSegmentImage >::
ImagePointer
GaussianProfileMatchingImageFilter< TFeatureImage, TInputImage, TSegmentImage >
::InitializeBlob
  (FeatureImageSpacingType spacing, FeatureImageSizeType CellExtent)
{
  ImageRegionType blobRegion;
  ImageIndexType  blobStart, blobOrigin;
  unsigned int    i;

  for ( i = 0; i < ImageDimension; i++ )
    {
    blobOrigin[i] = static_cast< ImageSizeValueType >(CellExtent[i] / 2);
    blobStart[i] = 0;
    }
  blobRegion.SetSize (CellExtent);
  blobRegion.SetIndex (blobStart);

  ImagePointer blob = ImageType::New();
  blob->SetRegions (blobRegion);
  blob->Allocate();
  blob->SetSpacing (spacing);

  float          T = -1. / ( 2. * m_SigmaForm * m_SigmaForm );
  ImageIndexType idx;
  float          dx, sum;

  IndexIteratorType blobIt( blob, blob->GetLargestPossibleRegion() );

  for ( blobIt.GoToBegin(); !blobIt.IsAtEnd(); ++blobIt )
    {
    idx = blobIt.GetIndex();
    sum = 0;
    for ( i = 0; i < ImageDimension; i++ )
      {
      dx = ( idx[i] - blobOrigin[i] ) * spacing[i];
      sum += dx * dx;
      }
    blobIt.Set ( static_cast< ImagePixelType >( vcl_exp (sum * T) ) );
    }
  return ( blob );
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
typename GaussianProfileMatchingImageFilter< TFeatureImage, TInputImage, TSegmentImage >::ImagePixelType
GaussianProfileMatchingImageFilter< TFeatureImage, TInputImage, TSegmentImage >
::PearsonCorrelation
  (ImageRegionType & region)
{
  int n(0);

  float x(0), y (0), xy (0), x2 (0), y2 (0);

  IteratorType inputIt1( m_Blob, m_Blob->GetLargestPossibleRegion() );

  FeatureConstIteratorType inputIt2(this->GetInput(), region);

  ImagePixelType        p1;
  FeatureImagePixelType p2;
  for ( inputIt1.GoToBegin(), inputIt2.GoToBegin(); !inputIt1.IsAtEnd();
        ++inputIt1, ++inputIt2, ++n )
    {
    p1 = inputIt1.Get();
    p2 = inputIt2.Get();
    x += p1;
    y += p2;
    xy += p1 * p2;
    x2 += p1 * p1;
    y2 += p2 * p2;
    }
  float d = ( n * x2 - x * x ) * ( n * y2 - y * y );

  if ( d == 0 )
    {
    return 0;
    }
  else
    {
    return ( ( n * xy ) - ( x * y ) ) / vcl_sqrt (d);
    }
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
GaussianProfileMatchingImageFilter< TFeatureImage, TInputImage, TSegmentImage >::EnlargeOutputRequestedRegion(
  DataObject *output)
{
  Superclass::EnlargeOutputRequestedRegion(output);
  output->SetRequestedRegionToLargestPossibleRegion();
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
GaussianProfileMatchingImageFilter< TFeatureImage, TInputImage, TSegmentImage >::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();
  if ( this->GetInput() )
    {
    FeatureImagePointer image = const_cast< FeatureImageType * >( this->GetInput() );
    image->SetRequestedRegionToLargestPossibleRegion();
    }
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
GaussianProfileMatchingImageFilter< TFeatureImage, TInputImage, TSegmentImage >::BeforeThreadedGenerateData()
{
  FeatureImageConstPointer input = this->GetInput();
  FeatureImageSizeType     inputSize = input->GetLargestPossibleRegion().GetSize();
  FeatureImageSpacingType  spacing = input->GetSpacing();

  for ( unsigned int i = 0; i < ImageDimension; i++ )
    {
    m_CellExtent[i] = static_cast< FeatureImageSizeValueType >
                        (2 * m_LargestCellRadius / spacing[i]);
    if ( m_CellExtent[i] % 2 == 1 )
      {
      m_CellExtent[i] += 1;
      }
    }

  // Define a image region to run windowing on
  FeatureImageIndexType start;
  FeatureImageSizeType  size;
  for ( unsigned int i = 0; i < ImageDimension; i++ )
    {
    start[i] = static_cast< FeatureImageSizeValueType >(m_CellExtent[i] / 2);
    size[i] = inputSize[i] - m_CellExtent[i] + 1;
    }
  m_ImageRegion.SetSize (size);
  m_ImageRegion.SetIndex (start);

  ImagePointer output = this->GetOutput();
  output->FillBuffer(-1.0);

  // Initialize Gaussian blob
  m_Blob = InitializeBlob (spacing, m_CellExtent);
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
GaussianProfileMatchingImageFilter< TFeatureImage, TInputImage, TSegmentImage >::AfterThreadedGenerateData()
{
  ImagePointer output = this->GetOutput();

  IndexIteratorType outIt( output, output->GetLargestPossibleRegion() );

  FeatureImageIndexType idx;
  outIt.GoToBegin();
  while ( !outIt.IsAtEnd() )
    {
    idx = outIt.GetIndex();
    if ( !m_ImageRegion.IsInside(idx) )
      {
      outIt.Set(0);
      }
    ++outIt;
    }
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
GaussianProfileMatchingImageFilter< TFeatureImage, TInputImage, TSegmentImage >::ThreadedGenerateData(
  const ImageRegionType & windowRegion,
  int threadId)
{
  (void)threadId;

  ImagePointer output = this->GetOutput();

  //Set window region size and start locations
  FeatureImageRegionType kernelRegion;

  kernelRegion.SetSize (m_CellExtent);

  IndexIteratorType outIt(output, windowRegion);

  FeatureImageIndexType idx, windowStart;
  ImagePixelType        p;

  outIt.GoToBegin();
  while ( !outIt.IsAtEnd() )
    {
    idx = outIt.GetIndex();
    if ( m_ImageRegion.IsInside(idx) )
      {
      for ( unsigned int i = 0; i < ImageDimension; i++ )
        {
        windowStart[i] = idx[i] - static_cast< ImageSizeValueType >(
          m_CellExtent[i] / 2);
        }
      kernelRegion.SetIndex (windowStart);

      p = PearsonCorrelation (kernelRegion);
      outIt.Set(p);
      }
    ++outIt;
    }
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
GaussianProfileMatchingImageFilter< TFeatureImage, TInputImage, TSegmentImage >::PrintSelf(std::ostream & os,
                                                                                           Indent indent) const
{
  Superclass::PrintSelf (os, indent);
  os << indent << "Class Name:        " << GetNameOfClass() << std::endl;
  os << indent << "SigmaForm:         " << GetSigmaForm() << std::endl;
  os << indent << "Largest cell radius: " << GetLargestCellRadius()
     << std::endl;
}
} /* end namespace itk */

#endif
