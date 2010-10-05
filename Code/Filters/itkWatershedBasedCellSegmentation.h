/*=========================================================================
  Author: $Author: krm15 $  // Author of last commit
  Version: $Rev: 1550 $  // Revision of last commit
  Date: $Date: 2010-06-06 23:50:34 -0400 (Sun, 06 Jun 2010) $  // Date of last commit
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

#ifndef __itkWatershedBasedCellSegmentation_h
#define __itkWatershedBasedCellSegmentation_h

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageToImageFilter.h"
#include "itkCellForegroundExtraction.h"
#include "itkGradientWeightedDistanceImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkMorphologicalWatershedImageFilter2.h"
#include "itkImageRegionIterator.h"
#include "itkMinimumMaximumImageCalculator.h"

namespace itk
{
template < class TFeatureImage, class TInputImage, class TSegmentImage >
class ITK_EXPORT WatershedBasedCellSegmentation : public ImageToImageFilter<
  TFeatureImage, TSegmentImage >
{
  public:
    typedef WatershedBasedCellSegmentation                          Self;
    typedef ImageToImageFilter< TFeatureImage,TSegmentImage > Superclass;
    typedef SmartPointer< Self >                              Pointer;
    typedef SmartPointer< const Self >                        ConstPointer;

    itkStaticConstMacro ( ImageDimension, unsigned int,
                          TFeatureImage::ImageDimension );

    /** Method for creation through object factory */
    itkNewMacro ( Self );

    /** Run-time type information */
    itkTypeMacro ( WatershedBasedCellSegmentation, ImageToImageFilter );

    /** Display */
    void PrintSelf ( std::ostream& os, Indent indent ) const;

    typedef TFeatureImage                           FeatureImageType;
    typedef typename FeatureImageType::Pointer      FeatureImagePointer;
    typedef typename FeatureImageType::ConstPointer FeatureImageConstPointer;
    typedef typename FeatureImageType::PixelType    FeatureImagePixelType;
    typedef typename FeatureImageType::RegionType   FeatureImageRegionType;
    typedef typename FeatureImageType::SizeType     FeatureImageSizeType;
    typedef typename FeatureImageSizeType::SizeValueType FeatureImageSizeValueType;
    typedef typename FeatureImageType::SpacingType  FeatureImageSpacingType;
    typedef typename FeatureImageType::IndexType    FeatureImageIndexType;
    typedef typename FeatureImageType::PointType    FeatureImagePointType;

    typedef TInputImage                           InputImageType;
    typedef typename InputImageType::Pointer      InputImagePointer;
    typedef typename InputImageType::ConstPointer InputImageConstPointer;
    typedef typename InputImageType::PixelType    InputImagePixelType;
    typedef typename InputImageType::RegionType   InputImageRegionType;
    typedef typename InputImageType::SizeType     InputImageSizeType;
    typedef typename InputImageSizeType::SizeValueType InputImageSizeValueType;
    typedef typename InputImageType::SpacingType  InputImageSpacingType;
    typedef typename InputImageType::IndexType    InputImageIndexType;
    typedef typename InputImageType::PointType    InputImagePointType;

    typedef TSegmentImage                           SegmentImageType;
    typedef typename SegmentImageType::Pointer      SegmentImagePointer;
    typedef typename SegmentImageType::ConstPointer SegmentImageConstPointer;
    typedef typename SegmentImageType::IndexType    SegmentImageIndexType;
    typedef typename SegmentImageType::PixelType    SegmentImagePixelType;

    typedef CellForegroundExtraction< FeatureImageType, InputImageType, SegmentImageType >
      ForegroundFilterType;
    typedef typename ForegroundFilterType::Pointer ForegroundFilterPointer;
    
    typedef GradientWeightedDistanceImageFilter< FeatureImageType, InputImageType, SegmentImageType >
      DistanceFilterType;
    typedef typename DistanceFilterType::Pointer DistanceFilterPointer;
    
    typedef InvertIntensityImageFilter< InputImageType, InputImageType >
      RInvertType;
    typedef typename RInvertType::Pointer RInvertPointer;
    
    typedef MorphologicalWatershedImageFilter2< InputImageType, SegmentImageType >
      WatershedFilterType;
    typedef typename WatershedFilterType::Pointer WatershedFilterPointer;
    
    typedef MinimumMaximumImageCalculator< InputImageType >  MinMaxCalculatorType;
    
    typedef ImageRegionIterator< FeatureImageType > FeatureIteratorType;
    typedef ImageRegionIterator< SegmentImageType > SegmentIteratorType;
    
    itkGetConstMacro ( NucleusRadius,double );
    itkSetMacro ( NucleusRadius,double );
    itkGetConstMacro ( CorrelationKernelSigma,double );
    itkSetMacro ( CorrelationKernelSigma,double );
    itkGetConstMacro ( NucleusThresholdMin,double );
    itkSetMacro ( NucleusThresholdMin,double );
    itkGetConstMacro ( NucleusThresholdMax,double );
    itkSetMacro ( NucleusThresholdMax,double );
    itkGetConstMacro ( CorrelationThreshold1,double );
    itkSetMacro ( CorrelationThreshold1,double );
    itkGetConstMacro ( MembraneThreshold,double );
    itkSetMacro ( MembraneThreshold,double );
    itkGetConstMacro ( Alpha, float );
    itkSetMacro ( Alpha, float );
    itkGetConstMacro ( Beta,float );
    itkSetMacro ( Beta,float );
    
  protected:
    WatershedBasedCellSegmentation();
    ~WatershedBasedCellSegmentation() {}
    void GenerateData();
    
    double m_NucleusRadius;
    double m_CorrelationKernelSigma;
    int m_NucleusThresholdMin;
    int m_NucleusThresholdMax;
    double m_CorrelationThreshold1;
    int m_MembraneThreshold;
    float m_Alpha;
    float m_Beta;

  private:
    WatershedBasedCellSegmentation ( Self& );   // intentionally not implemented
    void operator= ( const Self& );   // intentionally not implemented
  };

} /* namespace itk */

#include "itkWatershedBasedCellSegmentation.txx"
#endif
