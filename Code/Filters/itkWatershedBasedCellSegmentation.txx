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

#ifndef __itkWatershedBasedCellSegmentation_txx
#define __itkWatershedBasedCellSegmentation_txx

#include "itkWatershedBasedCellSegmentation.h"

namespace itk
{
template< class TFeatureImage, class TInputImage, class TSegmentImage >
WatershedBasedCellSegmentation< TFeatureImage, TInputImage, TSegmentImage >
::WatershedBasedCellSegmentation()
{
  m_NucleusRadius          = 4.0;
  m_CorrelationKernelSigma = 4.0;
  m_NucleusThresholdMin    = 10;
  m_NucleusThresholdMax    = 30;
  m_CorrelationThreshold1  = 0.50;
  m_MembraneThreshold      = 256;
  m_Alpha = 1.5;
  m_Beta = 3.0;

  this->Superclass::SetNumberOfRequiredInputs (1);
  this->Superclass::SetNumberOfRequiredOutputs (1);

  this->Superclass::SetNthOutput ( 0, TSegmentImage::New() );
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
WatershedBasedCellSegmentation< TFeatureImage, TInputImage, TSegmentImage >::GenerateData()
{
  PreprocessFilterPointer preprocess = PreprocessFilterType::New();
  preprocess->SetInput( this->GetInput() );
  preprocess->SetLargestCellRadius ( m_NucleusRadius );
  preprocess->Update();

  FeatureImageConstPointer m_NucleiImg = preprocess->GetOutput();
  FeatureImageConstPointer m_MembraneImg = m_NucleiImg;

  SegmentImagePointer m_ForegroundImg;
    {
    // Apply Foreground filter
    ForegroundFilterPointer fgFilter = ForegroundFilterType::New();
    fgFilter->SetInput (0, m_NucleiImg);
    fgFilter->SetInput (1, m_MembraneImg);
    fgFilter->SetSigmaForm (m_CorrelationKernelSigma);   // in real coordinates
    fgFilter->SetThresholdCellmin (m_NucleusThresholdMin);
    fgFilter->SetThresholdCellmax (m_NucleusThresholdMax);
    fgFilter->SetThresholdMembrane (m_MembraneThreshold);
    fgFilter->SetThresholdForm (m_CorrelationThreshold1);
    fgFilter->SetLargestCellRadius (m_NucleusRadius);   // in real coordinates
    fgFilter->SetNumberOfThreads( this->GetNumberOfThreads() );
    fgFilter->Update();
    std::cout << "Computed foreground" << std::endl;

    m_ForegroundImg = fgFilter->GetOutput();
    m_ForegroundImg->DisconnectPipeline();
    }

  // Gradient weighted distance -- Todo: extend with blob instead of laplace
  DistanceFilterPointer distFilter = DistanceFilterType::New();
  distFilter->SetInput ( m_NucleiImg );
  distFilter->SetUseLevelSet( true );
  distFilter->SetForeground ( m_ForegroundImg );
  distFilter->SetLargestCellRadius( m_NucleusRadius );
  distFilter->SetNucleiSigma ( 0.5 );
  distFilter->SetAlpha( m_Alpha );
  distFilter->SetBeta( m_Beta );
  distFilter->Update();
  std::cout << "Computed distance map" << std::endl;

  typename MinMaxCalculatorType::Pointer minMax = MinMaxCalculatorType::New();
  minMax->SetImage( distFilter->GetOutput() );
  minMax->ComputeMaximum();
  double max = static_cast< double >( minMax->GetMaximum() );

  RInvertPointer idistance = RInvertType::New();
  idistance->SetInput( distFilter->GetOutput() );
  idistance->SetMaximum(max);
  idistance->Update();
  std::cout << "Inverted distance map" << std::endl;

  WatershedFilterPointer wshed = WatershedFilterType::New();
  wshed->SetInput( idistance->GetOutput() );
  wshed->SetMarkWatershedLine(false);
  wshed->SetLevel( 1.0 );
  wshed->FullyConnectedOn();
  wshed->SetNumberOfThreads( this->GetNumberOfThreads() );
  wshed->SetForegroundImage(m_ForegroundImg);
  wshed->Update();

  SegmentImagePointer output = wshed->GetOutput();
  output->DisconnectPipeline();

  SegmentImageSizeType size = output->GetLargestPossibleRegion().GetSize();
  SegmentImageIndexType index, index2;
  SegmentImageSizeType size2;
  for( unsigned int i = 0; i < ImageDimension; i++ )
  {
    index[i] = static_cast<SegmentImageIndexValueType>( size[i]/2 );
    index2[i] = 1;
    size2[i] = size[i] - 2;
  }
  SegmentImagePixelType label = output->GetPixel( index );

  SegmentImageRegionType region;
  region.SetIndex( index2 );
  region.SetSize( size2 );

  std::cout << "label: " << label << std::endl;

  SegmentIteratorType It( output, output->GetLargestPossibleRegion() );
  It.GoToBegin();
  if ( label > 0 )
  {
    while( !It.IsAtEnd() )
    {
      index = It.GetIndex();

      if ( (It.Get() ==  label ) && ( region.IsInside( index ) ) )
      {
        It.Set( 1 );
      }
      else
      {
        It.Set( 0 );
      }

      ++It;
    }
  }
  std::cout << "Computed watershed segmentation" << std::endl;

//   InputImagePointer smooth;
//   {
//     AntiAliasFilterPointer antialiaser = AntiAliasFilterType::New();
//     antialiaser->SetInput( output );//
//     antialiaser->SetMaximumRMSError( 1.0 );
//     antialiaser->SetNumberOfIterations( 100 );
//     antialiaser->SetInterpolateSurfaceLocation( true );
//     antialiaser->UseImageSpacingOn();
//     antialiaser->Update();
//     smooth = antialiaser->GetOutput();
//     smooth->DisconnectPipeline();
//   }
//
//   InputIteratorType iIt( smooth, smooth->GetLargestPossibleRegion() );
//   for( iIt.GoToBegin(), It.GoToBegin(); !It.IsAtEnd(); ++iIt, ++It )
//   {
//     if ( iIt.Get() > 0 )
//     {
//       It.Set( 1 );
//     }
//     else
//     {
//       It.Set( 0 );
//     }
//   }

  this->GraftOutput( output );
}

template< class TFeatureImage, class TInputImage, class TSegmentImage >
void
WatershedBasedCellSegmentation< TFeatureImage, TInputImage, TSegmentImage >::PrintSelf(std::ostream & os,
  Indent indent) const
{
  (void) os;
  (void) indent;
}

} /* end namespace itk */

#endif
