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

#ifndef __itkMultiScaleLoGDistanceImageFilter_h
#define __itkMultiScaleLoGDistanceImageFilter_h

#include "GoFiltersConfigure.h"

#include "itkImageToImageFilter.h"
#include <itkLaplacianRecursiveGaussianImageFilter.h>
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkNumericTraits.h"
#include "itkPixelTraits.h"

namespace itk
{
/**\class MultiScaleLoGDistanceImageFilter
 * \brief 3D blob structures detection based on multiscale LoG filter and
 * distance map information.
 */
template< class InputImageType, class DistanceMapImageType = InputImageType, class OutputImageType = InputImageType >
class GOFILTERS_EXPORT MultiScaleLoGDistanceImageFilter:
  public ImageToImageFilter< InputImageType, OutputImageType >
{
public:
  /** Standard class typedefs. */
  typedef MultiScaleLoGDistanceImageFilter                      Self;
  typedef ImageToImageFilter< InputImageType, OutputImageType > Superclass;
  typedef SmartPointer< Self >                                  Pointer;
  typedef SmartPointer< const Self >                            ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MultiScaleLoGDistanceImageFilter, ImageToImageFilter);

  itkStaticConstMacro (ImageDimension, unsigned int,
                       InputImageType::ImageDimension);

  /** Some convenient typedefs. */
  typedef typename InputImageType::Pointer      InputImagePointer;
  typedef typename InputImageType::ConstPointer InputImageConstPointer;
  typedef typename InputImageType::PixelType    InputPixelType;

  typedef typename DistanceMapImageType::Pointer      DistanceMapImagePointer;
  typedef typename DistanceMapImageType::ConstPointer ConstDistanceMapImagePointer;
  typedef typename DistanceMapImageType::PixelType    DistanceMapPixelType;

  typedef typename OutputImageType::Pointer   OutputImagePointer;
  typedef typename OutputImageType::PixelType OutputPixelType;

  typedef ImageRegionConstIterator< InputImageType >       ConstInputIteratorType;
  typedef ImageRegionConstIterator< DistanceMapImageType > ConstDistanceMapIteratorType;
  typedef ImageRegionIterator< OutputImageType >           OutputIteratorType;
  typedef ImageRegionIterator< OutputImageType >           ConstLoGIteratorType;

  typedef itk::LaplacianRecursiveGaussianImageFilter< InputImageType, OutputImageType >
  LoGFilterType;
  typedef typename LoGFilterType::Pointer LoGFilterPointer;

  /** Set Squared Signed Distance Map(SDM) of the input image to process (mandatory) :
   * Distances must be expressed in image spacing units,
   * use SetUseImageSpacing of your distance map filter to 'true'
   * SDM must be negative inside blobs */
  void SetDistanceMap(const DistanceMapImagePointer distanceMap)
  {
    m_DistanceMap = distanceMap;
  }

  /** Define the minimum sigma of the Guassian filter for scale adapting */
  itkSetMacro(SigmaMin, double);
  /** Define the maximum sigma of the Guassian filter for scale adapting */
  itkSetMacro(SigmaMax, double);
  /** Returns the minimum sigma of the Guassian filter for sgeFilter.h:125:cale
    adapting */
  itkGetMacro(SigmaMin, double);
  /** Returns the maximum sigma of the Guassian filter for scale adapting */
  itkGetMacro(SigmaMax, double);

  /** Set Number of Scales (Scale steps : the LoG runs for each step)*/
  itkSetMacro(NumberOfSigmaSteps, int);
  /** Get Number of Scales (Scale steps : the LoG runs for each step)*/
  itkGetMacro(NumberOfSigmaSteps, int);

  /** Set if possible to compute outside of objects (in positive SDM regions)*/
  itkSetMacro(ComputeOutsideForeground, bool);
  /** Get if possible to compute outside of objects (in positive SDM regions)*/
  itkGetMacro(ComputeOutsideForeground, bool);
protected:
  // constructor destructor, itk functions...
  MultiScaleLoGDistanceImageFilter();
  ~MultiScaleLoGDistanceImageFilter() {}
  void PrintSelf(std::ostream & os, Indent indent) const;

  void GenerateData(void);

  // Variables
  double m_SigmaMin;
  double m_SigmaMax;
  double m_Sigma;
  int m_NumberOfSigmaSteps;
  bool m_ComputeOutsideForeground;
  ConstDistanceMapImagePointer m_DistanceMap;

  // LoG filter
  LoGFilterPointer m_LoGFilter;
private:
  //purposely not implemented
  MultiScaleLoGDistanceImageFilter(const Self &);
  void operator=(const Self &); //purposely not implemented

  void UpdateMaximumResponse(const int & scaleLevel);

  double ComputeSigmaValue(int scaleLevel);
};
} /* namespace itk */

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMultiScaleLoGDistanceImageFilter.txx"
#endif

#endif
