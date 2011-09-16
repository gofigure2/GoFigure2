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

#ifndef __itkGaussianProfileMatchingImageFilter_h
#define __itkGaussianProfileMatchingImageFilter_h

#if defined( _MSC_VER )
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "GoFiltersConfigure.h"

#include "itkImageToImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegion.h"
#include "itkRegion.h"
#include "itkIndex.h"
#include "itkSize.h"

namespace itk
{
template< class TFeatureImage, class TInputImage, class TSegmentImage >
class GOFILTERS_EXPORT GaussianProfileMatchingImageFilter:public ImageToImageFilter<
    TFeatureImage, TInputImage >
{
public:
  typedef GaussianProfileMatchingImageFilter               Self;
  typedef ImageToImageFilter< TFeatureImage, TInputImage > Superclass;
  typedef SmartPointer< Self >                             Pointer;
  typedef SmartPointer< const Self >                       ConstPointer;

  itkStaticConstMacro (ImageDimension, unsigned int,
                       TFeatureImage::ImageDimension);

  /** Method for creation through object factory */
  itkNewMacro (Self);

  /** Run-time type information */
  itkTypeMacro (GaussianProfileMatchingImageFilter, ImageToImageFilter);

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

  typedef ImageRegionConstIterator< FeatureImageType >          FeatureConstIteratorType;
  typedef ImageRegionConstIteratorWithIndex< FeatureImageType > FeatureIndexConstIteratorType;
  typedef ImageRegionIterator< ImageType >                      IteratorType;
  typedef ImageRegionIteratorWithIndex< ImageType >             IndexIteratorType;

  itkGetConstMacro (SigmaForm, double);
  itkSetMacro (SigmaForm, double);
  itkGetConstMacro (LargestCellRadius, double);
  itkSetMacro (LargestCellRadius, double);
protected:
  GaussianProfileMatchingImageFilter();
  ~GaussianProfileMatchingImageFilter() {}

  ImagePointer GaussianCorrelation(FeatureImagePointer rawImg);

  inline ImagePixelType
  PearsonCorrelation(ImageRegionType & region);

  ImagePointer InitializeBlob(FeatureImageSpacingType spacing, FeatureImageSizeType size);

  /** Method for evaluating the implicit function over the image. */
  virtual void BeforeThreadedGenerateData();

  virtual void AfterThreadedGenerateData();

  virtual void ThreadedGenerateData(const ImageRegionType & windowRegion,
#ifdef ITKv4
                                    ThreadIdType threadId );
#else
                                    int threadId);
#endif
  void GenerateInputRequestedRegion();

  void EnlargeOutputRequestedRegion( DataObject *itkNotUsed(output) );

  double m_SigmaForm;
  double m_LargestCellRadius;
  FeatureImageSizeType m_CellExtent;
  FeatureImageRegionType m_ImageRegion;
  ImagePointer m_Blob;
private:
  GaussianProfileMatchingImageFilter (Self &); // intentionally not implemented
  void operator=(const Self &);                // intentionally not implemented
};
} /* namespace itk */

#include "itkGaussianProfileMatchingImageFilter.txx"
#endif
