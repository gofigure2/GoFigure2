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


/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMorphologicalWatershedImageFilter2.h,v $
  Language:  C++
  Date:      $Date: 2009-04-23 03:43:42 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMorphologicalWatershedImageFilter2_h
#define __itkMorphologicalWatershedImageFilter2_h

#include "itkImageToImageFilter.h"

namespace itk
{
/** \class MorphologicalWatershedImageFilter2
 * \brief
 *
 * \todo (Kishore) write a brief documentation!
 *
 * Watershed pixel are labeled 0.
 * TOutputImage should be an integer type.
 * Labels of output image are in no particular order. You can reorder the
 * labels such that object labels are consecutive and sorted based on object
 * size by passing the output of this filter to a RelabelComponentImageFilter.
 *
 * The morphological watershed transform algorithm is described in
 * Chapter 9.2 of Pierre Soille's book "Morphological Image Analysis:
 * Principles and Applications", Second Edition, Springer, 2003.
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * \sa WatershedImageFilter, MorphologicalWatershedFromMarkersImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */
template< class TInputImage, class TOutputImage >
class ITK_EXPORT MorphologicalWatershedImageFilter2:
  public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef MorphologicalWatershedImageFilter2              Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

  /** Some convenient typedefs. */
  typedef TInputImage                            InputImageType;
  typedef TOutputImage                           OutputImageType;
  typedef typename InputImageType::Pointer       InputImagePointer;
  typedef typename InputImageType::ConstPointer  InputImageConstPointer;
  typedef typename InputImageType::RegionType    InputImageRegionType;
  typedef typename InputImageType::PixelType     InputImagePixelType;
  typedef typename OutputImageType::Pointer      OutputImagePointer;
  typedef typename OutputImageType::ConstPointer OutputImageConstPointer;
  typedef typename OutputImageType::RegionType   OutputImageRegionType;
  typedef typename OutputImageType::PixelType    OutputImagePixelType;

  /** ImageDimension constants */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(MorphologicalWatershedImageFilter2,
               ImageToImageFilter);

  /**
   * Set/Get whether the connected components are defined strictly by
   * face connectivity or by face+edge+vertex connectivity.  Default is
   * FullyConnectedOff.  For objects that are 1 pixel wide, use
   * FullyConnectedOn.
   */
  itkSetMacro(FullyConnected, bool);
  itkGetConstReferenceMacro(FullyConnected, bool);
  itkBooleanMacro(FullyConnected);

  /**
   * Set/Get whether the watershed pixel must be marked or not. Default
   * is true. Set it to false do not only avoid writing watershed pixels,
   * it also decrease algorithm complexity.
   */
  itkSetMacro(MarkWatershedLine, bool);
  itkGetConstReferenceMacro(MarkWatershedLine, bool);
  itkBooleanMacro(MarkWatershedLine);

  /**
   */
  itkSetMacro(Level, InputImagePixelType);
  itkGetConstMacro(Level, InputImagePixelType);

  /** Set the marker image */
  void SetForegroundImage(TOutputImage *fg)
  {
    m_ForegroundImg = fg;
  }

protected:
  MorphologicalWatershedImageFilter2();
  ~MorphologicalWatershedImageFilter2() {}
  void PrintSelf(std::ostream & os, Indent indent) const;

  /** MorphologicalWatershedImageFilter2 needs the entire input be
   * available. Thus, it needs to provide an implementation of
   * GenerateInputRequestedRegion(). */
  void GenerateInputRequestedRegion();

  /** MorphologicalWatershedImageFilter2 will produce the entire output. */
  void EnlargeOutputRequestedRegion( DataObject *itkNotUsed(output) );

  /** Single-threaded version of GenerateData.  This filter delegates
   * to GrayscaleGeodesicErodeImageFilter. */
  void GenerateData();

  OutputImagePointer m_ForegroundImg;
private:
  MorphologicalWatershedImageFilter2(const Self &); //purposely not implemented
  void operator=(const Self &);                     //purposely not implemented

  bool m_FullyConnected;

  bool m_MarkWatershedLine;

  InputImagePixelType m_Level;
}; // end of class
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMorphologicalWatershedImageFilter2.txx"
#endif

#endif
