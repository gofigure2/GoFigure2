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

#ifndef __itkCellFeatureGenerator_h
#define __itkCellFeatureGenerator_h

#if defined( _MSC_VER )
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageToImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkAbsImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegion.h"
#include "itkRegion.h"
#include "itkIndex.h"
#include "itkSize.h"

#include "itkImageFileWriter.h"

namespace itk
{
template< class TFeatureImage, class TInputImage, class TSegmentImage >
class CellFeatureGenerator:
  public ImageToImageFilter< TFeatureImage, TInputImage >
{
public:
  typedef CellFeatureGenerator                             Self;
  typedef ImageToImageFilter< TFeatureImage, TInputImage > Superclass;
  typedef SmartPointer< Self >                             Pointer;
  typedef SmartPointer< const Self >                       ConstPointer;

  itkStaticConstMacro (ImageDimension, unsigned int,
                       TFeatureImage::ImageDimension);

  /** Method for creation through object factory */
  itkNewMacro (Self);

  /** Run-time type information */
  itkTypeMacro (CellFeatureGenerator, ImageToImageFilter);

  /** Display */
  void PrintSelf(std::ostream & os, Indent indent) const;

  typedef TFeatureImage                                FeatureImageType;
  typedef typename FeatureImageType::Pointer           FeatureImagePointer;
  typedef typename FeatureImageType::ConstPointer      FeatureImageConstPointer;
  typedef typename FeatureImageType::PixelType         FeatureImagePixelType;
  typedef typename FeatureImageType::RegionType        FeatureImageRegionType;
  typedef typename FeatureImageType::SizeType          FeatureImageSizeType;
  typedef typename FeatureImageSizeType::SizeValueType FeatureImageSizeValueType;
  typedef typename FeatureImageType::SpacingType       FeatureImageSpacingType;
  typedef typename FeatureImageType::IndexType         FeatureImageIndexType;
  typedef typename FeatureImageType::PointType         FeatureImagePointType;

  typedef TInputImage                           ImageType;
  typedef typename ImageType::Pointer           ImagePointer;
  typedef typename ImageType::ConstPointer      ImageConstPointer;
  typedef typename ImageType::PixelType         ImagePixelType;
  typedef typename ImageType::RegionType        ImageRegionType;
  typedef typename ImageType::SizeType          ImageSizeType;
  typedef typename ImageSizeType::SizeValueType ImageSizeValueType;
  typedef typename ImageType::SpacingType       ImageSpacingType;
  typedef typename ImageType::IndexType         ImageIndexType;
  typedef typename ImageType::PointType         ImagePointType;

  typedef TSegmentImage                           SegmentImageType;
  typedef typename SegmentImageType::Pointer      SegmentImagePointer;
  typedef typename SegmentImageType::ConstPointer SegmentImageConstPointer;
  typedef typename SegmentImageType::IndexType    SegmentImageIndexType;
  typedef typename SegmentImageType::PixelType    SegmentImagePixelType;

  typedef GradientMagnitudeRecursiveGaussianImageFilter< FeatureImageType, ImageType >
  GradientFilterType;
  typedef typename GradientFilterType::Pointer         GradientFilterPointer;
  typedef SigmoidImageFilter< ImageType, ImageType >   SigmoidFilterType;
  typedef typename SigmoidFilterType::Pointer          SigmoidFilterPointer;
  typedef AbsImageFilter< ImageType, ImageType >       AbsFilterType;
  typedef typename AbsFilterType::Pointer              AbsFilterPointer;
  typedef ThresholdImageFilter< ImageType >            ThreshFilterType;
  typedef typename ThreshFilterType::Pointer           ThreshFilterPointer;
  typedef ImageRegionConstIterator< FeatureImageType > ConstIteratorType;
  typedef ImageRegionIterator< ImageType >             IteratorType;
  typedef ImageRegionIteratorWithIndex< ImageType >    IndexIteratorType;
  typedef SignedMaurerDistanceMapImageFilter< SegmentImageType, ImageType >
  MaurerType;
  typedef typename MaurerType::Pointer                      MaurerPointer;
  typedef MinimumMaximumImageCalculator< ImageType >        MinMaxCalculatorType;
  typedef typename MinMaxCalculatorType::Pointer            MinMaxCalculatorPointer;
  typedef MinimumMaximumImageCalculator< FeatureImageType > FeatureMinMaxCalculatorType;
  typedef typename FeatureMinMaxCalculatorType::Pointer     FeatureMinMaxCalculatorPointer;

  itkGetConstMacro (NucleiSigma, double);
  itkSetMacro (NucleiSigma, double);
  itkGetConstMacro (MembraneSigma, double);
  itkSetMacro (MembraneSigma, double);
  itkGetConstMacro (LargestCellRadius, double);
  itkSetMacro (LargestCellRadius, double);
  itkGetConstMacro (DistanceMapWeight, double);
  itkSetMacro (DistanceMapWeight, double);
  itkGetConstMacro (NucleiGradientWeight, double);
  itkSetMacro (NucleiGradientWeight, double);
  itkGetConstMacro (MembraneWeight, double);
  itkSetMacro (MembraneWeight, double);

  void SetForeground(SegmentImagePointer fg)
  {
    m_ForegroundMap = fg;
  }

  ImagePointer GetDistanceMap()
  {
    return m_DistanceMap;
  }

  ImagePointer GetGradient()
  {
    return m_Gradient;
  }

protected:
  CellFeatureGenerator();
  ~CellFeatureGenerator() {}
  void DistanceMap();

  void Gradient();

  void Normalize();

  void GenerateData();

  double m_NucleiSigma;
  double m_MembraneSigma;
  double m_LargestCellRadius;
  double m_DistanceMapWeight;
  double m_NucleiGradientWeight;
  double m_MembraneWeight;

  SegmentImagePointer m_ForegroundMap;
  ImagePointer m_DistanceMap;
  ImagePointer m_Gradient;
private:

  CellFeatureGenerator (Self &);        // intentionally not implemented
  void operator=(const Self &);         // intentionally not implemented
};
}   /* namespace itk */

#include "itkCellFeatureGenerator.txx"
#endif
