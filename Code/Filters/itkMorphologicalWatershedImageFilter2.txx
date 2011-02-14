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
  Module:    $RCSfile: itkMorphologicalWatershedImageFilter2.txx,v $
  Language:  C++
  Date:      $Date: 2007-01-26 15:01:47 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMorphologicalWatershedImageFilter2_txx
#define __itkMorphologicalWatershedImageFilter2_txx

#include "itkMorphologicalWatershedImageFilter2.h"
#include "itkRegionalMinimaImageFilter.h"
#include "itkHMinimaImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkMorphologicalWatershedFromMarkersImageFilter2.h"
#include "itkNumericTraits.h"

namespace itk
{
template< class TInputImage, class TOutputImage >
MorphologicalWatershedImageFilter2< TInputImage, TOutputImage >
::MorphologicalWatershedImageFilter2()
{
  m_FullyConnected = false;
  m_MarkWatershedLine = true;
  m_Level = NumericTraits< InputImagePixelType >::Zero;

  m_ForegroundImg = 0;
}

template< class TInputImage, class TOutputImage >
void
MorphologicalWatershedImageFilter2< TInputImage, TOutputImage >
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // We need all the input.
  InputImagePointer input = const_cast< InputImageType * >( this->GetInput() );
  if ( !input )
    {
    return;
    }
  input->SetRequestedRegion( input->GetLargestPossibleRegion() );
}

template< class TInputImage, class TOutputImage >
void
MorphologicalWatershedImageFilter2< TInputImage, TOutputImage >
::EnlargeOutputRequestedRegion(DataObject *)
{
  this->GetOutput()
  ->SetRequestedRegion( this->GetOutput()->GetLargestPossibleRegion() );
}

template< class TInputImage, class TOutputImage >
void
MorphologicalWatershedImageFilter2< TInputImage, TOutputImage >
::GenerateData()
{
  // Create a process accumulator for tracking the progress of this minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();

  progress->SetMiniPipelineFilter(this);

  // Allocate the output
  this->AllocateOutputs();

  // h-minima filter to remove the smallest minima
  typedef HMinimaImageFilter< TInputImage, TInputImage > HMinimaType;
  typename HMinimaType::Pointer hmin;

  // Delegate to a R-Min filter to find the regional minima
  typedef RegionalMinimaImageFilter< TInputImage, TOutputImage > RMinType;
  typename RMinType::Pointer rmin = RMinType::New();
  rmin->SetInput( this->GetInput() );
  rmin->SetFullyConnected(m_FullyConnected);
  rmin->SetBackgroundValue(NumericTraits< OutputImagePixelType >::Zero);
  rmin->SetForegroundValue( NumericTraits< OutputImagePixelType >::max() );

  // label the components
  typedef ConnectedComponentImageFilter< TOutputImage, TOutputImage >
  ConnectedCompType;
  typename ConnectedCompType::Pointer label = ConnectedCompType::New();
  label->SetFullyConnected(m_FullyConnected);
  label->SetInput( rmin->GetOutput() );

  // the watershed
  typedef
  MorphologicalWatershedFromMarkersImageFilter2< TInputImage, TOutputImage >
  WatershedType;
  typename WatershedType::Pointer wshed = WatershedType::New();
  wshed->SetInput( this->GetInput() );
  wshed->SetMarkerImage( label->GetOutput() );
  wshed->SetFullyConnected(m_FullyConnected);
  wshed->SetMarkWatershedLine(m_MarkWatershedLine);

  if ( m_ForegroundImg )
    {
    wshed->SetForegroundImage (m_ForegroundImg);
    }

  if ( m_Level != NumericTraits< InputImagePixelType >::Zero )
    {
    // insert a h-minima filter to remove the smallest minima
    //
    hmin = HMinimaType::New();
    hmin->SetInput( this->GetInput() );
    hmin->SetHeight(m_Level);
    hmin->SetFullyConnected(m_FullyConnected);
    // replace the input of the r-min filter
    rmin->SetInput( hmin->GetOutput() );

    progress->RegisterInternalFilter(hmin, 0.4f);
    progress->RegisterInternalFilter(rmin, 0.1f);
    progress->RegisterInternalFilter(label, .2f);
    progress->RegisterInternalFilter(wshed, .3f);
    }
  else
    {
    // don't insert the h-minima to save some ressources
    progress->RegisterInternalFilter(rmin, 0.167f);
    progress->RegisterInternalFilter(label, .333f);
    progress->RegisterInternalFilter(wshed, .5f);
    }

  // run the algorithm
  // graft our output to the watershed filter to force the proper regions
  // to be generated
  wshed->GraftOutput( this->GetOutput() );

  wshed->Update();

  // graft the output of the watershed filter back onto this filter's
  // output. this is needed to get the appropriate regions passed
  // back.
  this->GraftOutput( wshed->GetOutput() );
}

template< class TInputImage, class TOutputImage >
void
MorphologicalWatershedImageFilter2< TInputImage, TOutputImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "FullyConnected: "  << m_FullyConnected << std::endl;
  os << indent << "MarkWatershedLine: "  << m_MarkWatershedLine << std::endl;
  os << indent << "Level: "
     << static_cast< typename NumericTraits< InputImagePixelType >::PrintType >(m_Level)
     << std::endl;
}

} // end namespace itk
#endif