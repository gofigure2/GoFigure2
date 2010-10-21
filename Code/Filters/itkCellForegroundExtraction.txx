/*=========================================================================
  Author: $Author: krm15 $  // Author of last commit
  Version: $Rev: 1658 $  // Revision of last commit
  Date: $Date: 2010-06-14 15:49:25 -0400 (Mon, 14 Jun 2010) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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

#ifndef __itkCellForegroundExtraction_txx
#define __itkCellForegroundExtraction_txx

#include "itkCellForegroundExtraction.h"

namespace itk
{
template< class TFeatureImage, class TInputImage, class TSegmentImage >
CellForegroundExtraction< TFeatureImage, TInputImage, TSegmentImage >
::CellForegroundExtraction():m_SigmaForm (2.0),
  m_ThresholdCellmin (10), m_ThresholdCellmax (30),  m_ThresholdMembrane (0),
  m_ThresholdForm (0.15), m_LargestCellRadius (4.0)
{
  m_Sampling = 0;
  this->Superclass::SetNumberOfRequiredInputs (2);
  this->Superclass::SetNumberOfRequiredOutputs (1);

  this->Superclass::SetNthOutput ( 0, TSegmentImage::New() );
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
typename CellForegroundExtraction< TFeatureImage, TInputImage, TSegmentImage >::FeatureImagePointer
CellForegroundExtraction< TFeatureImage, TInputImage, TSegmentImage >
::ResampleInput
  (FeatureImageConstPointer input, FeatureImageSpacingType spacing,
  FeatureImageSizeType size, FeatureImagePointType origin)
{
  // create the resample filter, transform and interpolator
  TransformPointer transform = TransformType::New();

  transform->SetIdentity();

  InterpolatorFeaturePointer interp = InterpolatorFeatureType::New();

  ResampleFeatureFilterPointer m_resamp = ResampleFeatureFilterType::New();
  m_resamp->SetTransform (transform);
  m_resamp->SetInterpolator (interp);
  m_resamp->SetInput (input);
  m_resamp->SetSize (size);
  m_resamp->SetOutputOrigin (origin);
  m_resamp->SetOutputSpacing (spacing);
  m_resamp->SetDefaultPixelValue (0);
  m_resamp->Update();

  return ( m_resamp->GetOutput() );
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
typename CellForegroundExtraction< TFeatureImage, TInputImage, TSegmentImage >::ImagePointer
CellForegroundExtraction< TFeatureImage, TInputImage, TSegmentImage >
::Resample
  (ImagePointer input, FeatureImageSpacingType spacing,
  FeatureImageSizeType size, FeatureImagePointType origin)
{
  // create the resample filter, transform and interpolator
  TransformPointer transform = TransformType::New();

  transform->SetIdentity();

  InterpolatorPointer interp = InterpolatorType::New();

  ResampleFilterPointer m_resamp = ResampleFilterType::New();
  m_resamp->SetTransform (transform);
  m_resamp->SetInterpolator (interp);
  m_resamp->SetInput (input);
  m_resamp->SetSize (size);
  m_resamp->SetOutputOrigin (origin);
  m_resamp->SetOutputSpacing (spacing);
  m_resamp->SetDefaultPixelValue (0);
  m_resamp->Update();

  return ( m_resamp->GetOutput() );
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
CellForegroundExtraction< TFeatureImage, TInputImage, TSegmentImage >::GenerateData()
{
  FeatureImageConstPointer cellImg = this->GetInput();
  FeatureImageSpacingType  spacing = cellImg->GetSpacing();
  FeatureImageSizeType     size = cellImg->GetLargestPossibleRegion().GetSize();
  FeatureImagePointType    origin = cellImg->GetOrigin();

  FeatureImageSpacingType subSpacing;
  FeatureImageSizeType    subSize;

  float t;

  for ( unsigned int i = 0; i < ImageDimension; i++ )
    {
    if ( m_Sampling )
      {
      t = m_Sampling[i];
      }
    else
      {
      t = 1.0;
      }
    subSpacing[i] = spacing[i] * t;
    subSize[i]    = static_cast< ImageSizeValueType >(size[i] / t);
    }

    {
    ImagePointer gaussCorrImg;
      {
      FeatureImagePointer rawSubsampledImage = ResampleInput (
        cellImg, subSpacing, subSize, origin);

      GaussProfileFilterPointer gaussCorr = GaussProfileFilterType::New();
      gaussCorr->SetInput(rawSubsampledImage);
      gaussCorr->SetSigmaForm(m_SigmaForm);
      gaussCorr->SetNumberOfThreads( this->GetNumberOfThreads() );
      try
        {
        gaussCorr->Update();
        }
      catch (itk::ExceptionObject e)
        {
        std::cerr << "Error: " << e << std::endl;
        }

      gaussCorrImg = Resample (gaussCorr->GetOutput(), spacing, size, origin);
      }

    RescaleFilterPointer m_rescale = RescaleFilterType::New();
    m_rescale->SetInput (gaussCorrImg);
    m_rescale->SetOutputMinimum (0);
    m_rescale->SetOutputMaximum (1);
    m_rescale->Update();

    gauss = m_rescale->GetOutput();
    gauss->DisconnectPipeline();
    }

  FeatureImageConstPointer membraneImg = this->GetInput(1);

  SegmentImagePointer fg = SegmentImageType::New();
  fg->SetRegions( cellImg->GetLargestPossibleRegion() );
  fg->CopyInformation(cellImg);
  fg->Allocate();
  fg->FillBuffer(0);

  // We reuse the space allocated in cellImg to now represent the foreground
  // Fuzzy thresholding operation
  SegmentIteratorType oIt( fg, fg->GetLargestPossibleRegion() );

  IteratorType GaussIt( gauss, gauss->GetLargestPossibleRegion() );

  FeatureConstIteratorType mIt( membraneImg, membraneImg->GetLargestPossibleRegion() );

  FeatureConstIteratorType It( cellImg, cellImg->GetLargestPossibleRegion() );

  oIt.GoToBegin();
  It.GoToBegin();
  GaussIt.GoToBegin();
  mIt.GoToBegin();
  while ( !It.IsAtEnd() )
    {
    if ( ( It.Get()  > m_ThresholdCellmax )
         && ( mIt.Get() < m_ThresholdMembrane ) )
      {
      oIt.Set (1);
      }
    else if ( ( It.Get()      > m_ThresholdCellmin )
              && ( GaussIt.Get() > m_ThresholdForm )
              && ( mIt.Get() < m_ThresholdMembrane ) )
      {
      oIt.Set (1);
      }
    else
      {
      oIt.Set (0);
      }
    ++oIt;
    ++It;
    ++GaussIt;
    ++mIt;
    }

  this->GraftOutput (fg);
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
CellForegroundExtraction< TFeatureImage, TInputImage, TSegmentImage >::PrintSelf(std::ostream & os,
                                                                                 Indent indent) const
{
  Superclass::PrintSelf (os, indent);
  os << indent << "Class Name:        " << GetNameOfClass() << std::endl;
  os << indent << "SigmaForm:         " << GetSigmaForm() << std::endl;
  os << indent << "ThresholdCellmin:  " << GetThresholdCellmin() << std::endl;
  os << indent << "ThresholdCellmax:  " << GetThresholdCellmax() << std::endl;
  os << indent << "ThresholdMembrane: " << GetThresholdMembrane()
     << std::endl;
  os << indent << "ThresholdForm:     " << GetThresholdForm() << std::endl;
  os << indent << "Largest cell radius: " << GetLargestCellRadius()
     << std::endl;
  os << indent << "Sampling factors:  ";
  for ( unsigned int i = 0; i < ImageDimension; i++ )
    {
    std::cout << ( this->m_Sampling )[i] << ' ';
    }
  std::cout << std::endl;
}
} /* end namespace itk */

#endif
