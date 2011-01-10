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

#ifndef __itkCellForegroundExtraction_h
#define __itkCellForegroundExtraction_h

#if defined( _MSC_VER )
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkAffineTransform.h"
#include "itkImageToImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkAffineTransform.h"
#include "itkGrayscaleFillholeImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkGaussianProfileMatchingImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegion.h"
#include "itkRegion.h"
#include "itkIndex.h"
#include "itkSize.h"

namespace itk
{
template< class TFeatureImage, class TInputImage, class TSegmentImage >
class ITK_EXPORT CellForegroundExtraction:public ImageToImageFilter<
    TFeatureImage, TSegmentImage >
{
public:
  typedef CellForegroundExtraction                           Self;
  typedef ImageToImageFilter< TFeatureImage, TSegmentImage > Superclass;
  typedef SmartPointer< Self >                               Pointer;
  typedef SmartPointer< const Self >                         ConstPointer;

  itkStaticConstMacro (ImageDimension, unsigned int,
                       TFeatureImage::ImageDimension);

  /** Method for creation through object factory */
  itkNewMacro (Self);

  /** Run-time type information */
  itkTypeMacro (CellForegroundExtraction, ImageToImageFilter);

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

  typedef RegionOfInterestImageFilter< FeatureImageType, FeatureImageType >
  ROIFilterType;
  typedef typename ROIFilterType::Pointer ROIFilterPointer;

  typedef CastImageFilter< FeatureImageType, FeatureImageType > CastFilterType;
  typedef typename CastFilterType::Pointer                      CastFilterPointer;

  typedef ResampleImageFilter< FeatureImageType, FeatureImageType > ResampleFeatureFilterType;
  typedef typename ResampleFeatureFilterType::Pointer               ResampleFeatureFilterPointer;

  typedef LinearInterpolateImageFunction< FeatureImageType > InterpolatorFeatureType;
  typedef typename InterpolatorFeatureType::Pointer          InterpolatorFeaturePointer;

  typedef ResampleImageFilter< ImageType, ImageType > ResampleFilterType;
  typedef typename ResampleFilterType::Pointer        ResampleFilterPointer;

  typedef LinearInterpolateImageFunction< ImageType > InterpolatorType;
  typedef typename InterpolatorType::Pointer          InterpolatorPointer;

  typedef AffineTransform< double, ImageDimension > TransformType;
  typedef typename TransformType::Pointer           TransformPointer;
  typedef RescaleIntensityImageFilter< ImageType, ImageType >
  RescaleFilterType;
  typedef typename RescaleFilterType::Pointer RescaleFilterPointer;

  typedef GaussianProfileMatchingImageFilter< TFeatureImage,
                                              TInputImage, TSegmentImage > GaussProfileFilterType;
  typedef typename GaussProfileFilterType::Pointer GaussProfileFilterPointer;

  typedef ImageRegionConstIterator< FeatureImageType >     FeatureConstIteratorType;
  typedef ImageRegionIteratorWithIndex< FeatureImageType > FeatureIndexIteratorType;
  typedef ImageRegionIterator< ImageType >                 IteratorType;
  typedef ImageRegionIterator< SegmentImageType >          SegmentIteratorType;
  typedef ImageRegionIteratorWithIndex< ImageType >        IndexIteratorType;

  itkGetConstMacro (SigmaForm, double);
  itkSetMacro (SigmaForm, double);
  itkGetConstMacro (ThresholdCellmin, double);
  itkSetMacro (ThresholdCellmin, double);
  itkGetConstMacro (ThresholdCellmax, double);
  itkSetMacro (ThresholdCellmax, double);
  itkGetConstMacro (ThresholdMembrane, double);
  itkSetMacro (ThresholdMembrane, double);
  itkGetConstMacro (ThresholdForm, double);
  itkSetMacro (ThresholdForm, double);
  itkGetConstMacro (LargestCellRadius, double);
  itkSetMacro (LargestCellRadius, double);

  void SetSampling(float *sampling)
  {
    m_Sampling = sampling;
  }

  ImagePointer GetGaussCorrImage(void) const
  {
    return gauss;
  }

protected:
  CellForegroundExtraction();
  ~CellForegroundExtraction() {}

  FeatureImagePointer ResampleInput(FeatureImageConstPointer input, FeatureImageSpacingType spacing,
                                    FeatureImageSizeType size, FeatureImagePointType origin);

  ImagePointer Resample(ImagePointer input, FeatureImageSpacingType spacing,
                        FeatureImageSizeType size, FeatureImagePointType origin);

  void GenerateData();

  double m_SigmaForm;
  double m_ThresholdCellmin;
  double m_ThresholdCellmax;
  double m_ThresholdMembrane;   // Change to size threshold
  double m_ThresholdForm;
  double m_LargestCellRadius;
  float *m_Sampling;
  ImagePointer gauss;
private:
  CellForegroundExtraction (Self &);  // intentionally not implemented
  void operator=(const Self &);       // intentionally not implemented
};
} /* namespace itk */

#include "itkCellForegroundExtraction.txx"
#endif
