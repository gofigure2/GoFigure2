/*=========================================================================
  Author: $Author: krm15 $  // Author of last commit
  Version: $Rev: 738 $  // Revision of last commit
  Date: $Date: 2009-10-10 23:59:10 -0400 (Sat, 10 Oct 2009) $  // Date of last commit
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

#ifndef __itkSingleCellSplitImageFilter_h
#define __itkSingleCellSplitImageFilter_h

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageToImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkDanielssonDistanceMapImageFilter.h"

#include <list>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>

namespace itk
{
template < class TInputImage >
class ITK_EXPORT SingleCellSplitImageFilter : public ImageToImageFilter<
        TInputImage, TInputImage >
{
  public:
    typedef SingleCellSplitImageFilter             Self;
    typedef ImageToImageFilter< TInputImage,TInputImage > Superclass;
    typedef SmartPointer<Self>                        Pointer;
    typedef SmartPointer<const Self>                  ConstPointer;

    itkStaticConstMacro ( ImageDimension, unsigned int,
      TInputImage::ImageDimension );

    /** Method for creation through object factory */
    itkNewMacro ( Self );

    /** Run-time type information */
    itkTypeMacro ( SingleCellSplitImageFilter, ImageToImageFilter );

    /** Display */
    void PrintSelf ( std::ostream& os, Indent indent ) const;

    typedef TInputImage                      ImageType;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::PixelType    ImagePixelType;
    typedef typename ImageType::PointType    ImagePointType;
    typedef typename ImageType::IndexType    ImageIndexType;

    typedef ImageRegionIterator< ImageType > IteratorType;
    typedef ImageRegionConstIterator< ImageType > ConstIteratorType;
    
    typedef DanielssonDistanceMapImageFilter< ImageType, ImageType > DistanceFilterType;
    typedef typename DistanceFilterType::Pointer DistanceFilterPointer;

    typedef std::list< ImagePointType > SeedListType;
    typedef typename SeedListType::iterator ListIteratorType;
    
    itkGetConstMacro ( ForegroundValue, ImagePixelType );
    itkSetMacro ( ForegroundValue, ImagePixelType );
    
    SeedListType m_Seeds;
    
    void SetSeedImage( ImagePointer in )
    {
      m_SeedImage = in;
    }
    
  protected:
    SingleCellSplitImageFilter();
    ~SingleCellSplitImageFilter() {}
    void GenerateData();
    
    ImagePointer m_SeedImage;
    ImagePixelType m_ForegroundValue;

  private:
    SingleCellSplitImageFilter ( Self& );   // intentionally not implemented
    void operator= ( const Self& );       // intentionally not implemented
};

} /* namespace itk */

#include "itkSingleCellSplitImageFilter.txx"
#endif
