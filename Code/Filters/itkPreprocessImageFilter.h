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
#ifndef __itkPreprocessImageFilter_h
#define __itkPreprocessImageFilter_h

#if defined( _MSC_VER )
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageToImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkGrayscaleFillholeImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"

namespace itk
{
/** \class PreprocessImageFilter
*  \brief Denoise images - remove median noise and perform morphological
* reconstruction. Makes it easier to segment and prevents formation of holes
* in the segmentation.
*/
template< class TInputImage, class TOutputImage = TInputImage >
class ITK_EXPORT PreprocessImageFilter:public ImageToImageFilter<
    TInputImage, TOutputImage >
{
public:

  typedef PreprocessImageFilter                                  Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

  itkStaticConstMacro (ImageDimension, unsigned int,
                       TInputImage::ImageDimension);

  /** Method for creation through object factory */
  itkNewMacro (Self);

  /** Run-time type information */
  itkTypeMacro (PreprocessImageFilter, ImageToImageFilter);

  /** Display */
  void PrintSelf(std::ostream & os, Indent indent) const;

  typedef Image< float, ImageDimension >        ImageType;
  typedef typename ImageType::Pointer           ImagePointer;
  typedef typename ImageType::ConstPointer      ImageConstPointer;
  typedef typename ImageType::PixelType         ImagePixelType;
  typedef typename ImageType::RegionType        ImageRegionType;
  typedef typename ImageType::SizeType          ImageSizeType;
  typedef typename ImageSizeType::SizeValueType ImageSizeValueType;

  typedef typename ImageType::SpacingType ImageSpacingType;
  typedef typename ImageType::IndexType   ImageIndexType;
  typedef typename ImageType::PointType   ImagePointType;

  typedef CastImageFilter< TInputImage, ImageType > InputCastType;
  typedef typename InputCastType::Pointer           InputCastPointer;

  typedef MedianImageFilter< ImageType, ImageType > MedianFilterType;
  typedef typename MedianFilterType::Pointer        MedianFilterPointer;
  typedef RecursiveGaussianImageFilter< ImageType, ImageType >
  SmoothingFilterType;
  typedef typename SmoothingFilterType::Pointer                SmoothingFilterPointer;
  typedef GrayscaleFillholeImageFilter< ImageType, ImageType > GrayscaleFillholeFilterType;
  typedef typename GrayscaleFillholeFilterType::Pointer        GrayscaleFillholePointer;
  typedef CastImageFilter< ImageType, TOutputImage >           OutputCastType;
  typedef typename OutputCastType::Pointer                     OutputCastPointer;

  itkGetConstMacro (LargestCellRadius, double);
  itkSetMacro (LargestCellRadius, double);
protected:

  PreprocessImageFilter();
  ~PreprocessImageFilter() {}

  void GenerateData();

  double m_LargestCellRadius;
private:
  PreprocessImageFilter (Self &);        // intentionally not implemented
  void operator=(const Self &);   // intentionally not implemented
};
}   /* namespace itk */

#include "itkPreprocessImageFilter.txx"
#endif
