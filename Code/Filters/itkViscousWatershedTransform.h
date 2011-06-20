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

#ifndef __itkViscousWatershedTransform_h
#define __itkViscousWatershedTransform_h

#if defined( _MSC_VER )
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "GoFiltersConfigure.h"

#include "itkImageToImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkImageRegionIterator.h"

namespace itk
{
template< class TFeatureImage, class TInputImage, class TSegmentImage >
class GOFILTERS_EXPORT ViscousWatershedTransform:public ImageToImageFilter<
    TFeatureImage, TFeatureImage >
{
public:
  typedef ViscousWatershedTransform                          Self;
  typedef ImageToImageFilter< TFeatureImage, TFeatureImage > Superclass;
  typedef SmartPointer< Self >                               Pointer;
  typedef SmartPointer< const Self >                         ConstPointer;

  itkStaticConstMacro (ImageDimension, unsigned int,
                       TFeatureImage::ImageDimension);

  /** Method for creation through object factory */
  itkNewMacro (Self);

  /** Run-time type information */
  itkTypeMacro (ViscousWatershedTransform, ImageToImageFilter);

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

  typedef BinaryThresholdImageFilter< FeatureImageType, FeatureImageType >
  ThresholdFilterType;
  typedef typename ThresholdFilterType::Pointer ThresholdFilterPointer;

  typedef BinaryBallStructuringElement< int,
                                        ImageDimension >                  StructuringElementType;
  typedef BinaryMorphologicalClosingImageFilter< FeatureImageType, FeatureImageType,
                                                 StructuringElementType > ClosingFilterType;
  typedef typename ClosingFilterType::Pointer
                                                                          ClosingFilterPointer;
  typedef ImageRegionIterator< FeatureImageType >
                                                                          FeatureIteratorType;

  itkSetMacro (InitialLevel, unsigned int);
  itkGetConstMacro (InitialLevel, unsigned int);
  itkSetMacro (FinalLevel, unsigned int);
  itkGetConstMacro (FinalLevel, unsigned int);
  itkSetMacro (Increment, unsigned int);
  itkGetConstMacro (Increment, unsigned int);
  itkSetMacro (LargestRadius, unsigned int);
  itkGetConstMacro (LargestRadius, unsigned int);
  itkSetMacro (Slope, unsigned int);
  itkGetConstMacro (Slope, unsigned int);
protected:
  ViscousWatershedTransform();
  ~ViscousWatershedTransform() {}
  void GenerateData();

  unsigned int m_InitialLevel;
  unsigned int m_FinalLevel;
  unsigned int m_Increment;
  unsigned int m_LargestRadius;
  unsigned int m_Slope;
private:
  ViscousWatershedTransform (Self &); // intentionally not implemented
  void operator=(const Self &);       // intentionally not implemented
};
} /* namespace itk */

#include "itkViscousWatershedTransform.txx"
#endif
